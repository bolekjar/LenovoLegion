// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "CPUFrequencyControl.h"
#include "ui_CPUFrequencyControl.h"

#include "ThreadFrequencyControl.h"

#include <CPUFrequencyControlDataProvider.h>

namespace LenovoLegionGui {

QString const CPUFrequencyControl::NAME =  "CPU Frequency Control";

const QMap<QString,LenovoLegionDaemon::CPUXFreqControl::DataControl::CPUX> CPUFrequencyControl::CPUFREQ_PRESETS ={
    {"MAX_MAX", {.m_cpuScalingMinFreq = 5600 * 1000,.m_cpuScalingMaxFreq = 5600 * 1000}},
    {"MIN_MAX", {.m_cpuScalingMinFreq = 800  * 1000, .m_cpuScalingMaxFreq = 5600 * 1000}},
    {"MIN_3000",{.m_cpuScalingMinFreq = 800  * 1000, .m_cpuScalingMaxFreq = 3000 * 1000}},
    {"MIN_4000",{.m_cpuScalingMinFreq = 800  * 1000, .m_cpuScalingMaxFreq = 4000 * 1000}},
    {"3000_3000",{.m_cpuScalingMinFreq = 3000 * 1000, .m_cpuScalingMaxFreq = 3000 * 1000}},
    {"3500_3500",{.m_cpuScalingMinFreq = 3500 * 1000, .m_cpuScalingMaxFreq = 3500 * 1000}},
    {"4000_4000",{.m_cpuScalingMinFreq = 4000 * 1000, .m_cpuScalingMaxFreq = 4000 * 1000}},
    {"4500_4500",{.m_cpuScalingMinFreq = 4500 * 1000, .m_cpuScalingMaxFreq = 4500 * 1000}}
};

const QString CPUFrequencyControl::APPLY_TO_ALL = "All";
const QString CPUFrequencyControl::APPLY_TO_ALL_EFFICIENT = "Efficiency";
const QString CPUFrequencyControl::APPLY_TO_ALL_PERFORMANCE = "Performance";

CPUFrequencyControl::CPUFrequencyControl(CPUFrequencyControlDataProvider *dataProvider,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CPUFrequencyControl)
    , m_dataProvider(dataProvider)
{
    ui->setupUi(this);

    m_cpuFreqData           = m_dataProvider->getCPUsInfoData();
    m_cpuHetTopologyData    = m_dataProvider->getCPUHeterogenousTopologyData();
    m_cpuHomTopologyData    = m_dataProvider->getCPUHomogeneousTopologyData();



    /*
     * CPU Control render
     */

    forAllCpuPerformanceCores([this](const int i)
                              {
        ui->gridLayout_CPUFreqControl->addWidget(new ThreadFrequencyControl(QString("P-Thread: %1, CoreId: %2, DieId: %3").arg(i).arg(m_cpuFreqData.m_data.m_cpus.at(i).m_cpuCoreId).arg(m_cpuFreqData.m_data.m_cpus.at(i).m_dieId),
                                                                                                         m_cpuFreqData.m_data.m_cpus.at(i).m_cpuInfoMinFreq    / 1000,
                                                                                                         m_cpuFreqData.m_data.m_cpus.at(i).m_cpuInfoMaxFreq    / 1000,
                                                                                                         m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMinFreq / 1000,
                                                                                                         m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMaxFreq / 1000,
                                                                                                         "#919191",
                                                                                                         this
                                                                                                         )
                                                                                      ,
                                                                                      i % 8,i/8);
                                return true;
                              });

    /*
     * CPU Control render
     */
    forAllCpuEfficientCores([this](const int i){
        ui->gridLayout_CPUFreqControl->addWidget(new ThreadFrequencyControl(QString("E-Thread: %1, CoreId: %2, DieId: %3").arg(i).arg(m_cpuFreqData.m_data.m_cpus.at(i).m_cpuCoreId).arg(m_cpuFreqData.m_data.m_cpus.at(i).m_dieId),
                                                                             m_cpuFreqData.m_data.m_cpus.at(i).m_cpuInfoMinFreq    / 1000,
                                                                             m_cpuFreqData.m_data.m_cpus.at(i).m_cpuInfoMaxFreq    / 1000,
                                                                             m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMinFreq / 1000,
                                                                             m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMaxFreq / 1000,
                                                                             "#ababab",
                                                                             this
                                                                             )
                                                          ,
                                                          i % 8,i/8);
        return true;
    });


    ui->horizontalSlider_CPUFreqControlMin->blockSignals(true);
    ui->horizontalSlider_CPUFreqControlMax->blockSignals(true);
    forAllCPUThreads([this](const LenovoLegionDaemon::CPUXFreqControl::DataInfo::CPUX& cpuFreq, const size_t)
    {
        ui->horizontalSlider_CPUFreqControlMin->setMinimum(cpuFreq.m_cpuInfoMinFreq / 1000);
        ui->horizontalSlider_CPUFreqControlMin->setMaximum(cpuFreq.m_cpuInfoMaxFreq / 1000);

        ui->horizontalSlider_CPUFreqControlMax->setMinimum(cpuFreq.m_cpuInfoMinFreq / 1000);
        ui->horizontalSlider_CPUFreqControlMax->setMaximum(cpuFreq.m_cpuInfoMaxFreq / 1000);

        ui->horizontalSlider_CPUFreqControlMin->setValue(cpuFreq.m_cpuScalingMinFreq / 1000);
        ui->horizontalSlider_CPUFreqControlMax->setValue(cpuFreq.m_cpuScalingMaxFreq / 1000);

        ui->horizontalSlider_CPUFreqControlMin->setPageStep(1);
        ui->horizontalSlider_CPUFreqControlMax->setPageStep(1);
        return false;
    });
    forAllCPUThreads([this](const LenovoLegionDaemon::CPUXFreqControl::DataInfo::CPUX& cpuFreq, const size_t)
    {
        ui->horizontalSlider_CPUFreqControlMin->setMinimum(std::min(ui->horizontalSlider_CPUFreqControlMin->minimum(),static_cast<int>(cpuFreq.m_cpuInfoMinFreq / 1000)));
        ui->horizontalSlider_CPUFreqControlMin->setMaximum(std::max(ui->horizontalSlider_CPUFreqControlMin->maximum(),static_cast<int>(cpuFreq.m_cpuInfoMaxFreq / 1000)));

        ui->horizontalSlider_CPUFreqControlMax->setMinimum(std::min(ui->horizontalSlider_CPUFreqControlMax->minimum(),static_cast<int>(cpuFreq.m_cpuInfoMinFreq / 1000)));
        ui->horizontalSlider_CPUFreqControlMax->setMaximum(std::max(ui->horizontalSlider_CPUFreqControlMax->maximum(),static_cast<int>(cpuFreq.m_cpuInfoMaxFreq / 1000)));
        return true;
    });
    ui->horizontalSlider_CPUFreqControlMin->blockSignals(false);
    ui->horizontalSlider_CPUFreqControlMax->blockSignals(false);

    ui->comboBoxApplyTo->addItem(APPLY_TO_ALL);
    if(m_cpuHetTopologyData.m_isAvailable)
    {
        ui->comboBoxApplyTo->addItem(APPLY_TO_ALL_PERFORMANCE);
        ui->comboBoxApplyTo->addItem(APPLY_TO_ALL_EFFICIENT);
    }

    renderData();
}

void CPUFrequencyControl::on_horizontalSlider_CPUFreqControlMin_valueChanged(int value)
{
    ui->horizontalSlider_CPUFreqControlMax->setValue(ui->horizontalSlider_CPUFreqControlMax->value() < value ? value : ui->horizontalSlider_CPUFreqControlMax->value());

    if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL ||
       ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_PERFORMANCE)
    {
        forAllCpuPerformanceCores([this,value](const int i){
            dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->setMinFreq(value,m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMinFreq / 1000);
            return true;
        });
    }

    if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL ||
       ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_EFFICIENT)
    {
        forAllCpuEfficientCores([this,value](const int i){
            dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->setMinFreq(value,m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMinFreq / 1000);
            return true;
        });
    }
}

void CPUFrequencyControl::on_horizontalSlider_CPUFreqControlMax_valueChanged(int value)
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->value() > value ? value : ui->horizontalSlider_CPUFreqControlMin->value());

    if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL ||
        ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_PERFORMANCE)
    {
        forAllCpuPerformanceCores([this,value](const int i){
            dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->setMaxFreq(value,m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMaxFreq / 1000);
            return true;
        });
    }

    if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL ||
        ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_EFFICIENT)
    {
        forAllCpuEfficientCores([this,value](const int i){
            dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->setMaxFreq(value,m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMaxFreq / 1000);
            return true;
        });
    }

}


void CPUFrequencyControl::on_pushButton_CPUFreqControlCancel_clicked()
{
    renderData();
}

void CPUFrequencyControl::on_pushButton_CPUFreqControlApply_clicked()
{

    forAllCpuPerformanceCores([this](const int i){
        m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMinFreq = dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->getMinFreq() * 1000;
        m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMaxFreq = dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->getMaxFreq() * 1000;
        return true;
    });

    forAllCpuEfficientCores([this](const int i){
        m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMinFreq = dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->getMinFreq() * 1000;
        m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMaxFreq = dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->getMaxFreq() * 1000;
        return true;
    });

    m_dataProvider->setCpuFreqControlData(LenovoLegionDaemon::CPUXFreqControl::getDataControl(m_cpuFreqData));

    renderData();

    emit widgetEvent(LenovoLegionGui::WidgetMessage(LenovoLegionGui::WidgetMessage::Widget::CPU_FREQUENCY_CONTROL,LenovoLegionGui::WidgetMessage::Message::CPU_FREQ_CONTROL_APPLY));
}

void CPUFrequencyControl::on_pushButton_CPUFreqControlMin2500_clicked()
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum() + 1);
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum());
    ui->horizontalSlider_CPUFreqControlMax->setValue(2500 + 1);
    ui->horizontalSlider_CPUFreqControlMax->setValue(2500);
}

void CPUFrequencyControl::on_pushButton_CPUFreqControl25002500_clicked()
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(2500 + 1);
    ui->horizontalSlider_CPUFreqControlMin->setValue(2500);
    ui->horizontalSlider_CPUFreqControlMax->setValue(2500 + 1);
    ui->horizontalSlider_CPUFreqControlMax->setValue(2500);
}

void CPUFrequencyControl::on_pushButton_CPUFreqControlMin3000_clicked()
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum() + 1);
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum());
    ui->horizontalSlider_CPUFreqControlMax->setValue(3000 + 1);
    ui->horizontalSlider_CPUFreqControlMax->setValue(3000);
}

void CPUFrequencyControl::on_pushButton_CPUFreqControl30003000_clicked()
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(3000 + 1);
    ui->horizontalSlider_CPUFreqControlMin->setValue(3000);
    ui->horizontalSlider_CPUFreqControlMax->setValue(3000 + 1);
    ui->horizontalSlider_CPUFreqControlMax->setValue(3000);
}

void CPUFrequencyControl::on_pushButton_CPUFreqControlMin3500_clicked()
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum() + 1);
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum());
    ui->horizontalSlider_CPUFreqControlMax->setValue(3500 + 1);
    ui->horizontalSlider_CPUFreqControlMax->setValue(3500);
}

void CPUFrequencyControl::on_pushButton_CPUFreqControl35003500_clicked()
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(3500 + 1);
    ui->horizontalSlider_CPUFreqControlMin->setValue(3500);
    ui->horizontalSlider_CPUFreqControlMax->setValue(3500 + 1);
    ui->horizontalSlider_CPUFreqControlMax->setValue(3500);
}

void CPUFrequencyControl::on_pushButton_CPUFreqControlMin4000_clicked()
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum() + 1);
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum());
    ui->horizontalSlider_CPUFreqControlMax->setValue(4000 + 1);
    ui->horizontalSlider_CPUFreqControlMax->setValue(4000);
}

void CPUFrequencyControl::on_pushButton_CPUFreqControl40004000_clicked()
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(4000 + 1);
    ui->horizontalSlider_CPUFreqControlMin->setValue(4000);
    ui->horizontalSlider_CPUFreqControlMax->setValue(4000 + 1);
    ui->horizontalSlider_CPUFreqControlMax->setValue(4000);
}


void CPUFrequencyControl::on_pushButton_CPUFreqControlDefault_clicked()
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum() + 1);
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum());
    ui->horizontalSlider_CPUFreqControlMax->setValue(ui->horizontalSlider_CPUFreqControlMax->maximum() - 1);
    ui->horizontalSlider_CPUFreqControlMax->setValue(ui->horizontalSlider_CPUFreqControlMax->maximum());
}
void CPUFrequencyControl::forAllCpuPerformanceCores(const std::function<bool (const int)> &func)
{
    LenovoLegionDaemon::CPUTopology::forAllCpuRange(func,m_cpuHetTopologyData.m_isAvailable ? m_cpuHetTopologyData.m_data.m_ActiveCpusCore : m_cpuHomTopologyData.m_data.m_ActiveCpus);
}

void CPUFrequencyControl::forAllCpuEfficientCores(const std::function<bool (const int)> &func)
{
    if(m_cpuHetTopologyData.m_isAvailable)
    {
        LenovoLegionDaemon::CPUTopology::forAllCpuRange(func,m_cpuHetTopologyData.m_data.m_ActiveCpusAtom);
    }
}

void CPUFrequencyControl::forAllCPUThreads(const std::function<bool (const LenovoLegionDaemon::CPUXFreqControl::DataInfo::CPUX&,const size_t)> &func)
{
    LenovoLegionDaemon::CPUXFreqControl::forAllCpu(func,m_cpuFreqData.m_data.m_cpus);
}

void CPUFrequencyControl::renderData()
{
    if(!m_cpuFreqData.m_isAvailable         ||
       !m_cpuHetTopologyData.m_isAvailable  ||
       !m_cpuHomTopologyData.m_isAvailable
      )
    {
        emit widgetEvent( LenovoLegionGui::WidgetMessage {
            .m_widget  = LenovoLegionGui::WidgetMessage::Widget::CPU_FREQUENCY_CONTROL,
            .m_message = LenovoLegionGui::WidgetMessage::Message::CPU_FREQ_CONTROL_NOT_AVAILABLE
        });

        return;
    }

    /*
     * Render CPU Frequency Performance
     */
    forAllCpuPerformanceCores([this](const int i){
        dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i%8,i/8)->widget())->setMinFreq(m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMinFreq / 1000,m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMinFreq / 1000);
        dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i%8,i/8)->widget())->setMaxFreq(m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMaxFreq / 1000,m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMaxFreq / 1000);
        return true;
    });

    /*
     * Render CPU Frequency Efficiency
     */
    forAllCpuEfficientCores([this](const int i){
        dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i%8,i/8)->widget())->setMinFreq(m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMinFreq / 1000,m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMinFreq / 1000);
        dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i%8,i/8)->widget())->setMaxFreq(m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMaxFreq / 1000,m_cpuFreqData.m_data.m_cpus.at(i).m_cpuScalingMaxFreq / 1000);
        return true;
    });


    ui->horizontalSlider_CPUFreqControlMin->blockSignals(true);
    ui->horizontalSlider_CPUFreqControlMax->blockSignals(true);
    forAllCPUThreads([this](const LenovoLegionDaemon::CPUXFreqControl::DataInfo::CPUX& cpuFreq, const size_t)
    {
        ui->horizontalSlider_CPUFreqControlMin->setValue(cpuFreq.m_cpuScalingMinFreq / 1000);
        ui->horizontalSlider_CPUFreqControlMax->setValue(cpuFreq.m_cpuScalingMaxFreq / 1000);
        return false;
    });

    ui->horizontalSlider_CPUFreqControlMin->blockSignals(false);
    ui->horizontalSlider_CPUFreqControlMax->blockSignals(false);
}


CPUFrequencyControl::~CPUFrequencyControl()
{
    delete ui;
}

void CPUFrequencyControl::refresh()
{
    renderData();
}

void CPUFrequencyControl::on_pushButton_CPUFreqControlMin4500_clicked()
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum() + 1);
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum());
    ui->horizontalSlider_CPUFreqControlMax->setValue(4500 + 1);
    ui->horizontalSlider_CPUFreqControlMax->setValue(4500);
}

void CPUFrequencyControl::on_pushButton_CPUFreqControl45004500_clicked()
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(4500 + 1);
    ui->horizontalSlider_CPUFreqControlMin->setValue(4500);
    ui->horizontalSlider_CPUFreqControlMax->setValue(4500 + 1);
    ui->horizontalSlider_CPUFreqControlMax->setValue(4500);
}

void CPUFrequencyControl::on_pushButton_CPUFreqControlMin5000_clicked()
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum() + 1);
    ui->horizontalSlider_CPUFreqControlMin->setValue(ui->horizontalSlider_CPUFreqControlMin->minimum());
    ui->horizontalSlider_CPUFreqControlMax->setValue(5000 + 1);
    ui->horizontalSlider_CPUFreqControlMax->setValue(5000);
}

void CPUFrequencyControl::on_pushButton_CPUFreqControl50005000_clicked()
{
    ui->horizontalSlider_CPUFreqControlMin->setValue(5000 + 1);
    ui->horizontalSlider_CPUFreqControlMin->setValue(5000);
    ui->horizontalSlider_CPUFreqControlMax->setValue(5000 + 1);
    ui->horizontalSlider_CPUFreqControlMax->setValue(5000);
}

void CPUFrequencyControl::on_comboBoxApplyTo_currentTextChanged(const QString &arg1)
{
    if(arg1 == APPLY_TO_ALL)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(true);
            return true;
        });

        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(true);
            return true;
        });
    }
    else if(arg1 == APPLY_TO_ALL_PERFORMANCE)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(true);
            return true;
        });

        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(false);
            return true;
        });
    }
    else if(arg1 == APPLY_TO_ALL_EFFICIENT)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(false);
            return true;
        });

        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadFrequencyControl*>(ui->gridLayout_CPUFreqControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(true);
            return true;
        });
    }
}







}
