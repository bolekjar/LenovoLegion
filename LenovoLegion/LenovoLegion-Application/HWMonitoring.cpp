// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "HWMonitoring.h"
#include "GPUDetails.h"
#include "CPUDetails.h"
#include "ui_HWMonitoring.h"
#include "Utils.h"
#include "DataProvider.h"


#include "MainWindow.h"

#include "../LenovoLegion-Daemon/SysFsDataProviderHWMon.h"
#include "../LenovoLegion-Daemon/SysFsDataProviderCPUTopology.h"
#include "../LenovoLegion-Daemon/DataProviderNvidiaNvml.h"
#include "../LenovoLegion-Daemon/SysFsDataProviderCPUInfo.h"


#include "CPUFrequency.h"
#include "ThreadFrequency.h"


namespace LenovoLegionGui {

HWMonitoring::HWMonitoring(DataProvider *dataProvider,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HWMonitoring)
    , m_dataProvider(dataProvider)
    , m_windowFreqInfoByCore(new CPUFrequency(nullptr))
    , m_windowGPUDetails(new GPUDetails(nullptr))
    , m_windowCPUDetails(new CPUDetails(nullptr))
{
    ui->setupUi(this);

    m_hwMonitoringData = m_dataProvider->getDataMessage<legion::messages::HardwareMonitor>(LenovoLegionDaemon::SysFsDataProviderHWMon::dataType);
    m_cpuTopology      = m_dataProvider->getDataMessage<legion::messages::CPUTopology>(LenovoLegionDaemon::SysFsDataProviderCPUTopology::dataType);
    m_nvidiaNvmlData   = m_dataProvider->getDataMessage<legion::messages::NvidiaNvml>(LenovoLegionDaemon::DataProviderNvidiaNvml::dataType);
    m_cpuInfoData      = m_dataProvider->getDataMessage<legion::messages::CPUInfo>(LenovoLegionDaemon::SysFsDataProviderCPUInfo::dataType);

    /*
     * CPU Freq Info Performance GUI elements
     */
    forAllCpuPerformanceCores([this](const int i){
        m_windowFreqInfoByCore->addPerformanceWidget(i,new ThreadFrequency(QString("P-Core: ") + QString::number(i) ,m_hwMonitoringData.cpux_freq().at(i).cpu_info_min_freq()/1000,m_hwMonitoringData.cpux_freq().at(i).cpu_info_max_freq()/1000,this));
        return true;
    });

    /*
     * CPU Freq Info Efficiency GUI elements
     */
    forAllCpuEfficientCores([this](const int i){
        m_windowFreqInfoByCore->addEfficiencyWidget(i,new ThreadFrequency(QString("E-Core: ") + QString::number(i),m_hwMonitoringData.cpux_freq().at(i).cpu_info_min_freq()/1000,m_hwMonitoringData.cpux_freq().at(i).cpu_info_max_freq()/1000,this));
        return true;
    });

    m_windowGPUDetails->init(m_nvidiaNvmlData);

    ui->groupBox_CPU->setTitle(QString("CPU (%1)").arg(m_cpuInfoData.cpu_model()));
    ui->widget_CPUTemp->init("Temp [%1 °C]",30,100,30,100);
    ui->widget_CPUPower->init("Power [%1 W]",0,250,0,250);

    quint32 pMin = std::numeric_limits<quint32>::max();
    quint32 pMax = std::numeric_limits<quint32>::min();
    quint32 pScaleMin = std::numeric_limits<quint32>::max();
    quint32 pScaleMax = std::numeric_limits<quint32>::min();
    forAllCpuPerformanceCores([&pMin,&pMax,&pScaleMin,&pScaleMax,this](const int i){
        pMin = std::min(m_hwMonitoringData.cpux_freq().at(i).cpu_info_min_freq()/1000,pMin);
        pMax = std::max(m_hwMonitoringData.cpux_freq().at(i).cpu_info_max_freq()/1000,pMax);
        pScaleMin = std::min(m_hwMonitoringData.cpux_freq().at(i).cpu_scaling_min_freq()/1000,pScaleMin);
        pScaleMax = std::max(m_hwMonitoringData.cpux_freq().at(i).cpu_scaling_max_freq()/1000,pScaleMax);
        return true;
    });
    ui->widget_PCoresAvgFreq->init("P-Core [%1 MHz]",pMin,pMax,pScaleMin,pScaleMax);

    quint32 eMin = std::numeric_limits<quint32>::max();
    quint32 eMax = std::numeric_limits<quint32>::min();
    quint32 eScaleMin = std::numeric_limits<quint32>::max();
    quint32 eScaleMax = std::numeric_limits<quint32>::min();
    forAllCpuEfficientCores([&eMin,&eMax,&eScaleMin,&eScaleMax,this](const int i){
        eMin = std::min(m_hwMonitoringData.cpux_freq().at(i).cpu_info_min_freq()/1000,eMin);
        eMax = std::max(m_hwMonitoringData.cpux_freq().at(i).cpu_info_max_freq()/1000,eMax);
        eScaleMin = std::min(m_hwMonitoringData.cpux_freq().at(i).cpu_scaling_min_freq()/1000,eScaleMin);
        eScaleMax = std::max(m_hwMonitoringData.cpux_freq().at(i).cpu_scaling_max_freq()/1000,eScaleMax);
        return true;
    });

    if(eMin == std::numeric_limits<quint32>::max()||
       eMax == std::numeric_limits<quint32>::min())
    {
        ui->widget_ECoresAvgFreq->init("E-Core [%1 MHz]",0,0,0,0);
        ui->widget_CPUAvgFreq->init("Avg [%1 MHz]",pMin,pMax,pScaleMin,pScaleMax);
    }
    else
    {
        ui->widget_ECoresAvgFreq->init("E-Core [%1 MHz]",eMin,eMax,eScaleMin,eScaleMax);
        ui->widget_CPUAvgFreq->init("Avg [%1 MHz]",std::min(pMin,eMin),std::max(pMax,eMax),std::min(pScaleMin,eScaleMin),std::max(pScaleMax,eScaleMax));
    }

    m_windowCPUDetails->init(m_cpuInfoData,m_cpuTopology,std::min(pScaleMin,eScaleMin),std::max(pScaleMax,eScaleMax));

    ui->groupBox_GPU->setTitle(QString("GPU (%1)").arg(m_nvidiaNvmlData.name()));
    ui->widget_GPUTemp->init("Temp [%1 °C]",30,100,30,100);
    ui->widget_GPUClock->init("Power [%1 W]",0,m_nvidiaNvmlData.hardware_monitor().power().max_value() / 1000,0,m_nvidiaNvmlData.hardware_monitor().power().max_value() / 1000);

    ui->widget_CPUFanSpeed->init("CPU [%1 RPM]",m_hwMonitoringData.legion().fan1_speed_min(),m_hwMonitoringData.legion().fan1_speed_max() + 100,m_hwMonitoringData.legion().fan1_speed_min(),m_hwMonitoringData.legion().fan1_speed_max() + 100);
    ui->widget_GPUFanSpeed->init("GPU [%1 RPM]",m_hwMonitoringData.legion().fan2_speed_min(),m_hwMonitoringData.legion().fan2_speed_max() + 100,m_hwMonitoringData.legion().fan2_speed_min(),m_hwMonitoringData.legion().fan2_speed_max() + 100);

    connect(m_windowFreqInfoByCore,&CPUFrequency::closed,this,&HWMonitoring::freqInfoByCoreClosed);
    connect(m_windowGPUDetails,&GPUDetails::closed,this,&HWMonitoring::gpuDetailsClosed);
    connect(m_windowCPUDetails,&CPUDetails::closed,this,&HWMonitoring::cpuDetailsClosed);
}

void HWMonitoring::refresh()
{
    const legion::messages::HardwareMonitor data        = m_dataProvider->getDataMessage<legion::messages::HardwareMonitor>(LenovoLegionDaemon::SysFsDataProviderHWMon::dataType);
    const legion::messages::NvidiaNvml      nvidiaData  = m_dataProvider->getDataMessage<legion::messages::NvidiaNvml>(LenovoLegionDaemon::DataProviderNvidiaNvml::dataType);


    /*
     * HW monitoring
     */
    ui->widget_CPUTemp->refresh(data.legion().temp1_temp() / 1000,0,10,' ',QString("CPU Temperature: %1 °C\n"\
                                                                                   "CPU Temperature min: 30 °C\n"\
                                                                                   "CPU Temperature max: 100 °C\n").arg(data.legion().temp1_temp() / 1000));

    {

        quint64 diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_lastRefreshTime).count();

        if(diff < (MainWindow::TIMER_EVENT_IN_MS * 1200) && diff > (MainWindow::TIMER_EVENT_IN_MS * 800))
        {
            ui->widget_CPUPower->refresh((data.intel_power().power_cap_cpu_energy() - m_hwMonitoringData.intel_power().power_cap_cpu_energy())/diff ,
                                         0,10,' ',
                                         QString("CPU Power : %1 W\n"
                                                 "CPU Power min: 0 W\n"
                                                 "CPU Power max: 250 W\n").arg((data.intel_power().power_cap_cpu_energy() - m_hwMonitoringData.intel_power().power_cap_cpu_energy())/diff));
        }
        else
        {
            ui->widget_CPUPower->refresh(ui->widget_CPUPower->getValue() ,
                                         0,10,' ',
                                         QString("CPU Power : %1 W\n"
                                                 "CPU Power min: 0 W\n"
                                                 "CPU Power max: 250 W\n").arg(ui->widget_CPUPower->getValue()));
        }
    }

    ui->widget_GPUTemp->refresh(data.legion().temp2_temp() / 1000,0,10,' ',QString("GPU Temperature: %1 °C\n"\
                                                                                       "GPU Temperature min: 30 °C\n"\
                                                                                       "GPU Temperature max: 100 °C\n").arg(data.legion().temp2_temp() / 1000));

    ui->widget_GPUClock->refresh(nvidiaData.hardware_monitor().power().value() / 1000,0,10,' ',QString("Power: %1 W\n"\
                                                                                                     "Power min: %2 W\n"\
                                                                                                     "Power max: %3 W\n").arg(nvidiaData.hardware_monitor().power().value() / 1000)
                                                                                                                         .arg(nvidiaData.hardware_monitor().power().min_value() / 1000)
                                                                                                                         .arg(m_nvidiaNvmlData.hardware_monitor().power().max_value() / 1000));

    ui->widget_CPUFanSpeed->refresh(data.legion().fan1_speed(),0,10,' ',QString("CPU FAN Speed: %1 RPM\n"\
                                                                                      "CPU FAN Speed min: 0 RPM\n"\
                                                                                      "CPU FAN Speed max: %2 RPM\n").arg(data.legion().fan1_speed()).arg((data.legion().fan1_speed_max())));
    ui->widget_GPUFanSpeed->refresh(data.legion().fan2_speed(),0,10,' ',QString("GPU FAN Speed: %1 RPM\n"\
                                                                                      "GPU FAN Speed min: 0 RPM\n"\
                                                                                      "GPU FAN Speed max: %2 RPM\n").arg(data.legion().fan2_speed()).arg(data.legion().fan2_speed_max()));



    /*
     * CPU Frequency stats
     */
    struct Stats {
        int averageFreq             = 0;
        int avareqePCoreFreq        = 0;
        int avareqeACoreFreq        = 0;
        int averageFreqCount        = 0;
        int avareqePCoreFreqCount   = 0;
        int avareqeACoreFreqCount   = 0;
    } l_stats;

    if(m_windowFreqInfoByCore->isVisible())
    {
        /*
     * Render CPU Frequency Performance
     */
        forAllCpuPerformanceCores([this,&data,&l_stats](const int i)
                                  {
                                      dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getPerforamnceWidget(i))->setBaseFreq(data.cpux_freq().at(i).cpu_base_freq() / 1000);
                                      dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getPerforamnceWidget(i))->setMinFreq(data.cpux_freq().at(i).cpu_info_min_freq() / 1000);
                                      dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getPerforamnceWidget(i))->setMaxFreq(data.cpux_freq().at(i).cpu_info_max_freq() / 1000);

                                      dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getPerforamnceWidget(i))->setScalingMinFreq(data.cpux_freq().at(i).cpu_scaling_min_freq() / 1000);
                                      dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getPerforamnceWidget(i))->setScalingMaxFreq(data.cpux_freq().at(i).cpu_scaling_max_freq() / 1000);
                                      dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getPerforamnceWidget(i))->setScalingCurFreq(data.cpux_freq().at(i).cpu_scaling_cur_freq() / 1000);


                                      l_stats.avareqePCoreFreq += data.cpux_freq().at(i).cpu_scaling_cur_freq();++l_stats.avareqePCoreFreqCount;
                                      l_stats.averageFreq      += data.cpux_freq().at(i).cpu_scaling_cur_freq();++l_stats.averageFreqCount;
                                      return true;
                                  });


        /*
     * Render CPU Frequency Efficiency
     */
        forAllCpuEfficientCores([this,&data,&l_stats](const int i)
                                {
                                    dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getEfficiencyWidget(i))->setBaseFreq(data.cpux_freq().at(i).cpu_base_freq() / 1000);
                                    dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getEfficiencyWidget(i))->setMinFreq(data.cpux_freq().at(i).cpu_info_min_freq() / 1000);
                                    dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getEfficiencyWidget(i))->setMaxFreq(data.cpux_freq().at(i).cpu_info_max_freq() / 1000);

                                    dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getEfficiencyWidget(i))->setScalingMinFreq(data.cpux_freq().at(i).cpu_scaling_min_freq() / 1000);
                                    dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getEfficiencyWidget(i))->setScalingMaxFreq(data.cpux_freq().at(i).cpu_scaling_max_freq() / 1000);
                                    dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getEfficiencyWidget(i))->setScalingCurFreq(data.cpux_freq().at(i).cpu_scaling_cur_freq() / 1000);

                                    l_stats.avareqeACoreFreq += data.cpux_freq().at(i).cpu_scaling_cur_freq();++l_stats.avareqeACoreFreqCount;
                                    l_stats.averageFreq      += data.cpux_freq().at(i).cpu_scaling_cur_freq();++l_stats.averageFreqCount;
                                    return true;
                                });
    }
    else
    {
        forAllCpuPerformanceCores([&data,&l_stats](const int i)
                                  {
                                      l_stats.avareqePCoreFreq += data.cpux_freq().at(i).cpu_scaling_cur_freq();++l_stats.avareqePCoreFreqCount;
                                      l_stats.averageFreq      += data.cpux_freq().at(i).cpu_scaling_cur_freq();++l_stats.averageFreqCount;
                                      return true;
                                  });
        forAllCpuEfficientCores([&data,&l_stats](const int i)
                                {
                                    l_stats.avareqeACoreFreq += data.cpux_freq().at(i).cpu_scaling_cur_freq();++l_stats.avareqeACoreFreqCount;
                                    l_stats.averageFreq      += data.cpux_freq().at(i).cpu_scaling_cur_freq();++l_stats.averageFreqCount;
                                    return true;
                                });
    }

    if(m_windowGPUDetails->isVisible())
    {
        /*
         * GPU Details
         */
        m_windowGPUDetails->refresh(nvidiaData);
    }

    /*
     * Render CPU Frequency Avg
     */
    ui->widget_CPUAvgFreq->refresh(l_stats.averageFreqCount == 0 ? 0 : (l_stats.averageFreq/l_stats.averageFreqCount/1000),4,10,QChar('0'),
                                      QString("CPU Average Frequency: %1 MHz\n"\
                                              "CPU Min Frequency: %2 MHz\n"\
                                              "CPU Max Frequency: %3 MHz\n"\
                                              "CPU Scale Min Frequency: %4 MHz\n"\
                                              "CPU Scale Max Frequency: %5 MHz\n").arg(l_stats.averageFreqCount == 0 ? 0 : (l_stats.averageFreq/l_stats.averageFreqCount/1000)).arg(ui->widget_CPUAvgFreq->getMinValue()).arg(ui->widget_CPUAvgFreq->getMaxValue()).arg(ui->widget_CPUAvgFreq->getScaleMin()).arg(ui->widget_CPUAvgFreq->getScaleMax()));
    ui->widget_PCoresAvgFreq->refresh(l_stats.avareqePCoreFreqCount == 0 ? 0 : (l_stats.avareqePCoreFreq/l_stats.avareqePCoreFreqCount/1000),4,10,QChar('0'),
                                      QString("P-Core Average Frequency: %1 MHz\n"\
                                              "P-Core Min Frequency: %2 MHz\n"\
                                              "P-Core Max Frequency: %3 MHz\n"
                                              "P-Core Scale Min Frequency: %4 MHz\n"
                                              "P-Core Scale Maxn Frequency: %5 MHz\n").arg(l_stats.avareqePCoreFreqCount == 0 ? 0 : (l_stats.avareqePCoreFreq/l_stats.avareqePCoreFreqCount/1000)).arg(ui->widget_PCoresAvgFreq->getMinValue()).arg(ui->widget_PCoresAvgFreq->getMaxValue()).arg((ui->widget_PCoresAvgFreq->getScaleMin())).arg(ui->widget_PCoresAvgFreq->getScaleMax()));
    ui->widget_ECoresAvgFreq->refresh(l_stats.avareqeACoreFreqCount == 0 ? 0 : (l_stats.avareqeACoreFreq/l_stats.avareqeACoreFreqCount/1000),4,10,QChar('0'),
                                      QString("E-Core Average Frequency: %1 MHz\n"\
                                              "E-Core Min Frequency: %2 MHz\n"\
                                              "E-Core Max Frequency: %3 MHz\n"
                                              "E-Core Scale Min Frequency: %4 MHz\n"
                                              "E-Core Scale Max Frequency: %5 MHz\n").arg(l_stats.avareqeACoreFreqCount == 0 ? 0 : (l_stats.avareqeACoreFreq/l_stats.avareqeACoreFreqCount/1000)).arg(ui->widget_ECoresAvgFreq->getMinValue()).arg(ui->widget_ECoresAvgFreq->getMaxValue()).arg(ui->widget_ECoresAvgFreq->getScaleMin()).arg(ui->widget_ECoresAvgFreq->getScaleMax())
                                        );

    m_hwMonitoringData = m_dataProvider->getDataMessage<legion::messages::HardwareMonitor>(LenovoLegionDaemon::SysFsDataProviderHWMon::dataType);
    m_nvidiaNvmlData = m_dataProvider->getDataMessage<legion::messages::NvidiaNvml>(LenovoLegionDaemon::DataProviderNvidiaNvml::dataType);
    m_lastRefreshTime  = std::chrono::steady_clock::now();
}

HWMonitoring::~HWMonitoring()
{
    delete m_windowFreqInfoByCore;
    delete m_windowGPUDetails;
    delete m_windowCPUDetails;
    delete ui;    
}

void HWMonitoring::forAllCpuPerformanceCores(const std::function<bool (const int)> &func)
{
    Utils::ProtoBuf::forAllCpuTopologyRange(func,m_cpuTopology.active_cpus_core());
}

void HWMonitoring::forAllCpuEfficientCores(const std::function<bool (const int)> &func)
{
    Utils::ProtoBuf::forAllCpuTopologyRange(func,m_cpuTopology.active_cpus_atom());
}

void HWMonitoring::on_groupBox_17_clicked(bool checked)
{
    if(m_windowFreqInfoByCore->isHidden() && checked)
    {
        m_windowFreqInfoByCore->show();
    }
    else
    {
        m_windowFreqInfoByCore->hide();
    }
}

void HWMonitoring::freqInfoByCoreClosed()
{
    ui->groupBox_17->setChecked(false);
}

void HWMonitoring::gpuDetailsClosed()
{
    ui->groupBox_GPU->setChecked(false);
}

void HWMonitoring::cpuDetailsClosed()
{
    ui->groupBox_CPU->setChecked(false);
}

void HWMonitoring::on_groupBox_GPU_clicked(bool checked)
{
    if(m_windowGPUDetails->isHidden() && checked)
    {
        m_windowGPUDetails->show();
    }
    else
    {
        m_windowGPUDetails->hide();
    }
}

void HWMonitoring::on_groupBox_CPU_clicked(bool checked)
{
    if(m_windowCPUDetails->isHidden() && checked)
    {
        m_windowCPUDetails->show();
    }
    else
    {
        m_windowCPUDetails->hide();
    }
}





}
