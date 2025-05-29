// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "HWMonitoring.h"
#include "ui_HWMonitoring.h"

#include "MainWindow.h"

#include "HWMonitoringDataProvider.h"
#include "CPUFrequency.h"
#include "ThreadFrequency.h"


namespace LenovoLegionGui {

HWMonitoring::HWMonitoring(HWMonitoringDataProvider *dataProvider,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HWMonitoring)
    , m_dataProvider(dataProvider)
    , m_windowFreqInfoByCore(new CPUFrequency(nullptr))
{
    ui->setupUi(this);

    m_hwMonitoringData = m_dataProvider->getHWMonitoringData();
    m_cpuHetTopology   = m_dataProvider->getCPUHeterogenousTopologyData();
    m_cpuHomTopology   = m_dataProvider->getCPUHomogeneousTopologyData();


    /*
     * CPU Freq Info Performance GUI elements
     */
    forAllCpuPerformanceCores([this](const int i){
        m_windowFreqInfoByCore->addPerformanceWidget(i,new ThreadFrequency(QString("P-Core: ") + QString::number(i) ,m_hwMonitoringData.m_cpusFreq.at(i).m_data.m_cpuInfoMinFreq/1000,m_hwMonitoringData.m_cpusFreq.at(i).m_data.m_cpuInfoMaxFreq/1000,this));
        return true;
    });

    /*
     * CPU Freq Info Efficiency GUI elements
     */
    forAllCpuEfficientCores([this](const int i){
        m_windowFreqInfoByCore->addEfficiencyWidget(i,new ThreadFrequency(QString("E-Core: ") + QString::number(i),m_hwMonitoringData.m_cpusFreq.at(i).m_data.m_cpuInfoMinFreq/1000,m_hwMonitoringData.m_cpusFreq.at(i).m_data.m_cpuInfoMaxFreq/1000,this));
        return true;
    });

    ui->widget_CPUTemp->init("Temp [%1 °C]",30,100,30,100);
    ui->widget_CPUPower->init("Power [%1 W]",0,250,0,250);

    quint32 pMin = std::numeric_limits<quint32>::max();
    quint32 pMax = std::numeric_limits<quint32>::min();
    quint32 pScaleMin = std::numeric_limits<quint32>::max();
    quint32 pScaleMax = std::numeric_limits<quint32>::min();
    forAllCpuPerformanceCores([&pMin,&pMax,&pScaleMin,&pScaleMax,this](const int i){
        pMin = std::min(m_hwMonitoringData.m_cpusFreq.at(i).m_data.m_cpuInfoMinFreq/1000,pMin);
        pMax = std::max(m_hwMonitoringData.m_cpusFreq.at(i).m_data.m_cpuInfoMaxFreq/1000,pMax);
        pScaleMin = std::min(m_hwMonitoringData.m_cpusFreq.at(i).m_data.m_cpuScalingMinFreq/1000,pScaleMin);
        pScaleMax = std::max(m_hwMonitoringData.m_cpusFreq.at(i).m_data.m_cpuScalingMaxFreq/1000,pScaleMax);
        return true;
    });
    ui->widget_PCoresAvgFreq->init("P-Core [%1 MHz]",pMin,pMax,pScaleMin,pScaleMax);

    quint32 eMin = std::numeric_limits<quint32>::max();
    quint32 eMax = std::numeric_limits<quint32>::min();
    quint32 eScaleMin = std::numeric_limits<quint32>::max();
    quint32 eScaleMax = std::numeric_limits<quint32>::min();
    forAllCpuEfficientCores([&eMin,&eMax,&eScaleMin,&eScaleMax,this](const int i){
        eMin = std::min(m_hwMonitoringData.m_cpusFreq.at(i).m_data.m_cpuInfoMinFreq/1000,eMin);
        eMax = std::max(m_hwMonitoringData.m_cpusFreq.at(i).m_data.m_cpuInfoMaxFreq/1000,eMax);
        eScaleMin = std::min(m_hwMonitoringData.m_cpusFreq.at(i).m_data.m_cpuScalingMinFreq/1000,eScaleMin);
        eScaleMax = std::max(m_hwMonitoringData.m_cpusFreq.at(i).m_data.m_cpuScalingMaxFreq/1000,eScaleMax);
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


    ui->widget_GPUTemp->init("GPU [%1 °C]",30,100,30,100);
    ui->widget_CPUFanSpeed->init("CPU [%1 RPM]",0,m_hwMonitoringData.m_lenovo.m_data.m_fan1MaxSpeed + 100,0,m_hwMonitoringData.m_lenovo.m_data.m_fan1MaxSpeed + 100);
    ui->widget_GPUFanSpeed->init("GPU [%1 RPM]",0,m_hwMonitoringData.m_lenovo.m_data.m_fan2MaxSpeed + 100,0,m_hwMonitoringData.m_lenovo.m_data.m_fan2MaxSpeed + 100);

    connect(m_windowFreqInfoByCore,&CPUFrequency::closed,this,&HWMonitoring::on_freqInfoByCoreClosed);
}

void HWMonitoring::refresh()
{
    const LenovoLegionDaemon::HWMonitoring::DataInfo  data        = m_dataProvider->getHWMonitoringData();





    /*
     * HW monitoring
     */
    ui->widget_CPUTemp->refresh(data.m_lenovo.m_data.m_cpuTemp / 1000,0,10,' ',QString("CPU Temperature: %1 °C\n"\
                                                                                       "CPU Temperature min: 30 °C\n"\
                                                                                       "CPU Temperature max: 100 °C\n").arg(data.m_lenovo.m_data.m_cpuTemp / 1000));

    {

        quint64 diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_lastRefreshTime).count();

        if(diff < (MainWindow::TIMER_EVENT_IN_MS * 1200) && diff > (MainWindow::TIMER_EVENT_IN_MS * 800))
        {
            ui->widget_CPUPower->refresh((data.m_intelPowerapRapl.m_data.m_powercapCPUEnergy - m_hwMonitoringData.m_intelPowerapRapl.m_data.m_powercapCPUEnergy)/diff ,
                                         0,10,' ',
                                         QString("CPU Power : %1 W\n"
                                                 "CPU Power min: 0 W\n"
                                                 "CPU Power max: 250 W\n").arg((data.m_intelPowerapRapl.m_data.m_powercapCPUEnergy - m_hwMonitoringData.m_intelPowerapRapl.m_data.m_powercapCPUEnergy)/diff));
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

    ui->widget_GPUTemp->refresh(data.m_lenovo.m_data.m_gpuTemp / 1000,0,10,' ',QString("GPU Temperature: %1 °C\n"\
                                                                                       "GPU Temperature min: 30 °C\n"\
                                                                                       "GPU Temperature max: 100 °C\n").arg(data.m_lenovo.m_data.m_gpuTemp / 1000));

    ui->widget_CPUFanSpeed->refresh(data.m_lenovo.m_data.m_fan1Speed,0,10,' ',QString("CPU FAN Speed: %1 RPM\n"\
                                                                                      "CPU FAN Speed min: 0 RPM\n"\
                                                                                      "CPU FAN Speed max: %2 RPM\n").arg(data.m_lenovo.m_data.m_fan1Speed).arg((data.m_lenovo.m_data.m_fan1MaxSpeed)));
    ui->widget_GPUFanSpeed->refresh(data.m_lenovo.m_data.m_fan2Speed,0,10,' ',QString("GPU FAN Speed: %1 RPM\n"\
                                                                                      "GPU FAN Speed min: 0 RPM\n"\
                                                                                      "GPU FAN Speed max: %2 RPM\n").arg(data.m_lenovo.m_data.m_fan2Speed).arg(data.m_lenovo.m_data.m_fan2MaxSpeed));



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

    /*
     * Render CPU Frequency Performance
     */
    forAllCpuPerformanceCores([this,&data,&l_stats](const int i)
                              {
                                  dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getPerforamnceWidget(i))->setBaseFreq(data.m_cpusFreq.at(i).m_data.m_cpuBaseFreq / 1000);
                                  dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getPerforamnceWidget(i))->setMinFreq(data.m_cpusFreq.at(i).m_data.m_cpuInfoMinFreq / 1000);
                                  dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getPerforamnceWidget(i))->setMaxFreq(data.m_cpusFreq.at(i).m_data.m_cpuInfoMaxFreq / 1000);

                                  dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getPerforamnceWidget(i))->setScalingMinFreq(data.m_cpusFreq.at(i).m_data.m_cpuScalingMinFreq / 1000);
                                  dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getPerforamnceWidget(i))->setScalingMaxFreq(data.m_cpusFreq.at(i).m_data.m_cpuScalingMaxFreq / 1000);
                                  dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getPerforamnceWidget(i))->setScalingCurFreq(data.m_cpusFreq.at(i).m_data.m_cpuScalingCurFreq / 1000);


                                  l_stats.avareqePCoreFreq += data.m_cpusFreq.at(i).m_data.m_cpuScalingCurFreq;++l_stats.avareqePCoreFreqCount;
                                  l_stats.averageFreq      += data.m_cpusFreq.at(i).m_data.m_cpuScalingCurFreq;++l_stats.averageFreqCount;
                                  return true;
                              });


    /*
     * Render CPU Frequency Efficiency
     */
    forAllCpuEfficientCores([this,&data,&l_stats](const int i)
                            {
                                dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getEfficiencyWidget(i))->setBaseFreq(data.m_cpusFreq.at(i).m_data.m_cpuBaseFreq / 1000);
                                dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getEfficiencyWidget(i))->setMinFreq(data.m_cpusFreq.at(i).m_data.m_cpuInfoMinFreq / 1000);
                                dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getEfficiencyWidget(i))->setMaxFreq(data.m_cpusFreq.at(i).m_data.m_cpuInfoMaxFreq / 1000);

                                dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getEfficiencyWidget(i))->setScalingMinFreq(data.m_cpusFreq.at(i).m_data.m_cpuScalingMinFreq / 1000);
                                dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getEfficiencyWidget(i))->setScalingMaxFreq(data.m_cpusFreq.at(i).m_data.m_cpuScalingMaxFreq / 1000);
                                dynamic_cast<ThreadFrequency*>(m_windowFreqInfoByCore->getEfficiencyWidget(i))->setScalingCurFreq(data.m_cpusFreq.at(i).m_data.m_cpuScalingCurFreq / 1000);

                                l_stats.avareqeACoreFreq += data.m_cpusFreq.at(i).m_data.m_cpuScalingCurFreq;++l_stats.avareqeACoreFreqCount;
                                l_stats.averageFreq      += data.m_cpusFreq.at(i).m_data.m_cpuScalingCurFreq;++l_stats.averageFreqCount;
                                return true;
                            });

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

    m_hwMonitoringData = m_dataProvider->getHWMonitoringData();
    m_lastRefreshTime  = std::chrono::steady_clock::now();
}

HWMonitoring::~HWMonitoring()
{
    delete m_windowFreqInfoByCore;
    delete ui;    
}

void HWMonitoring::forAllCpuPerformanceCores(const std::function<bool (const int)> &func)
{
    LenovoLegionDaemon::CPUTopology::forAllCpuRange(func,m_cpuHetTopology.m_isAvailable ? m_cpuHetTopology.m_data.m_ActiveCpusCore : m_cpuHomTopology.m_data.m_ActiveCpus);
}

void HWMonitoring::forAllCpuEfficientCores(const std::function<bool (const int)> &func)
{
    if(m_cpuHetTopology.m_isAvailable)
    {
        LenovoLegionDaemon::CPUTopology::forAllCpuRange(func,m_cpuHetTopology.m_data.m_ActiveCpusAtom);
    }
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

void HWMonitoring::on_freqInfoByCoreClosed()
{
    ui->groupBox_17->setChecked(false);
}




}
