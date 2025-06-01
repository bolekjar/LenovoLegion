// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "CPUControl.h"
#include "ui_CPUControl.h"

#include "CPUControlDataProvider.h"
#include "ThreadControl.h"


namespace LenovoLegionGui {


QString const CPUControl::NAME      =   "CPU Control";

const QMap<QString,LenovoLegionDaemon::CPUXControl::DataControl::CPUX>CPUControl::CPU_CONTROL_PRESETS = {
    {CPUControl::PERFORMANCE.data(),{.m_Governor = "performance",.m_cpuOnline = true}},
    {CPUControl::POWER_SAVE.data(), {.m_Governor = "powersave"  ,.m_cpuOnline = true}},
    {CPUControl::ONDEMAND.data(),   {.m_Governor = "ondemand"   ,.m_cpuOnline = true}},
    {CPUControl::OFF.data(),        {.m_Governor = "powersave"  ,.m_cpuOnline = false}}
};


const QString CPUControl::APPLY_TO_ALL              = "ALL";
const QString CPUControl::APPLY_TO_ALL_ENABLED      = "Enabled";
const QString CPUControl::APPLY_TO_ALL_EFFICIENT    = "Efficiency";
const QString CPUControl::APPLY_TO_ALL_PERFORMANCE  = "Performance";
const QString CPUControl::APPLY_TO_ALL_DISABLED     = "Disabled";

const LenovoLegionDaemon::CPUSMTControl::DataControl CPUControl::SMT_ON_DATA = {
    .m_data = {
        .m_control = "on"
    }
};

const LenovoLegionDaemon::CPUSMTControl::DataControl CPUControl::SMT_OFF_DATA = {
    .m_data = {
        .m_control = "off"
    }
};

LenovoLegionDaemon::CPUXControl::DataControl::CPUX CPUControl::getCpuControlPreset(const QString &presetName,const LenovoLegionDaemon::CPUXControl::DataInfo::CPUX &dataInfo)
{
    auto availableGovernors = QString(dataInfo.m_availableGovernors).trimmed().split(' ');

    if(CPU_CONTROL_PRESETS.contains(presetName))
    {
        LenovoLegionDaemon::CPUXControl::DataControl::CPUX cpuControl = CPU_CONTROL_PRESETS.value(presetName);

        if(availableGovernors.contains(cpuControl.m_Governor))
        {
            return cpuControl;
        }
        else
        {
            LenovoLegionDaemon::CPUXControl::DataControl::CPUX cpuX = LenovoLegionDaemon::CPUXControl::getDataControl(dataInfo);
            cpuX.m_cpuOnline = cpuControl.m_cpuOnline;
            return cpuX;
        }
    }

    return LenovoLegionDaemon::CPUXControl::getDataControl(dataInfo);
}

CPUControl::CPUControl(CPUControlDataProvider *dataProvider, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CPUControl)
    , m_dataProvider(dataProvider)
{
    ui->setupUi(this);

    m_cpuInfoData = m_dataProvider->getCPUXInfoControlData();
    m_cpuHetTopology = m_dataProvider->getCPUHeterogenousTopologyData();
    m_cpuHomTopology = m_dataProvider->getCPUHomogeneousTopologyData();
    m_cpuSMTControlData = m_dataProvider->getCPUSMTControlData();

    /*
     * CPU Control Performance GUI elements
     */
    class
    {
    public:
        const QString& getColor(quint32 coreId)
        {
            if(coreId != m_coreId)
            {
                m_curentColor++;

                if(m_curentColor == m_cpuThreadsColors.end())
                {
                    m_curentColor = m_cpuThreadsColors.begin();
                }

                m_coreId = coreId;
            }

            return *m_curentColor;
        }
    private:

        std::list<QString>                 m_cpuThreadsColors  = { "white", "black"};
        std::list<QString>::const_iterator m_curentColor       = m_cpuThreadsColors.begin();

        quint32 m_coreId = 0;
    } l_ColorGenerator;



    /*
     * CPU Control Performance GUI elements
     */
    forAllCpuPerformanceCores([this,&l_ColorGenerator](const int i){
        ui->gridLayout_CPUControl->addWidget(new ThreadControl(QString("P-Thread: %1, CoreId: %2, DieId: %3").
                                                               arg(i).
                                                               arg(m_cpuInfoData.m_data.m_cpus.at(i).m_cpuCoreId).
                                                               arg(m_cpuInfoData.m_data.m_cpus.at(i).m_dieId),i,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline,m_cpuInfoData.m_data.m_cpus.at(i).m_Governor,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuCoreId,l_ColorGenerator.getColor(m_cpuInfoData.m_data.m_cpus.at(i).m_cpuCoreId),"#919191",this),i % 8,i/8);
        return true;
    });

    /*
     * CPU Control Efficiency GUI elements
     */
    forAllCpuEfficientCores([this,&l_ColorGenerator](const int i){
        ui->gridLayout_CPUControl->addWidget(new ThreadControl(QString("E-Thread: %1, CoreId: %2, DieId: %3").
                                                               arg(i).
                                                               arg(m_cpuInfoData.m_data.m_cpus.at(i).m_cpuCoreId).
                                                               arg(m_cpuInfoData.m_data.m_cpus.at(i).m_dieId),i,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline,m_cpuInfoData.m_data.m_cpus.at(i).m_Governor,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuCoreId,l_ColorGenerator.getColor(m_cpuInfoData.m_data.m_cpus.at(i).m_cpuCoreId),"#ababab",this),i % 8,i/8);
        return true;
    });

    /*
     * CPU Disabled Control GUI elements
     */
    forAllCpuDisabledCores([this,&l_ColorGenerator](const int i){
        ui->gridLayout_CPUControl->addWidget(new ThreadControl(QString("D-Thread: %1, CoreId: %2, DieId: %3").
                                                               arg(i).
                                                               arg(m_cpuInfoData.m_data.m_cpus.at(i).m_cpuCoreId).
                                                               arg(m_cpuInfoData.m_data.m_cpus.at(i).m_dieId),i,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline,m_cpuInfoData.m_data.m_cpus.at(i).m_Governor,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuCoreId,l_ColorGenerator.getColor(m_cpuInfoData.m_data.m_cpus.at(i).m_cpuCoreId),"#b0b0b0",this),i % 8,i/8);
        return true;
    });

    if(m_cpuInfoData.m_data.m_cpus.size() > 0)
    {
        ui->comboBox_CPUGovernor->addItems(QString(m_cpuInfoData.m_data.m_cpus.at(0).m_availableGovernors).trimmed().split(' '));
        ui->comboBox_CPUGovernor->setCurrentText(QString(m_cpuInfoData.m_data.m_cpus.at(0).m_Governor).trimmed());
    }

    renderData();

    ui->comboBoxApplyTo->addItem(APPLY_TO_ALL);
    ui->comboBoxApplyTo->addItem(APPLY_TO_ALL_ENABLED);
    ui->comboBoxApplyTo->addItem(APPLY_TO_ALL_DISABLED);

    if(m_cpuHetTopology.m_isAvailable)
    {
        ui->comboBoxApplyTo->addItem(APPLY_TO_ALL_PERFORMANCE);
        ui->comboBoxApplyTo->addItem(APPLY_TO_ALL_EFFICIENT);
    }

}

CPUControl::~CPUControl()
{
    delete ui;
}

void CPUControl::refresh()
{
    renderData();
}

void CPUControl::on_pushButton_CPUControlEnableAll_clicked()
{

    if( ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(true,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });

        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(true,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });

        forAllCpuDisabledCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(true,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });
    }
    else if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_ENABLED)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(true,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });

        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(true,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });
    }
    else if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_DISABLED)
    {
        forAllCpuDisabledCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(true,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });
    }
    else if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_PERFORMANCE)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(true,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });
    }
    else if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_EFFICIENT)
    {
        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(true,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });
    }
}

void CPUControl::on_pushButton_CPUControlDisableAll_clicked()
{
    if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(false,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });

        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(false,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });

        forAllCpuDisabledCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(false,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });
    }
    else if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_ENABLED)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(false,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });

        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(false,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });
    }
    else if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_DISABLED)
    {
        forAllCpuDisabledCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(false,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });
    }
    else if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_PERFORMANCE)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(false,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });
    }
    else if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_EFFICIENT)
    {
        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(false,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
            return true;
        });
    }
}

void CPUControl::on_comboBox_CPUGovernor_currentTextChanged(const QString &arg1)
{
    if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL)
    {
        forAllCpuPerformanceCores([this,arg1](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setScalingGovernor(arg1,QString(m_cpuInfoData.m_data.m_cpus.at(i).m_Governor).trimmed());
            return true;
        });

        forAllCpuEfficientCores([this,arg1](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setScalingGovernor(arg1,QString(m_cpuInfoData.m_data.m_cpus.at(i).m_Governor).trimmed());
            return true;
        });
    }
    else if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_ENABLED)
    {
        forAllCpuPerformanceCores([this,arg1](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setScalingGovernor(arg1,QString(m_cpuInfoData.m_data.m_cpus.at(i).m_Governor).trimmed());
            return true;
        });

        forAllCpuEfficientCores([this,arg1](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setScalingGovernor(arg1,QString(m_cpuInfoData.m_data.m_cpus.at(i).m_Governor).trimmed());
            return true;
        });
    }
    else if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_PERFORMANCE)
    {
        forAllCpuPerformanceCores([this,arg1](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setScalingGovernor(arg1,QString(m_cpuInfoData.m_data.m_cpus.at(i).m_Governor).trimmed());
            return true;
        });
    }
    else if(ui->comboBoxApplyTo->currentText() == APPLY_TO_ALL_EFFICIENT)
    {
        forAllCpuEfficientCores([this,arg1](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setScalingGovernor(arg1,QString(m_cpuInfoData.m_data.m_cpus.at(i).m_Governor).trimmed());
            return true;
        });
    }
}

void CPUControl::on_pushButton_CPUControlCancel_clicked()
{
    renderData();
}

void CPUControl::on_pushButton_CPUControlApply_clicked()
{
    LenovoLegionDaemon::CPUXControl::DataControl data;


    /*
     * CPU Control Performance GUI elements
     */
    forAllCpuPerformanceCores([this,&data](const int i){
        data.m_data.m_cpus.at(i).m_cpuOnline = dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->getCPUEnabled();
        std::memset(data.m_data.m_cpus.at(i).m_Governor,0,sizeof(data.m_data.m_cpus.at(i).m_Governor) * sizeof(data.m_data.m_cpus.at(i).m_Governor[0]));
        std::strncpy(data.m_data.m_cpus.at(i).m_Governor,dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->getScalingGovernor().toStdString().c_str(),sizeof(data.m_data.m_cpus.at(i).m_Governor)  * sizeof(data.m_data.m_cpus.at(i).m_Governor[0]) - 1);
        return true;
    });


    /*
     * CPU Control Efficiency GUI elements
     */
    forAllCpuEfficientCores([this,&data](const int i){
        data.m_data.m_cpus.at(i).m_cpuOnline = dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->getCPUEnabled();
        std::memset(data.m_data.m_cpus.at(i).m_Governor,0,sizeof(data.m_data.m_cpus.at(i).m_Governor) * sizeof(data.m_data.m_cpus.at(i).m_Governor[0]));
        std::strncpy(data.m_data.m_cpus.at(i).m_Governor,dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->getScalingGovernor().toStdString().c_str(),sizeof(data.m_data.m_cpus.at(i).m_Governor) *  sizeof(data.m_data.m_cpus.at(i).m_Governor[0])- 1);
        return true;
    });

    /*
     * CPU Disabled Control GUI elements
     */
    forAllCpuDisabledCores([this,&data](const int i){
        data.m_data.m_cpus.at(i).m_cpuOnline = dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->getCPUEnabled();
        return true;
    });

    m_dataProvider->setCpuSControlData(data);

    emit widgetEvent(LenovoLegionGui::WidgetMessage{LenovoLegionGui::WidgetMessage::Widget::CPU_CONTROL,LenovoLegionGui::WidgetMessage::Message::CPU_CONTROL_CHANGED});
}

void CPUControl::forAllCpuPerformanceCores(const std::function<bool (const int)> &func)
{
    LenovoLegionDaemon::CPUTopology::forAllCpuRange(func,m_cpuHetTopology.m_isAvailable ? m_cpuHetTopology.m_data.m_ActiveCpusCore : m_cpuHomTopology.m_data.m_ActiveCpus);
}

void CPUControl::forAllCpuEfficientCores(const std::function<bool (const int)> &func)
{
    if(m_cpuHetTopology.m_isAvailable)
    {
        LenovoLegionDaemon::CPUTopology::forAllCpuRange(func,m_cpuHetTopology.m_data.m_ActiveCpusAtom);
    }
}

void CPUControl::forAllCpuDisabledCores(const std::function<bool (const int)> &func)
{
    for (size_t i = 0;i < m_cpuInfoData.m_data.m_cpus.size();++i) {
        if(!m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline) {
            if(!func(i))
            {
                return;
            };
        }
    }

}

void CPUControl::renderData()
{
    if(!m_cpuInfoData.m_isAvailable     ||
       !m_cpuHetTopology.m_isAvailable  ||
       !m_cpuHomTopology.m_isAvailable  ||
       !m_cpuSMTControlData.m_isAvailable
      )
    {
        emit widgetEvent( LenovoLegionGui::WidgetMessage {
            .m_widget       = LenovoLegionGui::WidgetMessage::Widget::CPU_CONTROL,
            .m_message = LenovoLegionGui::WidgetMessage::Message::CPU_CONTROL_NOT_AVAILABLE
        });

        return;
    }

    /*
     * CPU Control Performance GUI elements
     */
    forAllCpuPerformanceCores([this](const int i){
        dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
        dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setScalingGovernor(QString(m_cpuInfoData.m_data.m_cpus.at(i).m_Governor).trimmed(),QString(m_cpuInfoData.m_data.m_cpus.at(i).m_Governor).trimmed());
        return true;
    });

    /*
     * CPU Control Efficiency GUI elements
     */
    forAllCpuEfficientCores([this](const int i){
        dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
        dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setScalingGovernor(QString(m_cpuInfoData.m_data.m_cpus.at(i).m_Governor).trimmed(),QString(m_cpuInfoData.m_data.m_cpus.at(i).m_Governor).trimmed());
        return true;
    });

    /*
     * CPU Disabled Control GUI elements
     */
    forAllCpuDisabledCores([this](const int i){
        dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setCPUEnabled(m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline,m_cpuInfoData.m_data.m_cpus.at(i).m_cpuOnline);
        dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setScalingGovernor(QString(m_cpuInfoData.m_data.m_cpus.at(i).m_Governor).trimmed(),QString(m_cpuInfoData.m_data.m_cpus.at(i).m_Governor).trimmed());
        return true;
    });

    if(m_cpuInfoData.m_data.m_cpus.size() > 0)
    {
        ui->comboBox_CPUGovernor->setCurrentText(QString(m_cpuInfoData.m_data.m_cpus.at(0).m_Governor).trimmed());
    }

    if(m_cpuSMTControlData.m_isAvailable)
    {
        ui->checkBox_DisableSMT->blockSignals(true);
        if(m_cpuSMTControlData.m_data.m_active)
        {
            if(QString(m_cpuSMTControlData.m_data.m_control).trimmed() == SMT_OFF_DATA.m_data.m_control)
            {
                m_dataProvider->setCpuSMTControlData(SMT_ON_DATA);
            }

            ui->checkBox_DisableSMT->setChecked(false);
        }
        else
        {
            if(QString(m_cpuSMTControlData.m_data.m_control).trimmed() == SMT_ON_DATA.m_data.m_control)
            {
                m_dataProvider->setCpuSMTControlData(SMT_OFF_DATA);
            }

            ui->checkBox_DisableSMT->setChecked(true);
        }
        ui->checkBox_DisableSMT->blockSignals(false);
    }
    else
    {
        ui->checkBox_DisableSMT->setEnabled(false);
    }
}

void CPUControl::on_checkBox_DisableSMT_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::CheckState::Checked)
    {
         m_dataProvider->setCpuSMTControlData(SMT_OFF_DATA);
    }
    else
    {
        m_dataProvider->setCpuSMTControlData(SMT_ON_DATA);
    }


    emit widgetEvent(LenovoLegionGui::WidgetMessage{LenovoLegionGui::WidgetMessage::Widget::CPU_CONTROL,LenovoLegionGui::WidgetMessage::Message::CPU_CONTROL_CHANGED});
}

void CPUControl::on_comboBoxApplyTo_currentTextChanged(const QString &arg1)
{
    if(arg1 == APPLY_TO_ALL)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(true);
            return true;
        });

        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(true);
            return true;
        });

        forAllCpuDisabledCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(true);
            return true;
        });
    }
    else if(arg1 == APPLY_TO_ALL_ENABLED)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(true);
            return true;
        });

        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(true);
            return true;
        });

        forAllCpuDisabledCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(false);
            return true;
        });
    }
    else if(arg1 == APPLY_TO_ALL_DISABLED)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(false);
            return true;
        });

        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(false);
            return true;
        });

        forAllCpuDisabledCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(true);
            return true;
        });
    }
    else if(arg1 == APPLY_TO_ALL_PERFORMANCE)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(true);
            return true;
        });

        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(false);
            return true;
        });

        forAllCpuDisabledCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(false);
            return true;
        });
    }
    else if(arg1 == APPLY_TO_ALL_EFFICIENT)
    {
        forAllCpuPerformanceCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(false);
            return true;
        });

        forAllCpuEfficientCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(true);
            return true;
        });

        forAllCpuDisabledCores([this](const int i){
            dynamic_cast<ThreadControl*>(ui->gridLayout_CPUControl->itemAtPosition(i % 8,i/8)->widget())->setVisible(false);
            return true;
        });
    }
}



}
