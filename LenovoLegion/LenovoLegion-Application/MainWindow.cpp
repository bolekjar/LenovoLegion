// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include <MainWindow.h>
#include <ThreadFrequency.h>
#include <ThreadFrequencyControl.h>
#include <ThreadControl.h>
#include <CPUFrequency.h>
#include <PowerControl.h>
#include <FanControl.h>
#include <HWMonitoring.h>
#include <CPUControl.h>
#include <CPUFrequencyControl.h>
#include <PowerProfileControl.h>
#include <BateryStatus.h>

#include <CPUControlDataProvider.h>
#include <PowerProfileControlDataProvider.h>
#include <PowerControlDataProvider.h>
#include <CPUFrequencyControlDataProvider.h>

#include <Core/LoggerHolder.h>

#include <QMessageBox>
#include <QCloseEvent>

#include <thread>

namespace  LenovoLegionGui {

MainWindow::MainWindow(DataProvider *dataProvider, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_dataProvider(dataProvider),
    m_timerId(-1),
    m_timerEventCounter(0),
    m_messageBox(new QMessageBox(QMessageBox::Icon::NoIcon,
                                 "Lenovo Legion",
                                 "Lenovo Legion configuration is refresing, please wait...",
                                 QMessageBox::NoButton,
                                 this))
{
    ui->setupUi(this);

    m_messageBox->setStandardButtons(QMessageBox::NoButton);
    m_messageBox->setWindowModality(Qt::WindowModality::ApplicationModal);


    /*
     * Set default values
     */
    ui->label_DaemonStatus->setText(DAEMON_CONNECTION_STATUS_DISCONNECTED.data());

    connect(m_dataProvider,&DataProvider::dataProviderEvent,this,&MainWindow::on_dataProviderEvent);

    ui->actionMaximum_Game->connect(ui->actionMaximum_Game,&QAction::triggered,this,&MainWindow::on_actionMaximum_Game_triggered);
    ui->actionOptimum_Game->connect(ui->actionOptimum_Game,&QAction::triggered,this,&MainWindow::on_actionOptimum_Game_triggered);
    ui->actionSilent_Game->connect(ui->actionSilent_Game,&QAction::triggered,this,&MainWindow::on_actionSilent_Game_triggered);

    ui->actionMaximum_Multicore->connect(ui->actionMaximum_Multicore,&QAction::triggered,this,&MainWindow::on_actionMaximum_Multicore_triggered);
    ui->actionOptimum_Multicore->connect(ui->actionOptimum_Multicore,&QAction::triggered,this,&MainWindow::on_actionOptimum_Multicore_triggered);
    ui->actionSilent_Multicore->connect(ui->actionSilent_Multicore,&QAction::triggered,this,&MainWindow::on_actionSilent_Multicore_triggered);




    m_removeGuiEleamentsTasks.push_back([this] {
        removeAllControlTabs();

        removeLayoutItem(ui->horizontalLayout_HWMonitoring->takeAt(0));
        removeLayoutItem(ui->verticalLayout_PowerProfiles->takeAt(0));
        removeLayoutItem(ui->horizontalLayout_BateryStatus->takeAt(0));

        ui->menubar->blockSignals(true);
        ui->menubar->setEnabled(false);
    });

    m_removeGuiPPTasks.push_back([this] {
        removeControlTab(PowerControl::NAME);
        removeControlTab(FanControl::NAME);

        ui->menubar->blockSignals(true);
        ui->menubar->setEnabled(false);
    });


    m_addGuiEleamentsTasks.push_back([this] {
        addTabWidgetControls(new CPUControl(m_dataProvider->getCPUControlDataProvider(),ui->tabWidget_Controls),CPUControl::NAME);
        addTabWidgetControls(new CPUFrequencyControl(m_dataProvider->getCPUFrequencyControlDataProvider(),ui->tabWidget_Controls),CPUFrequencyControl::NAME);

        addHBoxLayoutWidget(new HWMonitoring(m_dataProvider->getHWMonitoringDataProvider(),this),*ui->horizontalLayout_HWMonitoring);
        addHBoxLayoutWidget(new BateryStatus(m_dataProvider->getBateryStatusDataProvider(),this),*ui->horizontalLayout_BateryStatus);
        addVBoxLayoutWidget(new PowerProfileControl(m_dataProvider->getPowerProfileControlDataProvider(),this),*ui->verticalLayout_PowerProfiles);
    });

    m_addGuiPPCustomTasks.push_back([this] {
        addTabWidgetControls(new PowerControl(m_dataProvider->getPowerControlDataProvider(),ui->tabWidget_Controls),PowerControl::NAME);
        addTabWidgetControls(new FanControl(m_dataProvider->getFanControlDataProvider(),ui->tabWidget_Controls),FanControl::NAME);

        ui->menubar->blockSignals(false);
        ui->menubar->setEnabled(true);
    });



    insertTasksBack(m_refreshGuiEleamentsTasks,m_removeGuiEleamentsTasks);
    insertTasksBack(m_refreshGuiEleamentsTasks,m_addGuiEleamentsTasks);


    insertTasksBack(m_refreshGuiPPCustomTasks,m_removeGuiPPTasks);
    insertTasksBack(m_refreshGuiPPCustomTasks,m_addGuiPPCustomTasks);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addModules(std::unique_ptr<bj::framework::ApplicationModulesHandler>)
{}

void MainWindow::timerEvent(QTimerEvent *)
{
    if(!m_evaluateAsyncTasks.empty())
    {
        m_evaluateAsyncTasks.front()();
        m_evaluateAsyncTasks.pop_front();
        return;
    }

    if(m_timerEventCounter > (TIMER_EVENT_IN_MS * 5/ 1000))
    {
       m_messageBox->hide();
    }


    if(ui->horizontalLayout_HWMonitoring->itemAt(0) != nullptr)
    {
        dynamic_cast<HWMonitoring*>(ui->horizontalLayout_HWMonitoring->itemAt(0)->widget())->refresh();
    }

    m_timerEventCounter++;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!event->spontaneous() || !isVisible())
        return;

    hide();
    event->ignore();
}

void MainWindow::on_actionMaximum_Game_triggered(bool)
{
    optimize(
        CPUControl::SMT_OFF_DATA,
        CPUControl::CPU_CONTROL_PRESETS["PERFORMANCE"],
        CPUControl::CPU_CONTROL_PRESETS["OFF"],
        CPUFrequencyControl::CPUFREQ_PRESETS["MAX_MAX"],
        CPUFrequencyControl::CPUFREQ_PRESETS["MAX_MAX"],
        PowerControl::CPU_PRESETS["MAXIMUM_GAME"],
        PowerControl::GPU_PRESETS["MAXIMUM_GAME"]
        );
}

void MainWindow::on_actionOptimum_Game_triggered(bool)
{
    optimize(
        CPUControl::SMT_OFF_DATA,
        CPUControl::CPU_CONTROL_PRESETS["PERFORMANCE"],
        CPUControl::CPU_CONTROL_PRESETS["OFF"],
        CPUFrequencyControl::CPUFREQ_PRESETS["4000_4000"],
        CPUFrequencyControl::CPUFREQ_PRESETS["4000_400"],
        PowerControl::CPU_PRESETS["MAXIMUM_GAME"],
        PowerControl::GPU_PRESETS["MAXIMUM_GAME"]
        );
}

void MainWindow::on_actionSilent_Game_triggered(bool)
{
    optimize(
        CPUControl::SMT_OFF_DATA,
        CPUControl::CPU_CONTROL_PRESETS["PERFORMANCE"],
        CPUControl::CPU_CONTROL_PRESETS["OFF"],
        CPUFrequencyControl::CPUFREQ_PRESETS["3500_3500"],
        CPUFrequencyControl::CPUFREQ_PRESETS["3500_3500"],
        PowerControl::CPU_PRESETS["MAXIMUM_GAME"],
        PowerControl::GPU_PRESETS["MAXIMUM_GAME"]
    );
}

void MainWindow::on_actionMaximum_Multicore_triggered(bool)
{
    optimize(
        CPUControl::SMT_ON_DATA,
        CPUControl::CPU_CONTROL_PRESETS["ONDEMAND"],
        CPUControl::CPU_CONTROL_PRESETS["ONDEMAND"],
        CPUFrequencyControl::CPUFREQ_PRESETS["MIN_MAX"],
        CPUFrequencyControl::CPUFREQ_PRESETS["MIN_MAX"],
        PowerControl::CPU_PRESETS["MAXIMUM_MULTICORE"],
        PowerControl::GPU_PRESETS["MAXIMUM_MULTICORE"]
    );
}

void MainWindow::on_actionOptimum_Multicore_triggered(bool)
{
    optimize(
        CPUControl::SMT_ON_DATA,
        CPUControl::CPU_CONTROL_PRESETS["ONDEMAND"],
        CPUControl::CPU_CONTROL_PRESETS["ONDEMAND"],
        CPUFrequencyControl::CPUFREQ_PRESETS["MIN_MAX"],
        CPUFrequencyControl::CPUFREQ_PRESETS["MIN_MAX"],
        PowerControl::CPU_PRESETS["OPTIMUM_MULTICORE"],
        PowerControl::GPU_PRESETS["OPTIMUM_MULTICORE"]
        );
}

void MainWindow::on_actionSilent_Multicore_triggered(bool)
{
    optimize(
        CPUControl::SMT_ON_DATA,
        CPUControl::CPU_CONTROL_PRESETS["ONDEMAND"],
        CPUControl::CPU_CONTROL_PRESETS["ONDEMAND"],
        CPUFrequencyControl::CPUFREQ_PRESETS["MIN_MAX"],
        CPUFrequencyControl::CPUFREQ_PRESETS["MIN_MAX"],
        PowerControl::CPU_PRESETS["SILENT_MULTICORE"],
        PowerControl::GPU_PRESETS["SILENT_MULTICORE"]
        );
}
void MainWindow::on_dataProviderEvent(const LenovoLegionGui::DataProvider::Message &event)
{
    switch (event.m_type) {

    case DataProvider::Message::Type::NOTIFICATION_CLIENT_IS_CONNECTED:
    {
        evaluateTasksAsync({[this]{
            evaluateTasks(m_refreshGuiEleamentsTasks);
        }});

        /*
         * Set timer for monitoring data
         */
        m_timerId = startTimer(TIMER_EVENT_IN_MS);

        /*
         * Set status
         */
        ui->label_DaemonStatus->setText(DAEMON_CONNECTION_STATUS_CONNECTED.data());
    }
    break;
    case DataProvider::Message::Type::NOTIFICATION_CLIENT_IS_DISCONNECTED:
    {
        evaluateTasks(m_removeGuiEleamentsTasks);

        /*
         * Kill timer
         */
        killTimer(m_timerId);m_timerId = -1;m_timerEventCounter = 0;m_evaluateAsyncTasks.clear();


        /*
         * Set status
         */
        ui->label_DaemonStatus->setText(DAEMON_CONNECTION_STATUS_DISCONNECTED.data());
    }
    break;   
    case LenovoLegionGui::DataProvider::Message::Type::NOTIFICATION_POWER_PROFILE_CHANGED:
    {
        if(ui->verticalLayout_PowerProfiles->itemAt(0) != nullptr)
        {
            dynamic_cast<PowerProfileControl*>(ui->verticalLayout_PowerProfiles->itemAt(0)->widget())->refresh();
        }
    }
    break;
    case LenovoLegionGui::DataProvider::Message::Type::NOTIFICATION_BATTERY_STATUS_CHANGED:
    {
        if(ui->horizontalLayout_BateryStatus->itemAt(0) != nullptr)
        {
            dynamic_cast<BateryStatus*>(ui->horizontalLayout_BateryStatus->itemAt(0)->widget())->refresh();
        }
    }
    break;
    case LenovoLegionGui::DataProvider::Message::Type::NOTIFICATION_LENOVO_DRIVER_ADDED:
    {
        evaluateTasks({[this]{
            blockDataProviderSignals(true);
            evaluateTasks(m_removeGuiEleamentsTasks);
        }});
        evaluateTasksAsync({[this]{
            evaluateTasks(m_refreshGuiEleamentsTasks);
            blockDataProviderSignals(false);
        }});
    }
    break;
    case LenovoLegionGui::DataProvider::Message::Type::NOTIFICATION_LENOVO_DRIVER_REMOVED:
    {
        evaluateTasks({[this]{
            blockDataProviderSignals(true);
            evaluateTasks(m_removeGuiEleamentsTasks);
        }});
        evaluateTasksAsync({[this]{
            evaluateTasks(m_refreshGuiEleamentsTasks);
            blockDataProviderSignals(false);
        }});
    }
    break;
    case LenovoLegionGui::DataProvider::Message::Type::NOTIFICATION_CPU_X_LIST_RELOADED:
    {
        evaluateTasks({[this]{
            blockDataProviderSignals(true);
            evaluateTasks(m_removeGuiEleamentsTasks);
        }});

        evaluateTasksAsync({[this]{
            evaluateTasks(m_refreshGuiEleamentsTasks);
            blockDataProviderSignals(false);
        }});
    }
    break;
    }
}

void MainWindow::on_widgetEvent(const WidgetMessage &event)
{
    if(event.m_widget == WidgetMessage::Widget::POWER_PROFILE_CONTROL)
    {
        if(event.m_message == WidgetMessage::Message::POWER_PROFILE_CHANGED_CUSTOM)
        {
            evaluateTasks(m_removeGuiPPTasks);
            evaluateTasksAsync({[this]{
                evaluateTasks(m_refreshGuiPPCustomTasks);
            }});
        }

        if(event.m_message == WidgetMessage::Message::POWER_PROFILE_CHANGED)
        {
            evaluateTasks(m_removeGuiPPTasks);
        }

        if(event.m_message == WidgetMessage::Message::POWER_PROFILE_NOT_AVAILABLE)
        {
            evaluateTasks(m_removeGuiPPTasks);
            evaluateTasks({[this]{removeLayoutItem(ui->verticalLayout_PowerProfiles->takeAt(0));}});
        }
    }

    if(event.m_widget == WidgetMessage::Widget::CPU_CONTROL)
    {
        if(event.m_message == WidgetMessage::Message::CPU_CONTROL_CHANGED)
        {
            evaluateTasks({[this]{
                blockDataProviderSignals(true);
                evaluateTasks(m_removeGuiEleamentsTasks);
            }});
            evaluateTasksAsync({[this]{
                evaluateTasks(m_refreshGuiEleamentsTasks);
                blockDataProviderSignals(false);
            }});
        }
    }

    if(event.m_widget == WidgetMessage::Widget::CPU_FREQUENCY_CONTROL)
    {
        if(event.m_message == WidgetMessage::Message::CPU_FREQ_CONTROL_APPLY)
        {
            evaluateTasks({[this]{
                removeLayoutItem(ui->horizontalLayout_HWMonitoring->takeAt(0));
                addHBoxLayoutWidget(new HWMonitoring(m_dataProvider->getHWMonitoringDataProvider(),this),*ui->horizontalLayout_HWMonitoring);
            }});
        }
    }
}

void MainWindow::removeControlTab(const QString &name)
{
    /*
     * Remove tab
     */
    for(int i = 0; i < ui->tabWidget_Controls->count(); i++)
    {
        if(ui->tabWidget_Controls->tabText(i) == name)
        {
            ui->tabWidget_Controls->widget(i)->deleteLater();
            ui->tabWidget_Controls->removeTab(i);
            break;
        }
    }
}

void MainWindow::removeAllControlTabs()
{
    while (ui->tabWidget_Controls->count() > 0)
    {
        ui->tabWidget_Controls->widget(ui->tabWidget_Controls->count() - 1)->deleteLater();
        ui->tabWidget_Controls->removeTab(ui->tabWidget_Controls->count() - 1);
    }
}

void MainWindow::removeLayoutItem(QLayoutItem *layout)
{
    if(layout != nullptr)
    {
        if(layout->widget() != nullptr)
        {
            layout->widget()->deleteLater();
        }

        if(layout->layout() != nullptr)
        {
            layout->layout()->deleteLater();
        }
        delete layout;
    }
}

void MainWindow::evaluateTasks(const std::list<std::function<void ()> >& tasks)
{
    if(m_messageBox->isHidden())
    {
        m_messageBox->show();
    }

    for(auto & task: tasks)
    {
        task();
    }

    m_timerEventCounter = 0;
}

void MainWindow::evaluateTasksAsync(const std::list<std::function<void ()> >& tasks)
{
    if(m_messageBox->isHidden())
    {
        m_messageBox->show();
    }

    insertTasksBack(m_evaluateAsyncTasks,tasks);

    m_timerEventCounter = 0;
}

void MainWindow::insertTasksBack(std::list<std::function<void ()> > &tasks, const std::list<std::function<void ()> > &tasksToInsert)
{
    tasks.insert(tasks.end(),tasksToInsert.begin(),tasksToInsert.end());
}

void MainWindow::insertTasksFront(std::list<std::function<void ()> > &tasks, const std::list<std::function<void ()> > &tasksToInsert)
{
    tasks.insert(tasks.begin(),tasksToInsert.begin(),tasksToInsert.end());
}

void MainWindow::blockDataProviderSignals(bool block) {
    m_dataProvider->blockSignals(block);
}

void MainWindow::optimize(const LenovoLegionDaemon::CPUSMTControl::DataControl          &smt,
                          const LenovoLegionDaemon::CPUXControl::DataControl::CPUX      &performance,
                          const LenovoLegionDaemon::CPUXControl::DataControl::CPUX      &efficient,
                          const LenovoLegionDaemon::CPUXFreqControl::DataControl::CPUX  &freqPerformance,
                          const LenovoLegionDaemon::CPUXFreqControl::DataControl::CPUX  &freqEfficient,
                          const LenovoLegionDaemon::PowerControl::CPU::DataControl      &powerCPU,
                          const LenovoLegionDaemon::PowerControl::GPU::DataControl      &powerGPU
                          )
{
    evaluateTasks({[this]{
        blockDataProviderSignals(true);
        evaluateTasks(m_removeGuiEleamentsTasks);
    }});


    evaluateTasksAsync({[this,smt,performance,efficient,freqPerformance,freqEfficient,powerCPU,powerGPU]{
        setTheValueWithTimeout([this] {
            m_dataProvider->getPowerProfileControlDataProvider()->setPowerProfileData(LenovoLegionDaemon::PowerProfile::Control::getDataControl(LenovoLegionDaemon::PowerProfile::Control::PowerProfiles::POWER_PROFILE_CUSTOM));
        }, [this] {
                                   return m_dataProvider->getPowerProfileControlDataProvider()->getPowerProfileData().m_data.m_profile == LenovoLegionDaemon::PowerProfile::Control::PowerProfiles::POWER_PROFILE_CUSTOM;
                               });


        setTheValueWithTimeout([this] {
            m_dataProvider->getCPUControlDataProvider()->setCpuSControlData(LenovoLegionDaemon::CPUXControl::getDataControl(LenovoLegionDaemon::CPUXControl::DataControl::CPUX{.m_Governor = "ondeamand",.m_cpuOnline = true}));
        }, [] {
                                   return true;
                               });



        setTheValueWithTimeout([this,smt] {
            m_dataProvider->getCPUControlDataProvider()->setCpuSMTControlData(smt);
        }, [] {
                                   return true;
                               });

        setTheValueWithTimeout([this,performance] {
            m_dataProvider->getCPUControlDataProvider()->setCpuSControlData(LenovoLegionDaemon::CPUXControl::getDataControl(
                m_dataProvider->getCPUControlDataProvider()->getCPUXInfoControlData(),
                m_dataProvider->getCPUControlDataProvider()->getCPUHeterogenousTopologyData().m_data.m_ActiveCpusCore,
                performance));
        }, [] {
                                   return true;
                               });

        setTheValueWithTimeout([this,efficient] {
            m_dataProvider->getCPUControlDataProvider()->setCpuSControlData(LenovoLegionDaemon::CPUXControl::getDataControl(
                m_dataProvider->getCPUControlDataProvider()->getCPUXInfoControlData(),
                m_dataProvider->getCPUControlDataProvider()->getCPUHeterogenousTopologyData().m_data.m_ActiveCpusAtom,
                efficient));
        }, [] {
                                   return true;
                               });

        setTheValueWithTimeout([this,freqPerformance] {
            m_dataProvider->getCPUFrequencyControlDataProvider()->setCpuFreqControlData(LenovoLegionDaemon::CPUXFreqControl::getDataControl(
                m_dataProvider->getCPUFrequencyControlDataProvider()->getCPUsInfoData(),
                m_dataProvider->getCPUFrequencyControlDataProvider()->getCPUHeterogenousTopologyData().m_data.m_ActiveCpusCore,
                freqPerformance));
        }, [] {
                                   return true;
                               });

        setTheValueWithTimeout([this,freqEfficient] {
            m_dataProvider->getCPUFrequencyControlDataProvider()->setCpuFreqControlData(LenovoLegionDaemon::CPUXFreqControl::getDataControl(
                m_dataProvider->getCPUFrequencyControlDataProvider()->getCPUsInfoData(),
                m_dataProvider->getCPUFrequencyControlDataProvider()->getCPUHeterogenousTopologyData().m_data.m_ActiveCpusAtom,
                freqEfficient));
        }, [] {
                                   return true;
                               });

        setTheValueWithTimeout([this,powerCPU,powerGPU] {
            m_dataProvider->getPowerControlDataProvider()->setCpuControlData(powerCPU);
            m_dataProvider->getPowerControlDataProvider()->setGpuControlData(powerGPU);
        }, [] {
                                   return true;
                               });
    }});

    evaluateTasksAsync({[this]{
        evaluateTasks(m_refreshGuiEleamentsTasks);
        blockDataProviderSignals(false);
    }});
}


void MainWindow::setTheValueWithTimeout(std::function<void ()> setter, std::function<bool ()> getter, int timeoutInMilSec)
{
    setter();

    while (!getter()) {
        if (timeoutInMilSec <= 0) {
            THROW_EXCEPTION(exception_T,REQUEST_TIMEOUTED,"Request timeouted !");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        timeoutInMilSec--;
    }
};


}
