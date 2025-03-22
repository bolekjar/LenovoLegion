// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "CPUFrequencyControlDataProvider.h"

#include "ProtocolProcessor.h"


namespace LenovoLegionGui {

CPUFrequencyControlDataProvider::CPUFrequencyControlDataProvider(ProtocolProcessor * protocolProcessor,QObject *parent)
    : QObject{parent}
    , m_protocolProcessor{protocolProcessor}
{}

LenovoLegionDaemon::CPUTopology::Heterogeneous::DataInfo CPUFrequencyControlDataProvider::getCPUHeterogenousTopologyData() const
{
        return m_protocolProcessor->sendRequestCPUHeterogenousTopologyData();
}

LenovoLegionDaemon::CPUTopology::Homogeneous::DataInfo CPUFrequencyControlDataProvider::getCPUHomogeneousTopologyData() const
{
        return m_protocolProcessor->sendRequestCPUHomogeneousTopologyData();
}

LenovoLegionDaemon::CPUXFreqControl::DataInfo CPUFrequencyControlDataProvider::getCPUsInfoData() const
{
        return m_protocolProcessor->sendRequestCPUsInfoData();
}

void CPUFrequencyControlDataProvider::setCpuFreqControlData(const LenovoLegionDaemon::CPUXFreqControl::DataControl &data)
{
    m_protocolProcessor->sendRequestSetCPUsFreqControlData(data);
}

}
