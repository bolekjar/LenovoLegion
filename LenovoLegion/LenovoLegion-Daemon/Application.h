// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <Core/ApplicationInterface.h>
#include <Core/ApplicationModulesLoader.h>
#include <Core/ExceptionBuilder.h>


#include <QLocalServer>

#include <QCoreApplication>


namespace LenovoLegionDaemon {

class SysFsStructure;
class ProtocolProcessorBase;
class SysFsDriverManager;

class Application : public QCoreApplication,
                    public bj::framework::ApplicationInterface
{
    Q_OBJECT

public:

    DEFINE_EXCEPTION(Application);

    enum ERROR_CODES : int {
        NOT_ROOT = -1
    };

public:

    static constexpr const char* const  SOCKET_NAME                  = "LenovoLegionDaemonSocket";
    static constexpr const char* const  SOCKET_NAME_NOTIFICATION     = "LenovoLegionDaemonSocketNotifycation";

public:

    Application(int &argc, char *argv[]);

private:

    bool notify(QObject* receiver, QEvent* event) override;

    virtual void appRollBackImpl()                                          noexcept override;
    virtual void appStartImpl()                                                      override;
    virtual void appInitImpl(std::unique_ptr<ApplicationModulesHandler_T> modules)   override;
    virtual void appStopImpl()                                              noexcept override;
    virtual void appMainLoop()                                                       override;

    virtual void setLogingSeverityLevel(const bj::framework::Logger::SEVERITY_BITSET& severity) override;



    virtual const QDir  modulesPath() const                                          override;


private slots:

    /*
     * New connnectios handler
     */
    void newConnectionHandler();

    /*
     * New connnectios notification handler
     */
    void newConnectionNotificationHandler();;

private:

    void signalEventHandler(int signal) noexcept;

    void saveCurrentConfiguration();
    void saveCurrentPowerProfile();
    void saveCurrentCPUsControlProfile();


    void loadSavedConfiguration();
    void loadSavedPowerProfile();
    void loadSavedCPUsControlProfile();



private:

    /*
     * Connections
     */
    QLocalServer*                   m_serverSocket;
    QLocalServer*                   m_serverSocketNotification;


    /*
     * SysFs Driver Manager
     */
    SysFsDriverManager*            m_sysFsDriverManager;


    /*
     * Processing of the server protocol part
     */
    ProtocolProcessorBase*         m_protocolProcessor;

    /*
     * Processing of the notification protocol part
     */
    ProtocolProcessorBase*         m_protocolProcessorNotification;

};



}
