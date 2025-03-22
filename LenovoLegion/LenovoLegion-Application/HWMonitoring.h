// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include "WidgetMessage.h"
#include <QWidget>

#include <DataProvider.h>

#include <chrono>

namespace Ui {
class HWMonitoring;
}

namespace LenovoLegionGui {

class HWMonitoringDataProvider;
class CPUFrequency;

class HWMonitoring : public QWidget
{
    Q_OBJECT

public:
    explicit HWMonitoring(HWMonitoringDataProvider *dataProvider,QWidget *parent = nullptr);

    void refresh();

    virtual ~HWMonitoring();

private slots:
    void on_groupBox_17_clicked(bool checked);
    void on_freqInfoByCoreClosed();
private:

    void forAllCpuPerformanceCores(const std::function<bool(const int index)> &func);
    void forAllCpuEfficientCores(const std::function<bool(const int index)> &func);

signals:

    void widgetEvent(const LenovoLegionGui::WidgetMessage& event);
private:

    Ui::HWMonitoring *ui;

    LenovoLegionDaemon::HWMonitoring::DataInfo                m_hwMonitoringData;
    LenovoLegionDaemon::CPUTopology::Heterogeneous::DataInfo  m_cpuHetTopology;
    LenovoLegionDaemon::CPUTopology::Homogeneous::DataInfo    m_cpuHomTopology;

    HWMonitoringDataProvider    *m_dataProvider;
    CPUFrequency                *m_windowFreqInfoByCore;

    std::chrono::steady_clock::time_point    m_lastRefreshTime;
};

}
