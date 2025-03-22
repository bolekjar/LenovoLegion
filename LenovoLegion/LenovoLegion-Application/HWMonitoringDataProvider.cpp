// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "HWMonitoringDataProvider.h"

#include "ProtocolProcessor.h"

namespace  LenovoLegionGui {

HWMonitoringDataProvider::HWMonitoringDataProvider(ProtocolProcessor * protocolProcessor,QObject *parent)
    : QObject{parent}
    , m_dataProcessor(protocolProcessor)
{

}

LenovoLegionDaemon::HWMonitoring::DataInfo HWMonitoringDataProvider::getHWMonitoringData() const
{
    return m_dataProcessor->sendRequestSensorData();
}

LenovoLegionDaemon::CPUTopology::Heterogeneous::DataInfo HWMonitoringDataProvider::getCPUHeterogenousTopologyData() const
{
    return m_dataProcessor->sendRequestCPUHeterogenousTopologyData();
}

LenovoLegionDaemon::CPUTopology::Homogeneous::DataInfo HWMonitoringDataProvider::getCPUHomogeneousTopologyData() const
{
    return m_dataProcessor->sendRequestCPUHomogeneousTopologyData();
}

}
