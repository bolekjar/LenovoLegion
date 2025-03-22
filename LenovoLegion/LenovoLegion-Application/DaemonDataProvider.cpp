// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "DaemonDataProvider.h"

#include "ProtocolProcessor.h"
#include "ProtocolProcessorNotifier.h"
#include "PowerControlDataProvider.h"
#include "FanControlDataProvider.h"
#include "HWMonitoringDataProvider.h"
#include "CPUControlDataProvider.h"
#include "CPUFrequencyControlDataProvider.h"
#include "PowerProfileControlDataProvider.h"
#include "BateryStatusDataProvider.h"

#include <../LenovoLegion-Daemon/Application.h>


#include <Core/LoggerHolder.h>

using namespace LenovoLegionDaemon;

namespace  LenovoLegionGui {


DaemonDataProvider::DaemonDataProvider(QObject *parent) : DataProvider(parent),
    m_protocolProcessor(new ProtocolProcessor(this)),
    m_protocolProcessorNotifier(new ProtocolProcessorNotifier(this)),
    m_powerControlDataProvider(new PowerControlDataProvider(m_protocolProcessor,this)),
    m_fanControlDataProvider(new FanControlDataProvider(m_protocolProcessor,this)),
    m_hwMonitoringDataProvider(new HWMonitoringDataProvider(m_protocolProcessor,this)),
    m_cpuControlDataProvider(new CPUControlDataProvider(m_protocolProcessor,this)),
    m_cpuFrequencyControlDataProvider(new CPUFrequencyControlDataProvider(m_protocolProcessor,this)),
    m_powerProfileControlDataProvider(new PowerProfileControlDataProvider(m_protocolProcessor,this)),
    m_bateryStatusDataProvider(new BateryStatusDataProvider(m_protocolProcessor,this))
{
    connect(m_protocolProcessor,&ProtocolProcessor::connected,this,&DaemonDataProvider::on_protocolProcessorConnected);
    connect(m_protocolProcessor,&ProtocolProcessor::disconnected,this,&DaemonDataProvider::on_protocolProcessorDisconnectd);
    connect(m_protocolProcessorNotifier,&ProtocolProcessorNotifier::powerProfileChanged,this,&DaemonDataProvider::on_powerProfileChanged);
    connect(m_protocolProcessorNotifier,&ProtocolProcessorNotifier::batteryStatusChanged,this,&DaemonDataProvider::on_batteryStatusChanged);
    connect(m_protocolProcessorNotifier,&ProtocolProcessorNotifier::lenovoDriverAdded,this,&DaemonDataProvider::on_lenovoDriverAdded);
    connect(m_protocolProcessorNotifier,&ProtocolProcessorNotifier::lenovoDriverRemoved,this,&DaemonDataProvider::on_lenovoDriverRemoved);
    connect(m_protocolProcessorNotifier,&ProtocolProcessorNotifier::cpuXListReloaded,this,&DaemonDataProvider::on_cpuXListReloaded);
}

BateryStatusDataProvider *DaemonDataProvider::getBateryStatusDataProvider()
{
    return m_bateryStatusDataProvider;
}

PowerControlDataProvider *DaemonDataProvider::getPowerControlDataProvider()
{
    return m_powerControlDataProvider;
}

FanControlDataProvider *DaemonDataProvider::getFanControlDataProvider()
{
    return m_fanControlDataProvider;
}

HWMonitoringDataProvider *DaemonDataProvider::getHWMonitoringDataProvider()
{
    return m_hwMonitoringDataProvider;
}

CPUControlDataProvider *DaemonDataProvider::getCPUControlDataProvider()
{
    return m_cpuControlDataProvider;
}

CPUFrequencyControlDataProvider *DaemonDataProvider::getCPUFrequencyControlDataProvider()
{
    return m_cpuFrequencyControlDataProvider;
}

PowerProfileControlDataProvider *DaemonDataProvider::getPowerProfileControlDataProvider()
{
    return m_powerProfileControlDataProvider;
}

void DaemonDataProvider::on_protocolProcessorConnected()
{
    emit dataProviderEvent({
                            .m_type = DataProvider::Message::Type::NOTIFICATION_CLIENT_IS_CONNECTED
                            });
}

void DaemonDataProvider::on_protocolProcessorDisconnectd()
{

    emit dataProviderEvent({
        .m_type = DataProvider::Message::Type::NOTIFICATION_CLIENT_IS_DISCONNECTED
    });

}

void DaemonDataProvider::on_powerProfileChanged()
{
    emit dataProviderEvent({
        .m_type = DataProvider::Message::Type::NOTIFICATION_POWER_PROFILE_CHANGED
    });
}

void DaemonDataProvider::on_batteryStatusChanged()
{
    emit dataProviderEvent({
        .m_type = DataProvider::Message::Type::NOTIFICATION_BATTERY_STATUS_CHANGED
    });
}

void DaemonDataProvider::on_lenovoDriverAdded()
{
    emit dataProviderEvent({
        .m_type = DataProvider::Message::Type::NOTIFICATION_LENOVO_DRIVER_ADDED
    });
}

void DaemonDataProvider::on_lenovoDriverRemoved()
{
    emit dataProviderEvent({
        .m_type = DataProvider::Message::Type::NOTIFICATION_LENOVO_DRIVER_REMOVED
    });
}

void DaemonDataProvider::on_cpuXListReloaded()
{
    emit dataProviderEvent({
        .m_type = DataProvider::Message::Type::NOTIFICATION_CPU_X_LIST_RELOADED
    });
}

}
