// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include <Application.h>

#include "ProtocolProcessor.h"
#include "ProtocolProcessorNotifier.h"
#include "Settings.h"

#include "SysFsDataProvider.h"
#include "SysFsDriverManager.h"


#include "SysFsDriverLenovo.h"
#include "SysFsDriverCPU.h"
#include "SysFsDriverCPUXList.h"
#include "SysFsDriverCPUCore.h"
#include "SysFsDriverCPUAtom.h"
#include "SysFsDriverACPIPlatformProfile.h"
#include "SysFsDriverIntelPowercapRapl.h"
#include "SysFsDriverPowerSuplyBattery0.h"





#include <Core/LoggerHolder.h>
#include <Core/Application.h>

#include <OS/Linux/Signal.h>

#include <QDir>

#include <signal.h>
#include <unistd.h>

namespace LenovoLegionDaemon {

Application::Application(int &argc, char *argv[]) :
    QCoreApplication(argc,argv),
    m_serverSocket(new QLocalServer(this)),
    m_serverSocketNotification(new QLocalServer(this)),
    m_sysFsDriverManager(new SysFsDriverManager(this)),
    m_protocolProcessor(nullptr),
    m_protocolProcessorNotification(nullptr)
{
    LoggerHolder::getInstance().init(QCoreApplication::applicationDirPath().append(QDir::separator()).append(bj::framework::Application::log_dir).append(QDir::separator()).append(bj::framework::Application::apps_names[1]).append(".log").toStdString());

    /*
     * Load SysFS Drivers
     */
    m_sysFsDriverManager->addDriver(new SysFsDriverLenovo(m_sysFsDriverManager));
    m_sysFsDriverManager->addDriver(new SysFsDriverCPU(m_sysFsDriverManager));
    m_sysFsDriverManager->addDriver(new SysFsDriverCPUXList(m_sysFsDriverManager));
    m_sysFsDriverManager->addDriver(new SysFsDriverCPUCore(m_sysFsDriverManager));
    m_sysFsDriverManager->addDriver(new SysFsDriverCPUAtom(m_sysFsDriverManager));
    m_sysFsDriverManager->addDriver(new SysFsDriverACPIPlatformProfile(m_sysFsDriverManager));
    m_sysFsDriverManager->addDriver(new SysFsDriverIntelPowercapRapl(m_sysFsDriverManager));
    m_sysFsDriverManager->addDriver(new SysFsDriverPowerSuplyBattery0(m_sysFsDriverManager));
}

void Application::appRollBackImpl() noexcept
{}

void Application::appStartImpl()
{}

void Application::appInitImpl(std::unique_ptr<ApplicationModulesHandler_T>)
{
     LOG_I(QString("Application ").append(bj::framework::Application::apps_names[1]).append(" version=").append(bj::framework::Application::app_version).append(" is starting ..."));

    /*
     * Check if you are root
     */
    if(geteuid() != 0)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::NOT_ROOT,"You must be root to run this application!");
    }

    /*
     * Set signal handler
     */
    bj::framework::Signal::SetSignalHandler(SIGINT,std::bind(&Application::signalEventHandler,this,std::placeholders::_1));
    bj::framework::Signal::SetSignalHandler(SIGTERM,std::bind(&Application::signalEventHandler,this,std::placeholders::_1));


    /*
     * Init SysFs drivers
     */
    m_sysFsDriverManager->initDrivers();



    /*
     * Load configuration
     */
    loadSavedConfiguration();


    /*
     * Start Server
     */   
    connect(m_serverSocket,&QLocalServer::newConnection,this,&Application::newConnectionHandler);

    m_serverSocket->setSocketOptions(QLocalServer::WorldAccessOption);
    m_serverSocket->setMaxPendingConnections(1);

    m_serverSocket->listen(SOCKET_NAME);


    /*
     * Start notification server
     */
    connect(m_serverSocketNotification,&QLocalServer::newConnection,this,&Application::newConnectionNotificationHandler);

    m_serverSocketNotification->setSocketOptions(QLocalServer::WorldAccessOption);
    m_serverSocketNotification->setMaxPendingConnections(1);

    m_serverSocketNotification->listen(SOCKET_NAME_NOTIFICATION);


}

void Application::appStopImpl() noexcept
{
    LOG_I("Stopping application ... ");

    /*
     * Stop Server
     */
    disconnect(m_serverSocket,&QLocalServer::newConnection,this,&Application::newConnectionHandler);
    m_serverSocket->close();
    m_protocolProcessor != nullptr ? m_protocolProcessor->stop() : void();
    delete m_protocolProcessor;

    /*
     * Stop notification server
     */
    disconnect(m_serverSocketNotification,&QLocalServer::newConnection,this,&Application::newConnectionNotificationHandler);
    m_serverSocketNotification->close();
    m_protocolProcessorNotification != nullptr ? m_protocolProcessorNotification->stop() : void();
    delete m_protocolProcessorNotification;

    /*
     * Save configuration
     */
    saveCurrentConfiguration();

    /*
     * Remove SysFs drivers
     */
    m_sysFsDriverManager->cleanDrivers();


    LOG_I(QString("Application ").append(bj::framework::Application::apps_names[1]).append(" version=").append(bj::framework::Application::app_version).append(" stopped !"));
}

void Application::appMainLoop()
{
    LOG_I("Application appMainLoop started!");
    exec();
    LOG_I("Application appMainLoop ended!");
}

void Application::setLogingSeverityLevel(const bj::framework::Logger::SEVERITY_BITSET &severity)
{
    LoggerHolder::getInstance().setSeverity(severity);
}

const QDir Application::modulesPath() const
{
    return QDir(QCoreApplication::applicationDirPath().append(QDir::separator()).append(bj::framework::Application::modules_dir).append(QDir::separator()));
}

void Application::newConnectionHandler()
{
    LOG_D("New connection !");

    try {

        if(m_protocolProcessor != nullptr && m_protocolProcessor->isRunning())
        {
            LOG_W("New connection refused !");
            ProtocolProcessorBase::refuseConnection(m_serverSocket->nextPendingConnection());
            return;
        }

        m_protocolProcessor != nullptr ? m_protocolProcessor->deleteLater() : void();
        m_protocolProcessor = new ProtocolProcessor(new SysFsDataProvider(m_sysFsDriverManager,this),m_serverSocket->nextPendingConnection(),this);
        m_protocolProcessor->start();
    }
    catch (ProtocolProcessor::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == ProtocolProcessorBase::CLIENT_POINTER_ERROR)
        {
            bj::framework::exception::ExceptionBuilder::print(ex);
        }
        else
        {
            LOG_E(bj::framework::exception::ExceptionBuilder::print(ex).c_str());
            QCoreApplication::exit(0);
        }

    } catch (...)
    {
        LOG_E("New connection unkownm error !");
    }

}

void Application::newConnectionNotificationHandler()
{
    LOG_D("New notification connection !");

    try {

        if(m_protocolProcessorNotification != nullptr && m_protocolProcessorNotification->isRunning())
        {
            LOG_W("Other client is connected, refuse connection !");
            ProtocolProcessorBase::refuseConnection(m_serverSocketNotification->nextPendingConnection());
            return;
        }

        m_protocolProcessorNotification != nullptr ? m_protocolProcessorNotification->deleteLater() : void();
        m_protocolProcessorNotification = new ProtocolProcessorNotifier(new SysFsDataProvider(m_sysFsDriverManager,this),m_serverSocketNotification->nextPendingConnection(),this);
        connect(m_sysFsDriverManager,&SysFsDriverManager::kernelEvent,dynamic_cast<ProtocolProcessorNotifier*>(m_protocolProcessorNotification),&ProtocolProcessorNotifier::kernelEventHandler);
        connect(m_sysFsDriverManager,&SysFsDriverManager::moduleSubsystem,dynamic_cast<ProtocolProcessorNotifier*>(m_protocolProcessorNotification),&ProtocolProcessorNotifier::moduleSubsystemHandler);

        m_protocolProcessorNotification->start();
    }
    catch (ProtocolProcessor::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == ProtocolProcessorBase::CLIENT_POINTER_ERROR)
        {
            bj::framework::exception::ExceptionBuilder::print(ex);
        }
        else
        {
            LOG_E(bj::framework::exception::ExceptionBuilder::print(ex).c_str());
            QCoreApplication::exit(0);
        }

    } catch (...)
    {
        LOG_E("New notification connection unkownm error !");
    }
}


void Application::signalEventHandler(int signal) noexcept
{
    LOG_D(QString("Signal ").append(QString::number(signal)).append(" received!"))

    if(signal == SIGINT || signal == SIGTERM)
    {
        QCoreApplication::exit(0);
    }
}

void Application::saveCurrentConfiguration()
{
    try {
        saveCurrentPowerProfile();
    } catch(const SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::DRIVER_NOT_AVAILABLE)
        {
            LOG_W("SysFs driver not available, power profile not saved!");
        }
        else
        {
            throw;
        }
    }
    try {
        saveCurrentCPUsControlProfile();
    } catch(const SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::DRIVER_NOT_AVAILABLE)
        {
            LOG_W("SysFs driver not available, CPU control profile not saved!");
        }
        else
        {
            throw;
        }
    }

}

void Application::saveCurrentPowerProfile()
{
    SettingsSaverPowerProfiles().saverPowerProfile({.m_data = SysFsDataProvider(m_sysFsDriverManager).getPowerProfileData().m_data});
}

void Application::saveCurrentCPUsControlProfile()
{
    /*
     * Save CPU control profile
     */
    SettingsSaverCPUControlData().saverPowerProfile(CPUXControl::getDataControl(SysFsDataProvider(m_sysFsDriverManager).getCPUsInfoControlData()));
}

void Application::loadSavedConfiguration()
{
    /*
     * Load saved power profile
     */
    try {
        loadSavedPowerProfile();
    } catch(const SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::DRIVER_NOT_AVAILABLE)
        {
            LOG_W("SysFs driver not available, power profile not loaded!");
        }
        else
        {
            throw;
        }
    }

    /*
     * Load saved CPU control profile
     */
    try {
        loadSavedCPUsControlProfile();
    } catch(const SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::DRIVER_NOT_AVAILABLE)
        {
            LOG_W("SysFs driver not available, CPU control profile not loaded!");
        }
        else
        {
            throw;
        }
    }
}

void Application::loadSavedPowerProfile()
{
    if(SettingsLoaderPowerProfiles().isCorrectDefined())
    {
        LOG_D("Power profile saved, loading ...");

        SysFsDataProvider                   dataProvider(m_sysFsDriverManager);
        PowerProfile::Control::DataControl  savedPowerProfile;


        SettingsLoaderPowerProfiles().loadPowerProfile(savedPowerProfile);



        if(dataProvider.getBateryControlData().m_data.m_powerChargeMode == Batery::PowerChargeMode::POWER_CHARGE_MODE_AC)
        {
            dataProvider.setPowerProfileData(savedPowerProfile);
        }
        else
        {
            if(savedPowerProfile.m_data.m_profile == PowerProfile::Control::PowerProfiles::POWER_PROFILE_CUSTOM  ||
                savedPowerProfile.m_data.m_profile == PowerProfile::Control::PowerProfiles::POWER_PROFILE_PERFORMANCE)
            {
                dataProvider.setPowerProfileData(PowerProfile::Control::DataControl{.m_data {PowerProfile::Control::PowerProfiles::POWER_PROFILE_BALANCED}});
            }
            else
            {
                dataProvider.setPowerProfileData(savedPowerProfile);
            }
        }
    }
    else
    {
        LOG_D("Power profile not saved, using default!");
    }
}

void Application::loadSavedCPUsControlProfile()
{
    /*
     * Load saved CPU control profile
     */
    if(SettingsLoaderCPUControlData().isCorrectDefined())
    {
        LOG_D("CPU control profile defined, loading ...");

        SysFsDataProvider           dataProvider(m_sysFsDriverManager);
        CPUXControl::DataControl    savedCPUControlProfile;

        SettingsLoaderCPUControlData().loadPowerProfile(savedCPUControlProfile);

        dataProvider.setCpuControlData(savedCPUControlProfile);
    }
    else
    {
        LOG_D("CPU control profile not defined, using default!");
    }
}


bool Application::notify(QObject* receiver, QEvent* event) {
  bool done = true;

  try {
      done = QCoreApplication::notify(receiver, event);
  }
  catch(const bj::framework::exception::Exception& ex)
  {
      LOG_E(bj::framework::exception::ExceptionBuilder::print(ex).c_str());
  } catch (...) {
      LOG_E(bj::framework::exception::ExceptionBuilder::print(__FILE__,__FUNCTION__,__LINE__,1,"Unknown error !").c_str());
  }

  return done;
}

}
