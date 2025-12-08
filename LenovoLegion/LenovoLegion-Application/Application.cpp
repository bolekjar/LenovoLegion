// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "Application.h"
#include "DataProviderManager.h"
#include "ProtocolProcessor.h"
#include "ProtocolProcessorNotifier.h"

#include <Core/LoggerHolder.h>
#include <Core/Application.h>


#include <QDir>
#include <QMessageBox>
#include <QMenu>


namespace LenovoLegionGui {

Application::Application(int &argc, char *argv[]) : QApplication(argc,argv),
    m_trayIcon(new QSystemTrayIcon(this))
{
    LoggerHolder::getInstance().init(QApplication::applicationDirPath().append(QDir::separator()).append(bj::framework::Application::log_dir).append(QDir::separator()).append(bj::framework::Application::apps_names[0]).append(".log").toStdString());
}

Application::~Application()
{}

void Application::appRollBackImpl() noexcept
{}

void Application::appStartImpl()
{}

void Application::appInitImpl(std::unique_ptr<ApplicationModulesHandler_T>)
{
    /*
     * Init Gui
     */

    m_mainWindow  = std::make_unique<MainWindow>();
    m_aboutWindow = std::make_unique<AboutWindow>();;


    /*
     * Init tray icon
     */
    m_trayIcon->setIcon(QIcon(":/images/icons/LenovoLegionIco.png"));
    m_trayIcon->setToolTip("Lenovo Legion control application");
    m_trayIcon->setContextMenu(new QMenu(m_mainWindow.get()));
    m_trayIcon->contextMenu()->addAction("Lenovo Legion", m_mainWindow.get(), &MainWindow::show);
    m_trayIcon->contextMenu()->addSeparator();
    m_trayIcon->contextMenu()->addAction("About", m_aboutWindow.get(), &AboutWindow::show);
    m_trayIcon->contextMenu()->addAction("Quit", this, &Application::onExitActionTriggered);


    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &Application::trayIconActivated);
    
    /*
     * Connect to settings changes
     */
    connect(ApplicationSettings::instance(), &ApplicationSettings::settingChanged,
            this, &Application::onSettingChanged);
    
    /*
     * Load and apply settings
     */
    loadSettings();
    applyStartupSettings();
}

void Application::appStopImpl() noexcept
{
    /*
     * Disconnect from settings signals
     */
    disconnect(ApplicationSettings::instance(), &ApplicationSettings::settingChanged,
               this, &Application::onSettingChanged);
    
    m_aboutWindow->close();
    m_mainWindow->close();

    m_trayIcon->setVisible(false);
    m_trayIcon->contextMenu()->clear();
}

void Application::appMainLoop()
{
    LOG_I(QString("Application name=").append(bj::framework::Application::apps_names[0]).append(" version=").append(bj::framework::Application::app_version).append(" started!"));
    m_trayIcon->setVisible(true);
    exec();
    LOG_I("Application stopped!");
}

void Application::setLogingSeverityLevel(const bj::framework::Logger::SEVERITY_BITSET &severity)
{
    LoggerHolder::getInstance().setSeverity(severity);
}

const QDir Application::modulesPath() const
{
    return QDir(QApplication::applicationDirPath().append(QDir::separator()).append(bj::framework::Application::modules_dir).append(QDir::separator()));
}

void Application::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {

        if(m_mainWindow->isVisible())
        {
            m_mainWindow->hide();
        }
        else
        {
            m_mainWindow->show();
        }
    }
}

void Application::onExitActionTriggered()
{
    m_aboutWindow->close();
    m_mainWindow->close();
    m_trayIcon->setVisible(false);
    QApplication::exit(0);
}

void Application::loadSettings()
{
    // Settings are loaded automatically via ApplicationSettings singleton
    LOG_D("Application settings loaded");
}

void Application::applyStartupSettings()
{
    // Apply debug logging setting
    bool appDebugLogging = false;
    ApplicationSettings::instance()->loadAppDebugLogging(appDebugLogging);
    applyDebugLogging(appDebugLogging);
    
    // Apply start minimized setting
    bool startMinimized = false;
    ApplicationSettings::instance()->loadStartMinimized(startMinimized);
    
    if (startMinimized) {
        LOG_D("Starting minimized to system tray");
        // Don't show main window on startup
    } else {
        m_mainWindow->show();
    }
}

void Application::applyDebugLogging(bool enable)
{
    if (enable) {
        // Enable all log levels including DEBUG
        setLogingSeverityLevel(bj::framework::Logger::SEVERITY_BITSET(
            (1 << bj::framework::Logger::SEVERITY::DEBUG)    |
            (1 << bj::framework::Logger::SEVERITY::INFO)     |
            (1 << bj::framework::Logger::SEVERITY::WARNING)  |
            (1 << bj::framework::Logger::SEVERITY::ERROR)
        ));
        LOG_D("Debug logging enabled");
    } else {
        // Disable DEBUG level, keep INFO, WARNING, ERROR
        setLogingSeverityLevel(bj::framework::Logger::SEVERITY_BITSET(
            (1 << bj::framework::Logger::SEVERITY::INFO)     |
            (1 << bj::framework::Logger::SEVERITY::WARNING)  |
            (1 << bj::framework::Logger::SEVERITY::ERROR)
        ));
        LOG_D("Debug logging disabled");
    }
}

void Application::onSettingChanged(ApplicationSettings::SettingType setting, bool value)
{
    using SettingType = ApplicationSettings::SettingType;
    
    switch (setting) {
        case SettingType::StartMinimized:
            LOG_D(QString("Setting changed: StartMinimized = ").append(value ? "true" : "false"));
            // This affects next startup, no action needed now
            break;
            
        case SettingType::MinimizeToTray:
            LOG_D(QString("Setting changed: MinimizeToTray = ").append(value ? "true" : "false"));
            // This is handled in MainWindow::closeEvent
            break;
            
        case SettingType::AppDebugLogging:
            LOG_D(QString("Setting changed: AppDebugLogging = ").append(value ? "true" : "false"));
            applyDebugLogging(value);
            break;
            
        default:
            break;
    }
}

bool Application::notify(QObject* receiver, QEvent* event) noexcept {
  try {
      return QApplication::notify(receiver, event);
  }
  catch (const ProtocolProcessor::exception_T& ex)
  {
      LOG_E(bj::framework::exception::ExceptionBuilder::print(ex).c_str());
      if(m_mainWindow) {
          QMessageBox::critical(m_mainWindow.get(),"LenovoLegion Protocol error",ex.descriptionInfo().value().c_str());
      }
  }
  catch (const ProtocolProcessorNotifier::exception_T& ex)
  {
      LOG_E(bj::framework::exception::ExceptionBuilder::print(ex).c_str());
      if(m_mainWindow) {
          QMessageBox::critical(m_mainWindow.get(),"LenovoLegion Protocol notifier error",ex.descriptionInfo().value().c_str());
      }
  }
  catch(const bj::framework::exception::Exception& ex)
  {    
      LOG_E(bj::framework::exception::ExceptionBuilder::print(ex).c_str());
      if(m_mainWindow) {
          QMessageBox::critical(m_mainWindow.get(),"LenovoLegion Error",QString("Unknown error occurred, error description = \" ").append(ex.descriptionInfo().value().c_str()).append(" \""));
      }
  } catch (...) {
      LOG_E(bj::framework::exception::ExceptionBuilder::print(__FILE__,__FUNCTION__,__LINE__,1,"Unknown error !").c_str());
      if(m_mainWindow) {
          QMessageBox::critical(m_mainWindow.get(),"LenovoLegion Error","Unknown error occurred!");
      }
  }

  return true;
}

}
