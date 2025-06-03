// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "Application.h"
#include "DaemonDataProvider.h"
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

    m_mainWindow  = std::make_unique<MainWindow>(new DaemonDataProvider(this));
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
}

void Application::appStopImpl() noexcept
{
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

bool Application::notify(QObject* receiver, QEvent* event) {
  bool done = true;

  try {
      done = QApplication::notify(receiver, event);
  }
  catch (const ProtocolProcessor::exception_T& ex)
  {
      LOG_E(bj::framework::exception::ExceptionBuilder::print(ex).c_str());
      QMessageBox::critical(m_mainWindow.get(),"LenovoLegion Protocol error",ex.descriptionInfo().value().c_str());
  }
  catch (const ProtocolProcessorNotifier::exception_T& ex)
  {
      LOG_E(bj::framework::exception::ExceptionBuilder::print(ex).c_str());
      QMessageBox::critical(m_mainWindow.get(),"LenovoLegion Protocol notifieer error",ex.descriptionInfo().value().c_str());
  }
  catch(const bj::framework::exception::Exception& ex)
  {    
      LOG_E(bj::framework::exception::ExceptionBuilder::print(ex).c_str());
      QMessageBox::critical(m_mainWindow.get(),"LenovoLegion Error",QString("Unknow error ocurre, error description = \" ").append(ex.descriptionInfo().value().c_str()).append(" \""));
  } catch (...) {
      LOG_E(bj::framework::exception::ExceptionBuilder::print(__FILE__,__FUNCTION__,__LINE__,1,"Unknown error !").c_str());
      QMessageBox::critical(m_mainWindow.get(),"LenovoLegion Error","Unknow error ocurre !");
  }

  return done;
}

}
