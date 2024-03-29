/***************************************************************************
 *   This file is part of Kate build plugin                                *
 *   SPDX-FileCopyrightText: 2014 Kåre Särs <kare.sars@iki.fi>             *
 *                                                                         *
 *   SPDX-License-Identifier: LGPL-2.0-or-later                            *
 ***************************************************************************/

#pragma once

#include <QAbstractItemModel>
#include <QByteArray>
#include <limits>

class TargetModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    struct Command {
        QString name;
        QString buildCmd;
        QString runCmd;
    };

    struct TargetSet {
        TargetSet(const QString &_name, const QString &_workDir);
        QString name;
        QString workDir;
        QList<Command> commands;
    };

    enum RowType {
        RootRow,
        TargetSetRow,
        CommandRow,
    };
    Q_ENUM(RowType)

    enum TargetRoles {
        CommandRole = Qt::UserRole,
        CommandNameRole,
        WorkDirRole,
        SearchPathsRole,
        TargetSetNameRole,
        RowTypeRole,
        IsProjectTargetRole,
    };
    Q_ENUM(TargetRoles)

    explicit TargetModel(QObject *parent = nullptr);
    ~TargetModel() override;

    /** This function clears all the target-sets */
    void clear(bool setSessionFirst);

    /** This function returns the root item for the Session TargetSets */
    QModelIndex sessionRootIndex() const;

    /** This function returns the root item for the Project TargetSets */
    QModelIndex projectRootIndex() const;

public Q_SLOTS:

    /** This function insert a target set and returns the model-index of the newly
     * inserted target-set */
    QModelIndex insertTargetSetAfter(const QModelIndex &beforeIndex, const QString &setName, const QString &workDir);

    /** This function adds a new command to a target-set and returns the model index */
    QModelIndex addCommandAfter(const QModelIndex &beforeIndex, const QString &cmdName, const QString &buildCmd, const QString &runCmd);

    /** This function copies the target(-set) the model index points to and returns
     * the model index of the copy. */
    QModelIndex copyTargetOrSet(const QModelIndex &index);

    /** This function deletes the index */
    void deleteItem(const QModelIndex &index);

    /** This function deletes the target-set with the same name */
    void deleteProjectTargerts();

    void moveRowUp(const QModelIndex &index);
    void moveRowDown(const QModelIndex &index);

    const QList<TargetSet> sessionTargetSets() const;
    const QList<TargetSet> projectTargetSets() const;

Q_SIGNALS:
    void projectTargetChanged();

public:
    static constexpr quintptr InvalidIndex = std::numeric_limits<quintptr>::max();
    // Model-View model functions
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    struct RootNode {
        bool isProject = false;
        QList<TargetSet> targetSets;
    };

private:
    QList<RootNode> m_rootNodes;
};
