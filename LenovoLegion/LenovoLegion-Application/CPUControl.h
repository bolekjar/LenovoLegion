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

namespace Ui {
class CPUControl;
}

namespace LenovoLegionGui {

class CPUControlDataProvider;


class CPUControl : public QWidget
{
    Q_OBJECT

private:

    static const QMap<QString,LenovoLegionDaemon::CPUXControl::DataControl::CPUX> CPU_CONTROL_PRESETS;

public:

    static const QString NAME;


    static const LenovoLegionDaemon::CPUSMTControl::DataControl                   SMT_ON_DATA;
    static const LenovoLegionDaemon::CPUSMTControl::DataControl                   SMT_OFF_DATA;

    static const QString APPLY_TO_ALL;
    static const QString APPLY_TO_ALL_ENABLED;
    static const QString APPLY_TO_ALL_EFFICIENT;
    static const QString APPLY_TO_ALL_PERFORMANCE;
    static const QString APPLY_TO_ALL_DISABLED;


    static constexpr std::string_view POWER_SAVE    = "POWER_SAVE";
    static constexpr std::string_view PERFORMANCE   = "PERFORMANCE";
    static constexpr std::string_view ONDEMAND      = "ONDEMAND";
    static constexpr std::string_view OFF           = "OFF";


public:
    explicit CPUControl(CPUControlDataProvider *dataProvider,QWidget *parent = nullptr);
    ~CPUControl();

    void refresh();


    static LenovoLegionDaemon::CPUXControl::DataControl::CPUX getCpuControlPreset(const QString &presetName,const LenovoLegionDaemon::CPUXControl::DataInfo::CPUX &dataInfo);


signals:

    void widgetEvent(const LenovoLegionGui::WidgetMessage& event);

private slots:
    void on_pushButton_CPUControlEnableAll_clicked();

    void on_pushButton_CPUControlDisableAll_clicked();

    void on_comboBox_CPUGovernor_currentTextChanged(const QString &arg1);

    void on_pushButton_CPUControlCancel_clicked();

    void on_pushButton_CPUControlApply_clicked();

    void on_checkBox_DisableSMT_checkStateChanged(const Qt::CheckState &arg1);

    void on_comboBoxApplyTo_currentTextChanged(const QString &arg1);

private:

    void forAllCpuPerformanceCores(const std::function<bool(const int index)> &func);
    void forAllCpuEfficientCores(const std::function<bool(const int index)> &func);
    void forAllCpuDisabledCores(const std::function<bool(const int index)> &func);

    void renderData();

private:
    Ui::CPUControl *ui;

    CPUControlDataProvider*  m_dataProvider;

    LenovoLegionDaemon::CPUXControl::DataInfo                           m_cpuInfoData;
    LenovoLegionDaemon::CPUTopology::Heterogeneous::DataInfo            m_cpuHetTopology;
    LenovoLegionDaemon::CPUTopology::Homogeneous::DataInfo              m_cpuHomTopology;
    LenovoLegionDaemon::CPUSMTControl::DataInfo                         m_cpuSMTControlData;

};

}
