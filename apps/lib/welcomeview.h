/*
    SPDX-FileCopyrightText: 2021 Jiří Wolker <woljiri@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef WELCOMEVIEW_H
#define WELCOMEVIEW_H

#include "ui_welcomeview.h"

#include <QFrame>
#include <QUrl>

class QListWidgetItem;
class QLabel;

class KRecentFilesAction;
class KateViewSpace;
class RecentItemsModel;
class RecentsListItemDelegate;

class WelcomeView : public QWidget, Ui::WelcomeView
{
    Q_OBJECT
public:
    explicit WelcomeView(QWidget *parent = nullptr);
    ~WelcomeView() override;

    void loadRecents();

Q_SIGNALS:
    void openClicked();
    void recentItemClicked(QUrl const &url);
    void forgetAllRecents();
    void forgetRecentItem(QUrl const &url);

private Q_SLOTS:
    void recentsItemActivated(QModelIndex const &index);
    void recentListChanged();

private:
    int recentsCount();

    RecentItemsModel *m_recentsModel;
    RecentsListItemDelegate *m_recentsItemDelegate;

    QLabel *m_noRecentsLabel;
};

#endif // WELCOMEVIEW_H
