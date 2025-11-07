// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "PowerControl.h"
#include "ui_PowerControl.h"
#include "DataProvider.h"

#include <MainWindow.h>

#include "../LenovoLegion-Daemon/SysFsDataProviderCPUPower.h"
#include "../LenovoLegion-Daemon/SysFsDataProviderGPUPower.h"
#include "../LenovoLegion-Daemon/SysFsDataProviderCPUPowerRapl.h"
#include "../LenovoLegion-Daemon/SysFsDataProviderPowerProfile.h"



#include <QStringList>
#include <QList>

namespace LenovoLegionGui {


struct __attribute__ ((__packed__)) Data {
    quint8 m_shortTermPowerLimit;
    quint8 m_longTermPowerLimit;
    quint8 m_crossLoadingPowerLimit;
    quint8 m_tempLimit;
    quint8 m_l1Tau;
    quint8 m_totalPowerTargetInAC;
    quint8 m_powerBoost;
};

const QString PowerControl::NAME = "Power Control";



PowerControl::PowerControl(DataProvider *dataProvider,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PowerControl)
    , m_dataProvider(dataProvider)
{
    ui->setupUi(this);

    /*
     * Read Data
     */
    m_cpuControlData    = m_dataProvider->getDataMessage<legion::messages::CPUPower> (LenovoLegionDaemon::SysFsDataProviderCPUPower::dataType);
    m_gpuControlData    = m_dataProvider->getDataMessage<legion::messages::GPUPower> (LenovoLegionDaemon::SysFsDataProviderGPUPower::dataType);
    m_cpuRaplData       = m_dataProvider->getDataMessage<legion::messages::CPUPowerRapl> (LenovoLegionDaemon::SysFsDataProviderCPUPowerRapl::dataType);
    m_powerProfileData  = m_dataProvider->getDataMessage<legion::messages::PowerProfile> (LenovoLegionDaemon::SysFsDataProviderPowerProfile::dataType);


    if(!m_cpuControlData.has_cpu_clp_limit()            ||
       !m_cpuControlData.has_cpu_ltp_limit()            ||
       !m_cpuControlData.has_cpu_stp_limit()            ||
       !m_cpuControlData.has_cpu_tmp_limit()            ||
       !m_cpuControlData.has_gpu_to_cpu_dynamic_boost() ||
       !m_cpuControlData.has_gpu_total_onac()           ||
       !m_gpuControlData.has_gpu_configurable_tgp()     ||
       !m_gpuControlData.has_gpu_power_boost()          ||
       !m_gpuControlData.has_gpu_temperature_limit()    ||
       !m_cpuRaplData.has_ltp_limit()                   ||
       !m_cpuRaplData.has_stp_limit()                   ||
       !m_powerProfileData.has_thermal_mode()
       )
    {
        THROW_EXCEPTION(exception_T, ERROR_CODES::DATA_NOT_READY, "Power Control data not available");
    }

    auto qlistToQStringList = [] (const QList<int>& list) {
        QStringList stringList;
        for (const auto& item : list) {
            stringList.append(QString::number(item).trimmed());
        }
        return stringList;
    };



    /*
     * Setup GUI elements
     */
    ui->comboBox_CPUBoostPowerControl->clear();
    ui->comboBox_CPUBoostPowerControl->addItems(qlistToQStringList(QList<int>(m_cpuControlData.gpu_to_cpu_dynamic_boost().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).steps().begin(),
                                                                              m_cpuControlData.gpu_to_cpu_dynamic_boost().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).steps().end()
                                                                              )
                                                                   )
                                                );

    ui->comboBox_PL1TauPowerControl->clear();
    ui->comboBox_PL1TauPowerControl->addItems(qlistToQStringList(QList<int>(m_cpuControlData.cpu_pl1_tau().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).steps().begin(),
                                                                              m_cpuControlData.cpu_pl1_tau().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).steps().end()
                                                                              )
                                                                   )
                                                );


    ui->comboBox_GPUTargetPowerPowerControl->clear();
    ui->comboBox_GPUTargetPowerPowerControl->addItems(qlistToQStringList(QList<int>(m_gpuControlData.gpu_configurable_tgp().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).steps().begin(),
                                                                            m_gpuControlData.gpu_configurable_tgp().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).steps().end()
                                                                            )
                                                                 )
                                              );


    ui->comboBox_GPUBoostClockPowerControl->clear();
    ui->comboBox_GPUBoostClockPowerControl->addItems(qlistToQStringList(QList<int>(m_gpuControlData.gpu_power_boost().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).steps().begin(),
                                                                                    m_gpuControlData.gpu_power_boost().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).steps().end()
                                                                                    )
                                                                         )
                                                      );


    ui->horizontalSlider_PowerTargetInACPowerControl->setMinimum(m_cpuControlData.gpu_total_onac().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).min_value());
    ui->horizontalSlider_PowerTargetInACPowerControl->setMaximum(m_cpuControlData.gpu_total_onac().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).max_value());

    ui->horizontalSlider_LTPowerLimitPowerControl->setMinimum(m_cpuControlData.cpu_ltp_limit().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).min_value());
    ui->horizontalSlider_LTPowerLimitPowerControl->setMaximum(m_cpuControlData.cpu_ltp_limit().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).max_value());

    ui->horizontalSlider_STPowerLimitPowerControl->setMinimum(m_cpuControlData.cpu_stp_limit().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).min_value());
    ui->horizontalSlider_STPowerLimitPowerControl->setMaximum(m_cpuControlData.cpu_stp_limit().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).max_value());

    ui->horizontalSlider_CrossLPowerLimitPowerControl->setMinimum(m_cpuControlData.cpu_clp_limit().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).min_value());
    ui->horizontalSlider_CrossLPowerLimitPowerControl->setMaximum(m_cpuControlData.cpu_clp_limit().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).max_value());

    ui->horizontalSlider_CPUTempLimitPowerControl->setMinimum(m_cpuControlData.cpu_tmp_limit().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).min_value());
    ui->horizontalSlider_CPUTempLimitPowerControl->setMaximum(m_cpuControlData.cpu_tmp_limit().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).max_value());

    ui->horizontalSlider_GPUTempLimitPowerControl->setMinimum(m_gpuControlData.gpu_temperature_limit().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).min_value());
    ui->horizontalSlider_GPUTempLimitPowerControl->setMaximum(m_gpuControlData.gpu_temperature_limit().mode_descriptor_map().at(legion::messages::PowerProfile::POWER_PROFILE_CUSTOM).max_value());


    ui->comboBox_PresetConfiguration->addItem("NONE");
    for (auto clpI = m_cpuControlData.cpu_clp_limit().mode_descriptor_map().begin(); clpI != m_cpuControlData.cpu_clp_limit().mode_descriptor_map().end(); ++clpI)
    {
        ui->comboBox_PresetConfiguration->addItem(legion::messages::PowerProfile::Profiles_descriptor()->FindValueByNumber(clpI->first)->name().data());
    }
    ui->comboBox_PresetConfiguration->setCurrentText("NONE");

    renderData();
    markChanges();
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
    legion::messages::CPUPower cpuPower;
    legion::messages::CPUPowerRapl cpuPowerRapl;

    cpuPower.mutable_cpu_clp_limit()->set_current_value((quint8)ui->horizontalSlider_CrossLPowerLimitPowerControl->value());
    cpuPower.mutable_cpu_ltp_limit()->set_current_value((quint8)ui->horizontalSlider_LTPowerLimitPowerControl->value());
    cpuPower.mutable_cpu_stp_limit()->set_current_value((quint8)ui->horizontalSlider_STPowerLimitPowerControl->value());
    cpuPower.mutable_cpu_tmp_limit()->set_current_value((quint8)ui->horizontalSlider_CPUTempLimitPowerControl->value());
    cpuPower.mutable_gpu_total_onac()->set_current_value((quint8)ui->horizontalSlider_PowerTargetInACPowerControl->value());
    cpuPower.mutable_gpu_to_cpu_dynamic_boost()->set_current_value((quint8)ui->comboBox_CPUBoostPowerControl->currentText().toUShort());
    cpuPower.mutable_cpu_pl1_tau()->set_current_value((quint8)ui->comboBox_PL1TauPowerControl->currentText().toUShort());


    cpuPowerRapl.mutable_ltp_limit()->set_power_limit_uw((quint32)(ui->horizontalSlider_LTPowerLimitPowerControl->value() * 1000000));
    cpuPowerRapl.mutable_ltp_limit()->set_time_window_us((quint32)(ui->comboBox_PL1TauPowerControl->currentText().toUShort() * 1000000));
    cpuPowerRapl.mutable_stp_limit()->set_power_limit_uw((quint32)(ui->horizontalSlider_STPowerLimitPowerControl->value() * 1000000));



    //CPU Power Control Apply
    m_dataProvider->setDataMessage(LenovoLegionDaemon::SysFsDataProviderCPUPower::dataType,cpuPower);
    m_dataProvider->setDataMessage(LenovoLegionDaemon::SysFsDataProviderCPUPowerRapl::dataType,cpuPowerRapl);

    m_cpuControlData = m_dataProvider->getDataMessage<legion::messages::CPUPower> (LenovoLegionDaemon::SysFsDataProviderCPUPower::dataType);
    m_cpuRaplData    = m_dataProvider->getDataMessage<legion::messages::CPUPowerRapl> (LenovoLegionDaemon::SysFsDataProviderCPUPowerRapl::dataType);


    renderCpuControlData();
    markChangesCpuControlData();
}

void PowerControl::on_pushButton_CPUPwoerControlCancel_clicked()
{
    renderCpuControlData();
}

void PowerControl::on_comboBox_CPUBoostPowerControl_currentIndexChanged(int)
{
    markChangesCpuControlData();
}

void PowerControl::on_comboBox_PL1TauPowerControl_currentIndexChanged(int)
{
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
    ui->lcdNumber_CPUTempLimitPowerControl->display(value);
    markChangesCpuControlData();
}

void PowerControl::on_comboBox_GPUTargetPowerPowerControl_currentIndexChanged(int)
{
    markChangesGpuControlData();
}

void PowerControl::on_comboBox_GPUBoostClockPowerControl_currentIndexChanged(int)
{
    markChangesGpuControlData();
}

void PowerControl::on_horizontalSlider_GPUTempLimitPowerControl_valueChanged(int value)
{
    ui->lcdNumber_GPUTempLimitPowerControl->display(value);
    markChangesGpuControlData();
}

void PowerControl::on_pushButton_GPUPowerControlApply_clicked()
{
    legion::messages::GPUPower gpuPower;

    gpuPower.mutable_gpu_power_boost()->set_current_value((quint8)ui->comboBox_GPUBoostClockPowerControl->currentText().toUShort());
    gpuPower.mutable_gpu_configurable_tgp()->set_current_value((quint8)ui->comboBox_GPUTargetPowerPowerControl->currentText().toUShort());
    gpuPower.mutable_gpu_temperature_limit()->set_current_value((quint8)ui->horizontalSlider_GPUTempLimitPowerControl->value());

    //GPU Power Control Apply
    m_dataProvider->setDataMessage(LenovoLegionDaemon::SysFsDataProviderGPUPower::dataType,gpuPower);
    m_gpuControlData = m_dataProvider->getDataMessage<legion::messages::GPUPower> (LenovoLegionDaemon::SysFsDataProviderGPUPower::dataType);
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

    if(m_powerProfileData.thermal_mode() == legion::messages::PowerProfile::POWER_PROFILE_CUSTOM)
    {
        ui->comboBox_PresetConfiguration->setEnabled(true);
    }
    else
    {
        ui->comboBox_PresetConfiguration->setEnabled(false);
    }
}

void PowerControl::renderGpuControlData()
{
    if(m_powerProfileData.thermal_mode() == legion::messages::PowerProfile::POWER_PROFILE_CUSTOM)
    {
        ui->comboBox_GPUTargetPowerPowerControl->setCurrentIndex(ui->comboBox_GPUTargetPowerPowerControl->findText(QString::number(m_gpuControlData.gpu_configurable_tgp().current_value())));
        ui->comboBox_GPUBoostClockPowerControl->setCurrentIndex(ui->comboBox_GPUBoostClockPowerControl->findText(QString::number(m_gpuControlData.gpu_power_boost().current_value())));

        ui->horizontalSlider_GPUTempLimitPowerControl->setValue(m_gpuControlData.gpu_temperature_limit().current_value());
        ui->lcdNumber_GPUTempLimitPowerControl->display(static_cast<int>(m_gpuControlData.gpu_temperature_limit().current_value()));


        ui->groupBox_GPUPowerControl->setEnabled(true);
    }
    else
    {
        ui->comboBox_GPUTargetPowerPowerControl->setCurrentIndex(ui->comboBox_GPUTargetPowerPowerControl->findText(QString::number(m_gpuControlData.gpu_configurable_tgp().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value())));
        ui->comboBox_GPUBoostClockPowerControl->setCurrentIndex(ui->comboBox_GPUBoostClockPowerControl->findText(QString::number(m_gpuControlData.gpu_power_boost().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value())));

        ui->horizontalSlider_GPUTempLimitPowerControl->setValue(m_gpuControlData.gpu_temperature_limit().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value());
        ui->lcdNumber_GPUTempLimitPowerControl->display(static_cast<int>(m_gpuControlData.gpu_temperature_limit().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value()));

        ui->groupBox_GPUPowerControl->setEnabled(false);
    }

    ui->comboBox_PresetConfiguration->setCurrentText("NONE");
}

void PowerControl::markChanges()
{
    if(m_powerProfileData.thermal_mode() != legion::messages::PowerProfile::POWER_PROFILE_CUSTOM)
    {
        return;
    }

    markChangesCpuControlData();
    markChangesGpuControlData();
}

void PowerControl::markChangesCpuControlData()
{
    if(m_powerProfileData.thermal_mode() != legion::messages::PowerProfile::POWER_PROFILE_CUSTOM)
    {
        return;
    }


    if(static_cast<uint32_t>(ui->horizontalSlider_LTPowerLimitPowerControl->value()) != (m_cpuRaplData.ltp_limit().power_limit_uw() / 1000000))
    {
        ui->label_LTPowerLimitPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_LTPowerLimitPowerControl->setStyleSheet(QString("QLabel { }"));
    }

    if(static_cast<uint32_t>(ui->horizontalSlider_STPowerLimitPowerControl->value()) != (m_cpuRaplData.stp_limit().power_limit_uw() / 1000000))
    {
        ui->label_STPowerLimitPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_STPowerLimitPowerControl->setStyleSheet(QString("QLabel { }"));
    }

    if(static_cast<uint32_t>(ui->horizontalSlider_CrossLPowerLimitPowerControl->value()) != m_cpuControlData.cpu_clp_limit().current_value())
    {
        ui->label_CrossLPowerLimitPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_CrossLPowerLimitPowerControl->setStyleSheet(QString("QLabel { }"));
    }

    if(static_cast<uint32_t>(ui->horizontalSlider_CPUTempLimitPowerControl->value()) != m_cpuControlData.cpu_tmp_limit().current_value())
    {
        ui->label_CPUTempLimitPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_CPUTempLimitPowerControl->setStyleSheet(QString("QLabel { }"));
    }

    if(static_cast<uint32_t>(ui->comboBox_PL1TauPowerControl->currentText().toUShort()) != ((m_cpuRaplData.ltp_limit().time_window_us() + 1000000 )/ 1000000))
    {
        ui->label_PL1TauPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_PL1TauPowerControl->setStyleSheet(QString("QLabel { }"));
    }

    if(static_cast<uint32_t>(ui->horizontalSlider_PowerTargetInACPowerControl->value()) != m_cpuControlData.gpu_total_onac().current_value())
    {
        ui->label_PowerTargetInACPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_PowerTargetInACPowerControl->setStyleSheet(QString("QLabel { }"));
    }

    if(static_cast<uint32_t>(ui->comboBox_CPUBoostPowerControl->currentText().toUShort()) != m_cpuControlData.gpu_to_cpu_dynamic_boost().current_value())
    {
        ui->label_CPUBoostPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_CPUBoostPowerControl->setStyleSheet(QString("QLabel { }"));
    }
}

void PowerControl::markChangesGpuControlData()
{

    if(m_powerProfileData.thermal_mode() != legion::messages::PowerProfile::POWER_PROFILE_CUSTOM)
    {
        return;
    }


    if(static_cast<uint32_t>(ui->horizontalSlider_GPUTempLimitPowerControl->value()) != m_gpuControlData.gpu_temperature_limit().current_value())
    {
        ui->label_GPUTempLimitPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_GPUTempLimitPowerControl->setStyleSheet(QString("QLabel { }"));
    }


    if(static_cast<uint32_t>(ui->comboBox_GPUBoostClockPowerControl->currentText().toShort()) != m_gpuControlData.gpu_power_boost().current_value())
    {
        ui->label_GPUBoostClockPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_GPUBoostClockPowerControl->setStyleSheet(QString("QLabel { }"));
    }

    if(static_cast<uint32_t>(ui->comboBox_GPUTargetPowerPowerControl->currentText().toUShort()) != m_gpuControlData.gpu_configurable_tgp().current_value())
    {
        ui->label_GPUTargetPowerPowerControl->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_GPUTargetPowerPowerControl->setStyleSheet(QString("QLabel { }"));
    }
}

void PowerControl::renderCpuControlData()
{
    if(m_powerProfileData.thermal_mode() == legion::messages::PowerProfile::POWER_PROFILE_CUSTOM)
    {
        legion::messages::CPUPowerRapl cpuPowerRapl;

        ui->horizontalSlider_LTPowerLimitPowerControl->setValue(m_cpuControlData.cpu_ltp_limit().current_value());
        ui->lcdNumber_LTPowerLimitPowerControl->display(static_cast<int>(m_cpuControlData.cpu_ltp_limit().current_value()));

        ui->horizontalSlider_CrossLPowerLimitPowerControl->setValue(m_cpuControlData.cpu_clp_limit().current_value());
        ui->lcdNumber_CrossLPowerLimitPowerControl->display(static_cast<int>(m_cpuControlData.cpu_clp_limit().current_value()));

        ui->horizontalSlider_STPowerLimitPowerControl->setValue(m_cpuControlData.cpu_stp_limit().current_value() );
        ui->lcdNumber_STPowerLimitPowerControl->display(static_cast<int>(m_cpuControlData.cpu_stp_limit().current_value()));

        ui->horizontalSlider_CPUTempLimitPowerControl->setValue(m_cpuControlData.cpu_tmp_limit().current_value());
        ui->lcdNumber_CPUTempLimitPowerControl->display(static_cast<int>(m_cpuControlData.cpu_tmp_limit().current_value()));

        ui->horizontalSlider_PowerTargetInACPowerControl->setValue(m_cpuControlData.gpu_total_onac().current_value());
        ui->lcdNumber_PowerTargetInACPowerControl->display(static_cast<int>(m_cpuControlData.gpu_total_onac().current_value()));

        ui->comboBox_PL1TauPowerControl->setCurrentIndex(ui->comboBox_PL1TauPowerControl->findText(QString::number((m_cpuControlData.cpu_pl1_tau().current_value()))));


        ui->groupBox_CPUPowerControl->setEnabled(true);


        cpuPowerRapl.mutable_ltp_limit()->set_power_limit_uw((quint32)(ui->horizontalSlider_LTPowerLimitPowerControl->value() * 1000000));
        cpuPowerRapl.mutable_ltp_limit()->set_time_window_us((quint32)(ui->comboBox_PL1TauPowerControl->currentText().toUShort() * 1000000));
        cpuPowerRapl.mutable_stp_limit()->set_power_limit_uw((quint32)(ui->horizontalSlider_STPowerLimitPowerControl->value() * 1000000));

        m_dataProvider->setDataMessage(LenovoLegionDaemon::SysFsDataProviderCPUPowerRapl::dataType,cpuPowerRapl);

        m_cpuRaplData = m_dataProvider->getDataMessage<legion::messages::CPUPowerRapl> (LenovoLegionDaemon::SysFsDataProviderCPUPowerRapl::dataType);
    }
    else
    {
        legion::messages::CPUPowerRapl cpuPowerRapl;

        ui->horizontalSlider_LTPowerLimitPowerControl->setValue(m_cpuControlData.cpu_ltp_limit().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value());
        ui->lcdNumber_LTPowerLimitPowerControl->display(static_cast<int>(m_cpuControlData.cpu_ltp_limit().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value()));

        ui->horizontalSlider_CrossLPowerLimitPowerControl->setValue(m_cpuControlData.cpu_clp_limit().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value());
        ui->lcdNumber_CrossLPowerLimitPowerControl->display(static_cast<int>(m_cpuControlData.cpu_clp_limit().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value()));

        ui->horizontalSlider_STPowerLimitPowerControl->setValue(m_cpuControlData.cpu_stp_limit().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value());
        ui->lcdNumber_STPowerLimitPowerControl->display(static_cast<int>(m_cpuControlData.cpu_stp_limit().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value()));

        ui->horizontalSlider_CPUTempLimitPowerControl->setValue(m_cpuControlData.cpu_tmp_limit().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value());
        ui->lcdNumber_CPUTempLimitPowerControl->display(static_cast<int>(m_cpuControlData.cpu_tmp_limit().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value()));

        ui->horizontalSlider_PowerTargetInACPowerControl->setValue(m_cpuControlData.gpu_total_onac().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value());
        ui->lcdNumber_PowerTargetInACPowerControl->display(static_cast<int>(m_cpuControlData.gpu_total_onac().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value()));

        ui->comboBox_PL1TauPowerControl->setCurrentIndex(ui->comboBox_PL1TauPowerControl->findText(QString::number(m_cpuControlData.cpu_pl1_tau().mode_descriptor_map().at(m_powerProfileData.current_value()).default_value())));

        ui->groupBox_CPUPowerControl->setEnabled(false);

        cpuPowerRapl.mutable_ltp_limit()->set_power_limit_uw((quint32)(ui->horizontalSlider_LTPowerLimitPowerControl->value() * 1000000));
        cpuPowerRapl.mutable_ltp_limit()->set_time_window_us((quint32)(ui->comboBox_PL1TauPowerControl->currentText().toUShort() * 1000000));
        cpuPowerRapl.mutable_stp_limit()->set_power_limit_uw((quint32)(ui->horizontalSlider_STPowerLimitPowerControl->value() * 1000000));

        m_dataProvider->setDataMessage(LenovoLegionDaemon::SysFsDataProviderCPUPowerRapl::dataType,cpuPowerRapl);

        m_cpuRaplData = m_dataProvider->getDataMessage<legion::messages::CPUPowerRapl> (LenovoLegionDaemon::SysFsDataProviderCPUPowerRapl::dataType);
    }

    ui->comboBox_PresetConfiguration->setCurrentText("NONE");
}

void PowerControl::on_comboBox_PresetConfiguration_currentTextChanged(const QString &arg1)
{
    if(arg1 == "NONE")
        return;

    ui->horizontalSlider_LTPowerLimitPowerControl->setValue(m_cpuControlData.cpu_ltp_limit().mode_descriptor_map().at(legion::messages::PowerProfile::Profiles_descriptor()->FindValueByName(arg1.toStdString().c_str())->number()).default_value());
    ui->horizontalSlider_STPowerLimitPowerControl->setValue(m_cpuControlData.cpu_stp_limit().mode_descriptor_map().at(legion::messages::PowerProfile::Profiles_descriptor()->FindValueByName(arg1.toStdString().c_str())->number()).default_value());
    ui->horizontalSlider_CrossLPowerLimitPowerControl->setValue(m_cpuControlData.cpu_clp_limit().mode_descriptor_map().at(legion::messages::PowerProfile::Profiles_descriptor()->FindValueByName(arg1.toStdString().c_str())->number()).default_value());
    ui->horizontalSlider_CPUTempLimitPowerControl->setValue(m_cpuControlData.cpu_tmp_limit().mode_descriptor_map().at(legion::messages::PowerProfile::Profiles_descriptor()->FindValueByName(arg1.toStdString().c_str())->number()).default_value());
    ui->horizontalSlider_PowerTargetInACPowerControl->setValue(m_cpuControlData.gpu_total_onac().mode_descriptor_map().at(legion::messages::PowerProfile::Profiles_descriptor()->FindValueByName(arg1.toStdString().c_str())->number()).default_value());
    ui->comboBox_PL1TauPowerControl->setCurrentIndex(ui->comboBox_PL1TauPowerControl->findText(QString::number(m_cpuControlData.cpu_pl1_tau().mode_descriptor_map().at(legion::messages::PowerProfile::Profiles_descriptor()->FindValueByName(arg1.toStdString().c_str())->number()).default_value())));

    ui->comboBox_CPUBoostPowerControl->setCurrentIndex(ui->comboBox_CPUBoostPowerControl->findText(QString::number(m_cpuControlData.gpu_to_cpu_dynamic_boost().mode_descriptor_map().at(legion::messages::PowerProfile::Profiles_descriptor()->FindValueByName(arg1.toStdString().c_str())->number()).default_value())));
    ui->comboBox_GPUTargetPowerPowerControl->setCurrentIndex(ui->comboBox_GPUTargetPowerPowerControl->findText(QString::number(m_gpuControlData.gpu_configurable_tgp().mode_descriptor_map().at(legion::messages::PowerProfile::Profiles_descriptor()->FindValueByName(arg1.toStdString().c_str())->number()).default_value())));
    ui->comboBox_GPUBoostClockPowerControl->setCurrentIndex(ui->comboBox_GPUBoostClockPowerControl->findText(QString::number(m_gpuControlData.gpu_power_boost().mode_descriptor_map().at(legion::messages::PowerProfile::Profiles_descriptor()->FindValueByName(arg1.toStdString().c_str())->number()).default_value())));
    ui->horizontalSlider_GPUTempLimitPowerControl->setValue(m_gpuControlData.gpu_temperature_limit().mode_descriptor_map().at(legion::messages::PowerProfile::Profiles_descriptor()->FindValueByName(arg1.toStdString().c_str())->number()).default_value());
}


}
