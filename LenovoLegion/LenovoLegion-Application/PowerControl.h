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
#include <QString>
#include <QMap>

namespace Ui {
class PowerControl;
}

namespace LenovoLegionGui {

class PowerControlDataProvider;

class PowerControl : public QWidget
{
    Q_OBJECT


public:

    static const QString NAME;

    static const QMap<QString,LenovoLegionDaemon::PowerControl::CPU::DataControl> CPU_PRESETS;
    static const QMap<QString,LenovoLegionDaemon::PowerControl::GPU::DataControl> GPU_PRESETS;

public:

    explicit PowerControl(PowerControlDataProvider *dataProvider , QWidget *parent = nullptr);

    ~PowerControl();

    void refresh();

signals:

    void widgetEvent(const LenovoLegionGui::WidgetMessage& event);

private slots:

    /*
     * CPU
     */
    void on_comboBox_CPUBoostPowerControl_currentIndexChanged(int index);
    void on_comboBox_PL1TauPowerControl_currentIndexChanged(int index);
    void on_horizontalSlider_PowerTargetInACPowerControl_valueChanged(int value);
    void on_horizontalSlider_LTPowerLimitPowerControl_valueChanged(int value);
    void on_horizontalSlider_STPowerLimitPowerControl_valueChanged(int value);
    void on_horizontalSlider_CrossLPowerLimitPowerControl_valueChanged(int value);
    void on_horizontalSlider_CPUTempLimitPowerControl_valueChanged(int value);
    void on_pushButton_CPUPowerControlApply_clicked();
    void on_pushButton_CPUPwoerControlCancel_clicked();



    /*
     * GPU
     */
    void on_comboBox_GPUTargetPowerPowerControl_currentIndexChanged(int index);
    void on_comboBox_GPUBoostClockPowerControl_currentIndexChanged(int index);
    void on_horizontalSlider_GPUTempLimitPowerControl_valueChanged(int value);
    void on_pushButton_GPUPowerControlApply_clicked();
    void on_pushButton_GPUPowerControlCancel_clicked();


private:

    void renderData();

    void renderCpuControlData();
    void renderGpuControlData();

    void markChanges();
    void markChangesCpuControlData();
    void markChangesGpuControlData();


private:

    Ui::PowerControl         *ui;
    PowerControlDataProvider *m_dataProvider;

    LenovoLegionDaemon::PowerControl::CPU::DataInfo             m_cpuControlData;
    LenovoLegionDaemon::PowerControl::GPU::DataInfo             m_gpuControlData;
};

}
