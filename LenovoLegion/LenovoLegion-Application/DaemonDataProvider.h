// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <ControlData.h>
#include <DataProvider.h>

namespace  LenovoLegionGui {

class ProtocolProcessor;
class ProtocolProcessorNotifier;


class DaemonDataProvider :  public DataProvider
{
    Q_OBJECT

public:

    DaemonDataProvider(QObject *parent);



    BateryStatusDataProvider*        getBateryStatusDataProvider()          override;
    PowerControlDataProvider*        getPowerControlDataProvider()          override;
    FanControlDataProvider*          getFanControlDataProvider()            override;
    HWMonitoringDataProvider*        getHWMonitoringDataProvider()          override;
    CPUControlDataProvider*          getCPUControlDataProvider()            override;
    CPUFrequencyControlDataProvider* getCPUFrequencyControlDataProvider()   override;
    PowerProfileControlDataProvider* getPowerProfileControlDataProvider()   override;

private slots:

    void on_protocolProcessorConnected();
    void on_protocolProcessorDisconnectd();
    void on_powerProfileChanged();
    void on_batteryStatusChanged();
    void on_lenovoDriverAdded();
    void on_lenovoDriverRemoved();
    void on_cpuXListReloaded();


private:

    ProtocolProcessor*          m_protocolProcessor;
    ProtocolProcessorNotifier*  m_protocolProcessorNotifier;

    PowerControlDataProvider*        m_powerControlDataProvider;
    FanControlDataProvider*          m_fanControlDataProvider;
    HWMonitoringDataProvider*        m_hwMonitoringDataProvider;
    CPUControlDataProvider*          m_cpuControlDataProvider;
    CPUFrequencyControlDataProvider* m_cpuFrequencyControlDataProvider;
    PowerProfileControlDataProvider* m_powerProfileControlDataProvider;
    BateryStatusDataProvider*        m_bateryStatusDataProvider;
};

}
