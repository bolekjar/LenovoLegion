// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "PowerControl.h"
#include "ui_PowerControl.h"

#include <PowerControlDataProvider.h>
#include <MainWindow.h>

namespace LenovoLegionGui {

const QString PowerControl::NAME = "Power Control";

const QMap<QString,LenovoLegionDaemon::PowerControl::CPU::DataControl> PowerControl::CPU_PRESETS = {
    {"MAXIMUM_GAME",{.m_data        = {100,80,110,97,56,110,0}}},
    {"MAXIMUM_MULTICORE",{.m_data   = {190,140,60,97,56,60,0}}},
    {"OPTIMUM_MULTICORE",{.m_data   = {140,120,60,97,56,60,0}}},
    {"SILENT_MULTICORE" ,{.m_data   = {110,90,60,97,56,60,0}}}
};

const QMap<QString,LenovoLegionDaemon::PowerControl::GPU::DataControl> PowerControl::GPU_PRESETS ={
    {"MAXIMUM_GAME",{.m_data        = {25,150,87}}},
    {"MAXIMUM_MULTICORE",{.m_data   = {0,120,87}}},
    {"OPTIMUM_MULTICORE",{.m_data   = {0,120,87}}},
    {"SILENT_MULTICORE" ,{.m_data   = {0,120,87}}}
};
PowerControl::PowerControl(PowerControlDataProvider *dataProvider,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PowerControl)
    , m_dataProvider(dataProvider)
{
    ui->setupUi(this);

    /*
     * Read Data
     */
    m_cpuControlData  = m_dataProvider->getCpuControlData();
    m_gpuControlData  = m_dataProvider->getGpuControlData();

    renderData();
}

PowerControl::~PowerControl()
{
    delete ui;
}

void PowerControl::refresh()
{
    renderData();
}

void PowerControl::on_pushButton_CPUPowerControlApply_clicked()
{
    //CPU Power Control Apply
    m_dataProvider->setCpuControlData(LenovoLegionDaemon::PowerControl::CPU::DataControl{
                                                                                         .m_data = {
                                                                                             .m_shortTermPowerLimit  = (quint8)ui->horizontalSlider_STPowerLimitPowerControl->value(),
                                                                                             .m_longTermPowerLimit = (quint8)ui->horizontalSlider_LTPowerLimitPowerControl->value(),
                                                                                             .m_crossLoadingPowerLimit   = (quint8)ui->horizontalSlider_CrossLPowerLimitPowerControl->value(),
                                                                                             .m_tempLimit = (quint8)ui->horizontalSlider_CPUTempLimitPowerControl->value(),
                                                                                             .m_l1Tau = (quint8)ui->comboBox_PL1TauPowerControl->currentText().toUShort(),
                                                                                             .m_totalPowerTargetInAC = (quint8)ui->horizontalSlider_PowerTargetInACPowerControl->value(),
                                                                                             .m_powerBoost = (quint8)ui->comboBox_CPUBoostPowerControl->currentText().toUShort()
                                                                                        }
    });

    m_cpuControlData = m_dataProvider->getCpuControlData();
    renderCpuControlData();
    markChangesCpuControlData();
}

void PowerControl::on_pushButton_CPUPwoerControlCancel_clicked()
{
    renderCpuControlData();
}

void PowerControl::on_comboBox_CPUBoostPowerControl_currentIndexChanged(int)
{
    //GPU to CPU Dynamic Boost 0,5,10,15 W
    markChangesCpuControlData();
}

void PowerControl::on_comboBox_PL1TauPowerControl_currentIndexChanged(int)
{
    //PL1 Tau : 20 24 28 32 40 48 56 64 80 96 112 128 160
    markChangesCpuControlData();
}

void PowerControl::on_horizontalSlider_PowerTargetInACPowerControl_valueChanged(int value)
{
    ui->lcdNumber_PowerTargetInACPowerControl->display(value);
    markChangesCpuControlData();
}

void PowerControl::on_horizontalSlider_LTPowerLimitPowerControl_valueChanged(int value)
{
    ui->lcdNumber_LTPowerLimitPowerControl->display(value);
    markChangesCpuControlData();
}

void PowerControl::on_horizontalSlider_STPowerLimitPowerControl_valueChanged(int value)
{
    ui->lcdNumber_STPowerLimitPowerControl->display(value);
    markChangesCpuControlData();
}

void PowerControl::on_horizontalSlider_CrossLPowerLimitPowerControl_valueChanged(int value)
{
    ui->lcdNumber_CrossLPowerLimitPowerControl->display(value);
    markChangesCpuControlData();
}

void PowerControl::on_horizontalSlider_CPUTempLimitPowerControl_valueChanged(int value)
{
    //CPU Temperature Limit - 85 - 100 W
    ui->lcdNumber_CPUTempLimitPowerControl->display(value);
    markChangesCpuControlData();
}

void PowerControl::on_comboBox_GPUTargetPowerPowerControl_currentIndexChanged(int)
{
    markChangesGpuControlData();
}

void PowerControl::on_comboBox_GPUBoostClockPowerControl_currentIndexChanged(int)
{
    //GPU to CPU Dynamic Boost 0,5,10,15 W
    markChangesGpuControlData();
}

void PowerControl::on_horizontalSlider_GPUTempLimitPowerControl_valueChanged(int value)
{
    //GPU Temperature Limit - 75-87 W
    ui->lcdNumber_GPUTempLimitPowerControl->display(value);
    markChangesGpuControlData();
}

void PowerControl::on_pushButton_GPUPowerControlApply_clicked()
{
    //GPU Power Control Apply
    m_dataProvider->setGpuControlData(LenovoLegionDaemon::PowerControl::GPU::DataControl{
                                                                                         .m_data = {
                                                                                           .m_boostLimit  = (quint8)ui->comboBox_GPUBoostClockPowerControl->currentText().toUShort(),
                                                                                           .m_targetLimit = (quint8)ui->comboBox_GPUTargetPowerPowerControl->currentText().toUShort(),
                                                                                           .m_tempLimit   = (quint8)ui->horizontalSlider_GPUTempLimitPowerControl->value()
                                                                                        }

    });
    m_gpuControlData = m_dataProvider->getGpuControlData();
    renderGpuControlData();
    markChangesGpuControlData();
}

void PowerControl::on_pushButton_GPUPowerControlCancel_clicked()
{
    renderGpuControlData();
}

void PowerControl::renderData()
{
    renderCpuControlData();
    renderGpuControlData();
}

void PowerControl::renderGpuControlData()
{
    ui->comboBox_GPUTargetPowerPowerControl->setCurrentIndex(ui->comboBox_GPUTargetPowerPowerControl->findText(QString::number(m_gpuControlData.m_data.m_targetLimit)));
    ui->comboBox_GPUBoostClockPowerControl->setCurrentIndex(ui->comboBox_GPUBoostClockPowerControl->findText(QString::number(m_gpuControlData.m_data.m_boostLimit)));

    ui->horizontalSlider_GPUTempLimitPowerControl->setValue(m_gpuControlData.m_data.m_tempLimit);
    ui->lcdNumber_GPUTempLimitPowerControl->display(m_gpuControlData.m_data.m_tempLimit);
}

void PowerControl::markChanges()
{
    markChangesCpuControlData();
    markChangesGpuControlData();
}

void PowerControl::markChangesCpuControlData()
{
    if(ui->horizontalSlider_LTPowerLimitPowerControl->value() != m_cpuControlData.m_data.m_longTermPowerLimit)
    {
        ui->label_LTPowerLimitPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_LTPowerLimitPowerControl->setStyleSheet(QString());
    }

    if(ui->horizontalSlider_STPowerLimitPowerControl->value() != m_cpuControlData.m_data.m_shortTermPowerLimit)
    {
        ui->label_STPowerLimitPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_STPowerLimitPowerControl->setStyleSheet(QString());
    }

    if(ui->horizontalSlider_CrossLPowerLimitPowerControl->value() != m_cpuControlData.m_data.m_crossLoadingPowerLimit)
    {
        ui->label_CrossLPowerLimitPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_CrossLPowerLimitPowerControl->setStyleSheet(QString());
    }

    if(ui->horizontalSlider_CPUTempLimitPowerControl->value() != m_cpuControlData.m_data.m_tempLimit)
    {
        ui->label_CPUTempLimitPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_CPUTempLimitPowerControl->setStyleSheet(QString());
    }

    if(ui->comboBox_PL1TauPowerControl->currentText().toUShort() != m_cpuControlData.m_data.m_l1Tau)
    {
        ui->label_PL1TauPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_PL1TauPowerControl->setStyleSheet(QString());
    }

    if(ui->horizontalSlider_PowerTargetInACPowerControl->value() != m_cpuControlData.m_data.m_totalPowerTargetInAC)
    {
        ui->label_PowerTargetInACPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_PowerTargetInACPowerControl->setStyleSheet(QString());
    }

    if(ui->comboBox_CPUBoostPowerControl->currentText().toUShort() != m_cpuControlData.m_data.m_powerBoost)
    {
        ui->label_CPUBoostPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_CPUBoostPowerControl->setStyleSheet(QString());
    }
}

void PowerControl::markChangesGpuControlData()
{

    if(ui->horizontalSlider_GPUTempLimitPowerControl->value() != m_gpuControlData.m_data.m_tempLimit)
    {
        ui->label_GPUTempLimitPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_GPUTempLimitPowerControl->setStyleSheet(QString());
    }


    if(ui->comboBox_GPUBoostClockPowerControl->currentText().toShort() != m_gpuControlData.m_data.m_boostLimit)
    {
        ui->label_GPUBoostClockPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_GPUBoostClockPowerControl->setStyleSheet(QString());
    }

    if(ui->comboBox_GPUTargetPowerPowerControl->currentText().toUShort() != m_gpuControlData.m_data.m_targetLimit)
    {
        ui->label_GPUTargetPowerPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_GPUTargetPowerPowerControl->setStyleSheet(QString());
    }
}

void PowerControl::renderCpuControlData()
{
    ui->horizontalSlider_LTPowerLimitPowerControl->setValue(m_cpuControlData.m_data.m_longTermPowerLimit);
    ui->lcdNumber_LTPowerLimitPowerControl->display(m_cpuControlData.m_data.m_longTermPowerLimit);

    ui->horizontalSlider_CrossLPowerLimitPowerControl->setValue(m_cpuControlData.m_data.m_crossLoadingPowerLimit);
    ui->lcdNumber_CrossLPowerLimitPowerControl->display(m_cpuControlData.m_data.m_crossLoadingPowerLimit);

    ui->horizontalSlider_STPowerLimitPowerControl->setValue(m_cpuControlData.m_data.m_shortTermPowerLimit);
    ui->lcdNumber_STPowerLimitPowerControl->display(m_cpuControlData.m_data.m_shortTermPowerLimit);

    ui->horizontalSlider_CPUTempLimitPowerControl->setValue(m_cpuControlData.m_data.m_tempLimit);
    ui->lcdNumber_CPUTempLimitPowerControl->display(m_cpuControlData.m_data.m_tempLimit);

    ui->horizontalSlider_PowerTargetInACPowerControl->setValue(m_cpuControlData.m_data.m_totalPowerTargetInAC);

    ui->comboBox_PL1TauPowerControl->setCurrentIndex(ui->comboBox_PL1TauPowerControl->findText(QString::number(m_cpuControlData.m_data.m_l1Tau)));
}

}
