// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <DataProvider.h>
#include <WidgetMessage.h>


#include <Core/ExceptionBuilder.h>
#include <Core/ApplicationModulesHandler.h>

#include <QMainWindow>
#include <QString>
#include <QLayoutItem>
#include <QVBoxLayout>

#include <list>

#include "ui_MainWindow.h"

namespace Ui {
class MainWindow;
}


namespace  LenovoLegionGui {

class CPUFrequency;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /*
     * Colors
     */
    static constexpr std::string_view VALUE_CHANGE_COLOR                       =    "#995e5e";
    static constexpr int TIMER_EVENT_IN_MS                                     =    500;


    DEFINE_EXCEPTION(MainWindow);

    enum ERROR_CODES : int {
        REQUEST_TIMEOUTED = 0
    };

private:

    /*
     * Daemon Connection Status
     */
    static constexpr std::string_view DAEMON_CONNECTION_STATUS_CONNECTED       =    "Daemon: Connected";
    static constexpr std::string_view DAEMON_CONNECTION_STATUS_DISCONNECTED    =    "Daemon: Disconnected";

public:

    explicit MainWindow(DataProvider* dataProvider,QWidget *parent = nullptr);

    virtual ~MainWindow();

    void addModules(std::unique_ptr<bj::framework::ApplicationModulesHandler>);


signals:
    
protected:

    void timerEvent(QTimerEvent *) override;
    void closeEvent(QCloseEvent *event) override;

private slots:

    void on_actionMaximum_Game_triggered(bool checked);
    void on_actionOptimum_Game_triggered(bool checked);
    void on_actionSilent_Game_triggered(bool checked);

    void on_actionMaximum_Multicore_triggered(bool checked);
    void on_actionOptimum_Multicore_triggered(bool checked);
    void on_actionSilent_Multicore_triggered(bool checked);



public slots:

    void on_dataProviderEvent(const LenovoLegionGui::DataProvider::Message& event);
    void on_widgetEvent(const LenovoLegionGui::WidgetMessage& event);

private:
    void removeControlTab(const QString& name);
    void removeAllControlTabs();
    void removeLayoutItem(QLayoutItem* layout);

    void evaluateAllTasks(std::list<std::function< void ()>> tasks);
    void insertTasksBack(std::list<std::function< void ()>>& tasks, const std::list<std::function< void ()>>& tasksToInsert);
    void insertTasksFront(std::list<std::function< void ()>>& tasks, const std::list<std::function< void ()>>& tasksToInsert);

    void blockDataProviderSignals(bool block);

    void optimize(const LenovoLegionDaemon::CPUSMTControl::DataControl&             smt,
                  const LenovoLegionDaemon::CPUXControl::DataControl::CPUX&         performance,
                  const LenovoLegionDaemon::CPUXControl::DataControl::CPUX&         efficient,
                  const LenovoLegionDaemon::CPUXFreqControl::DataControl::CPUX&     freqPerformance,
                  const LenovoLegionDaemon::CPUXFreqControl::DataControl::CPUX&     freqEfficient,
                  const LenovoLegionDaemon::PowerControl::CPU::DataControl&         powerCPU,
                  const LenovoLegionDaemon::PowerControl::GPU::DataControl&         powerGPU
                  );

    void setTheValueWithTimeout(std::function<void ()> setter, std::function<bool ()> getter, int timeoutInMilSec = 50);

    template<class T>
    void addTabWidgetControls(T* widget, const QString& name);

    template<class T>
    void addVBoxLayoutWidget(T* widget,QVBoxLayout& layout);

    template<class T>
    void addHBoxLayoutWidget(T* widget,QHBoxLayout& layout);

private:

    Ui::MainWindow *ui;

    /*
     * Data Provider
     */
    DataProvider*  m_dataProvider;

    /*
     * Id of timer
     */
    int m_timerId;

    /*
     * Tasks evaluated in timer event
     */
    std::list<std::function< void ()>>  m_tasksBasedOnTimer;

    /*
     * Tasks for GUI refresh
     */
    std::list<std::function< void ()>>  m_refreshGuiEleamentsTasks;
    std::list<std::function< void ()>>  m_refreshGuiPPCustomTasks;
    std::list<std::function< void ()>>  m_refreshGuiPPTasks;

    std::list<std::function< void ()>>  m_removeGuiEleamentsTasks;
    std::list<std::function< void ()>>  m_removeGuiPPTasks;

    std::list<std::function< void ()>>  m_addGuiEleamentsTasks;
    std::list<std::function< void ()>>  m_addGuiPPCustomTasks;
    std::list<std::function< void ()>>  m_addGuiPPTasks;
};

template<class T>
inline void MainWindow::addTabWidgetControls(T *widget, const QString &name)
{
    ui->tabWidget_Controls->addTab(widget,name);
    connect(widget,&T::widgetEvent,this,&MainWindow::on_widgetEvent);
    widget->refresh();
}

template<class T>
void MainWindow::addVBoxLayoutWidget(T *widget, QVBoxLayout &layout)
{

    layout.insertWidget(0,widget);
    connect(widget,&T::widgetEvent,this,&MainWindow::on_widgetEvent);
    widget->refresh();
}

template<class T>
void MainWindow::addHBoxLayoutWidget(T *widget, QHBoxLayout &layout)
{
    layout.insertWidget(0,widget);
    connect(widget,&T::widgetEvent,this,&MainWindow::on_widgetEvent);
    widget->refresh();

}

}
