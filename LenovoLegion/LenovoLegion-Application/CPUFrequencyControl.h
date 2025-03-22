// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include "WidgetMessage.h"

#include <DataProvider.h>

#include <QWidget>
#include <QMap>

namespace Ui {
class CPUFrequencyControl;
}

namespace LenovoLegionGui {


class CPUFrequencyControlDataProvider;

class CPUFrequencyControl : public QWidget
{
    Q_OBJECT

public:
    static const QString NAME;
    static const QMap<QString,LenovoLegionDaemon::CPUXFreqControl::DataControl::CPUX> CPUFREQ_PRESETS;

    static const QString APPLY_TO_ALL;
    static const QString APPLY_TO_ALL_EFFICIENT;
    static const QString APPLY_TO_ALL_PERFORMANCE;
public:
    explicit CPUFrequencyControl(CPUFrequencyControlDataProvider *dataProvider,QWidget *parent = nullptr);
    ~CPUFrequencyControl();

    void refresh();

signals:

    void widgetEvent(const LenovoLegionGui::WidgetMessage& event);

private slots:

    /*
     * CPU Freq Control, Topology dependencies
     */
    void on_horizontalSlider_CPUFreqControlMin_valueChanged(int value);

    void on_horizontalSlider_CPUFreqControlMax_valueChanged(int value);

    void on_pushButton_CPUFreqControlCancel_clicked();

    void on_pushButton_CPUFreqControlApply_clicked();

    void on_pushButton_CPUFreqControlMin2500_clicked();

    void on_pushButton_CPUFreqControl25002500_clicked();

    void on_pushButton_CPUFreqControlMin3000_clicked();

    void on_pushButton_CPUFreqControl30003000_clicked();

    void on_pushButton_CPUFreqControlMin3500_clicked();

    void on_pushButton_CPUFreqControl35003500_clicked();

    void on_pushButton_CPUFreqControlMin4000_clicked();

    void on_pushButton_CPUFreqControl40004000_clicked();

    void on_pushButton_CPUFreqControlDefault_clicked();

    void on_pushButton_CPUFreqControlMin4500_clicked();

    void on_pushButton_CPUFreqControl45004500_clicked();

    void on_pushButton_CPUFreqControlMin5000_clicked();

    void on_pushButton_CPUFreqControl50005000_clicked();

    void on_comboBoxApplyTo_currentTextChanged(const QString &arg1);

private:

    void forAllCpuPerformanceCores(const std::function<bool(const int index)> &func);
    void forAllCpuEfficientCores(const std::function<bool(const int index)> &func);
    void forAllCPUThreads(const std::function<bool (const LenovoLegionDaemon::CPUXFreqControl::DataInfo::CPUX&,const size_t)> &func);

    void renderData();

private:
    Ui::CPUFrequencyControl *ui;
    CPUFrequencyControlDataProvider *m_dataProvider;

    LenovoLegionDaemon::CPUXFreqControl::DataInfo            m_cpuFreqData;
    LenovoLegionDaemon::CPUTopology::Heterogeneous::DataInfo m_cpuHetTopologyData;
    LenovoLegionDaemon::CPUTopology::Homogeneous::DataInfo   m_cpuHomTopologyData;
};

}
