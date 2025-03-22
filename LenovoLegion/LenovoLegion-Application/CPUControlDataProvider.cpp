// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "CPUControlDataProvider.h"

#include "ProtocolProcessor.h"

namespace  LenovoLegionGui {

CPUControlDataProvider::CPUControlDataProvider(ProtocolProcessor* protocolProcessor,QObject *parent)
    : QObject{parent}
    , m_protocolProcessor{protocolProcessor}
{}

LenovoLegionDaemon::CPUXControl::DataInfo CPUControlDataProvider::getCPUXInfoControlData() const
{
    return m_protocolProcessor->sendRequestCPUsInfoControlData();
}

LenovoLegionDaemon::CPUTopology::Heterogeneous::DataInfo CPUControlDataProvider::getCPUHeterogenousTopologyData() const
{
    return m_protocolProcessor->sendRequestCPUHeterogenousTopologyData();
}

LenovoLegionDaemon::CPUTopology::Homogeneous::DataInfo CPUControlDataProvider::getCPUHomogeneousTopologyData() const
{
    return m_protocolProcessor->sendRequestCPUHomogeneousTopologyData();
}

LenovoLegionDaemon::CPUSMTControl::DataInfo CPUControlDataProvider::getCPUSMTControlData() const
{
    return m_protocolProcessor->sendRequestCPUSMTControlData();
}

void CPUControlDataProvider::setCpuSControlData(const LenovoLegionDaemon::CPUXControl::DataControl &data)
{
    m_protocolProcessor->sendRequestSetCPUsControlData(data);
}

void CPUControlDataProvider::setCpuSMTControlData(const LenovoLegionDaemon::CPUSMTControl::DataControl &data)
{
    m_protocolProcessor->sendRequestSetCPUSMTControlData(data);
}

}
