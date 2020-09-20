/*  SPDX-License-Identifier: LGPL-2.0-or-later

    SPDX-FileCopyrightText: 2009 Joseph Wenninger <jowenn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef _KATE_RUNNING_INSTANCE_INFO_
#define _KATE_RUNNING_INSTANCE_INFO_

#include <QDBusConnection>
#include <QDBusInterface>
#include <QMap>
#include <QVariant>
#include <iostream>

class KateRunningInstanceInfo : public QObject
{
    Q_OBJECT

public:
    KateRunningInstanceInfo(const QString &serviceName_)
        : QObject()
        , valid(false)
        , serviceName(serviceName_)
        , dbus_if(new QDBusInterface(serviceName_,
                                     QStringLiteral("/MainApplication"),
                                     QString(), // I don't know why it does not work if I specify org.kde.Kate.Application here
                                     QDBusConnection::sessionBus(),
                                     this))
    {
        if (!dbus_if->isValid()) {
            std::cerr << qPrintable(QDBusConnection::sessionBus().lastError().message()) << std::endl;
        }
        QVariant a_s = dbus_if->property("activeSession");
        /*      std::cerr<<a_s.isValid()<<std::endl;
              std::cerr<<"property:"<<qPrintable(a_s.toString())<<std::endl;
              std::cerr<<qPrintable(QDBusConnection::sessionBus().lastError().message())<<std::endl;*/
        if (!a_s.isValid()) {
            sessionName = QStringLiteral("___NO_SESSION_OPENED__%1").arg(dummy_session++);
            valid = false;
        } else {
            if (a_s.toString().isEmpty()) {
                sessionName = QStringLiteral("___DEFAULT_CONSTRUCTED_SESSION__%1").arg(dummy_session++);
            } else {
                sessionName = a_s.toString();
            }
            valid = true;
        }
    }
    ~KateRunningInstanceInfo() override
    {
        delete dbus_if;
    }
    bool valid;
    const QString serviceName;
    QDBusInterface *dbus_if;
    QString sessionName;

private:
    static int dummy_session;
};

typedef QMap<QString, KateRunningInstanceInfo *> KateRunningInstanceMap;

Q_DECL_EXPORT bool fillinRunningKateAppInstances(KateRunningInstanceMap *map);
Q_DECL_EXPORT void cleanupRunningKateAppInstanceMap(KateRunningInstanceMap *map);

#endif
