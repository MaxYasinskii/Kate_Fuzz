/*
    SPDX-FileCopyrightText: 2020 Waqar Ahmed <waqar.17a@gmail.com>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "kateurlbar.h"
#include "kateviewmanager.h"
#include "kateviewspace.h"

#include <KTextEditor/Document>
#include <KTextEditor/View>

#include <QAbstractListModel>
#include <QAction>
#include <QApplication>
#include <QDir>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QListView>
#include <QMenu>
#include <QMimeDatabase>
#include <QPainter>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QToolButton>
#include <QUrl>

class DirFilesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    DirFilesModel(QObject *parent = nullptr)
        : QAbstractListModel(parent)
    {
    }

    enum Role { FileInfo = Qt::UserRole + 1 };

    int rowCount(const QModelIndex & = {}) const override
    {
        return m_fileInfos.size();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid()) {
            return {};
        }

        const auto &fi = m_fileInfos.at(index.row());
        if (role == Qt::DisplayRole) {
            return fi.fileName();
        } else if (role == Qt::DecorationRole) {
            if (fi.isDir()) {
                return QIcon(QIcon::fromTheme(QStringLiteral("folder")));
            } else if (fi.isFile()) {
                return QIcon::fromTheme(QMimeDatabase().mimeTypeForFile(fi).iconName());
            }
        } else if (role == FileInfo) {
            return QVariant::fromValue(fi);
        }

        return {};
    }

    void setDir(const QDir &dir)
    {
        m_fileInfos.clear();

        beginResetModel();
        const auto fileInfos = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs | QDir::Hidden);
        for (const auto &fi : fileInfos) {
            if (fi.isDir()) {
                m_fileInfos << fi;
            } else if (QMimeDatabase().mimeTypeForFile(fi).inherits(QStringLiteral("text/plain"))) {
                m_fileInfos << fi;
            }
        }
        endResetModel();
    }

private:
    QList<QFileInfo> m_fileInfos;
};

class DirFilesList : public QMenu
{
    Q_OBJECT
public:
    DirFilesList(QWidget *parent)
        : QMenu(parent)
    {
        m_list.setModel(&m_model);
        m_list.setResizeMode(QListView::Adjust);
        m_list.setViewMode(QListView::ListMode);
        m_list.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_list.setFrameStyle(QFrame::NoFrame);

        auto *l = new QVBoxLayout(this);
        l->setContentsMargins({});
        l->addWidget(&m_list);

        connect(&m_list, &QListView::clicked, this, &DirFilesList::onClicked);
    }

    void setDir(const QDir &d)
    {
        m_model.setDir(d);
        updateGeometry();
    }

    void updateGeometry()
    {
        auto s = m_list.sizeHintForRow(0);
        auto c = m_model.rowCount();
        const auto h = s * c + (s / 2);
        const auto vScroll = m_list.verticalScrollBar();
        int w = m_list.sizeHintForColumn(0) + (vScroll ? vScroll->height() / 2 : 0);

        setFixedSize(qMin(w, 500), qMin(h, 600));
    }

    void onClicked(const QModelIndex &idx)
    {
        if (!idx.isValid()) {
            return;
        }
        const auto fi = idx.data(DirFilesModel::FileInfo).value<QFileInfo>();
        if (fi.isDir()) {
            setDir(QDir(fi.absoluteFilePath()));
        } else if (fi.isFile()) {
            const QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
            hide();
            Q_EMIT openUrl(url);
        }
    }

Q_SIGNALS:
    void openUrl(const QUrl &url);

private:
    QListView m_list;
    DirFilesModel m_model;
};

enum BreadCrumbRole {
    PathRole = Qt::UserRole + 1,
    IsFile = Qt::UserRole + 2,
};

class BreadCrumbDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    bool isDir(const QModelIndex &idx) const
    {
        return !idx.data(BreadCrumbRole::PathRole).toString().isEmpty();
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const override
    {
        auto option = opt;
        if (isDir(index) && option.state & QStyle::State_MouseOver) {
            painter->fillRect(opt.rect, option.palette.brush(QPalette::Inactive, QPalette::Highlight));
        }

        QStyledItemDelegate::paint(painter, opt, index);
    }

    QSize sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &idx) const override
    {
        constexpr int hMargin = 2;

        const auto str = idx.data(Qt::DisplayRole).toString();
        if (!str.isEmpty()) {
            auto size = QStyledItemDelegate::sizeHint(opt, idx);
            const int w = opt.fontMetrics.horizontalAdvance(str) + 8;
            size.rwidth() = w;

            if (idx.data(BreadCrumbRole::IsFile).toBool() && !idx.data(Qt::DecorationRole).isNull()) {
                size.rwidth() += 16 + hMargin + hMargin;
            }

            return size;
        } else if (!idx.data(Qt::DecorationRole).isNull()) {
            QSize s(16, 16);
            s = s.grownBy({hMargin, 0, hMargin, 0});
            return s;
        }
        return QStyledItemDelegate::sizeHint(opt, idx);
    }
};

class BreadCrumbView : public QListView
{
    Q_OBJECT
public:
    BreadCrumbView(QWidget *parent = nullptr)
        : QListView(parent)
    {
        setFlow(QListView::LeftToRight);
        setModel(&m_model);
        setFrameStyle(QFrame::NoFrame);
        setSelectionMode(QAbstractItemView::NoSelection);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setItemDelegate(new BreadCrumbDelegate(this));
        setEditTriggers(QAbstractItemView::NoEditTriggers);

        connect(qApp, &QApplication::paletteChanged, this, &BreadCrumbView::updatePalette, Qt::QueuedConnection);
        auto font = this->font();
        font.setPointSize(font.pointSize() - 1);
        setFont(font);
        updatePalette();

        connect(this, &QListView::clicked, this, &BreadCrumbView::onClicked);
    }

    void updatePalette()
    {
        auto pal = palette();
        pal.setBrush(QPalette::Base, parentWidget()->palette().window());
        setPalette(pal);
    }

    void setUrl(const QUrl &url)
    {
        if (url.isEmpty()) {
            return;
        }

        const QString s = url.toString(QUrl::NormalizePathSegments | QUrl::PreferLocalFile);
        const auto res = splittedUrl(s);

        const auto &file = res.first;
        const auto &dirs = res.second;

        m_model.clear();

        for (const auto &dir : dirs) {
            auto item = new QStandardItem(dir.name);
            item->setData(dir.path, BreadCrumbRole::PathRole);
            m_model.appendRow(item);

            auto sep = new QStandardItem(QIcon::fromTheme(QStringLiteral("arrow-right")), {});
            m_model.appendRow(sep);
        }

        const auto icon = QIcon::fromTheme(QMimeDatabase().mimeTypeForFile(s).iconName());
        auto fileRow = new QStandardItem(icon, file);
        fileRow->setData(true, BreadCrumbRole::IsFile);
        m_model.appendRow(fileRow);
    }

    void onClicked(const QModelIndex &idx)
    {
        auto path = idx.data(BreadCrumbRole::PathRole).toString();
        if (path.isEmpty()) {
            return;
        }

        auto pos = mapToGlobal(rectForIndex(idx).bottomLeft());

        QDir d(path);
        DirFilesList m(this);
        auto par = static_cast<KateUrlBar *>(parentWidget());
        connect(&m, &DirFilesList::openUrl, par, &KateUrlBar::openUrlRequested);
        m.setDir(d);
        m.exec(pos);
    }

private:
    struct DirNamePath {
        QString name;
        QString path;
    };

    std::pair<QString, QVector<DirNamePath>> splittedUrl(const QString &s)
    {
        const int slashIndex = s.lastIndexOf(QLatin1Char('/'));
        if (slashIndex == -1) {
            return {};
        }

        const QString fileName = s.mid(slashIndex + 1);

        QDir dir(s);
        QVector<DirNamePath> dirsList;
        while (dir.cdUp()) {
            if (dir.dirName().isEmpty()) {
                continue;
            }
            DirNamePath dnp{dir.dirName(), dir.absolutePath()};
            dirsList.push_back(dnp);
        }
        std::reverse(dirsList.begin(), dirsList.end());
        return {fileName, dirsList};
    }

    QStandardItemModel m_model;
};

KateUrlBar::KateUrlBar(KateViewSpace *parent)
    : QWidget(parent)
{
    setFixedHeight(24);
    setContentsMargins({});

    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins({});
    m_layout->setSpacing(0);

    m_breadCrumbView = new BreadCrumbView(this);
    m_layout->addWidget(m_breadCrumbView);

    auto *vm = parent->viewManger();
    connect(vm, &KateViewManager::viewChanged, this, &KateUrlBar::onViewChanged);

    connect(vm, &KateViewManager::showUrlNavBarChanged, this, [this, vm](bool show) {
        setHidden(!show);
        if (show) {
            onViewChanged(vm->activeView());
        }
    });

    setHidden(!vm->showUrlNavBar());
}

void KateUrlBar::onViewChanged(KTextEditor::View *v)
{
    if (!v) {
        hide();
        return;
    }

    auto *vm = static_cast<KateViewSpace *>(parentWidget())->viewManger();
    if (vm && !vm->showUrlNavBar()) {
        return;
    }

    const auto url = v->document()->url();
    if (url.isEmpty() || !url.isLocalFile()) {
        hide();
        return;
    }

    m_breadCrumbView->setUrl(v->document()->url());

    if (isHidden())
        show();
}

#include "kateurlbar.moc"
