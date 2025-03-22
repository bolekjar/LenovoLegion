// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "PowerControlDataProvider.h"

#include "ProtocolProcessor.h"


namespace LenovoLegionGui {

PowerControlDataProvider::PowerControlDataProvider(ProtocolProcessor *protocolProcessor, QObject *parent) : QObject(parent),
    m_protocolProcessor(protocolProcessor)
{}

LenovoLegionDaemon::PowerControl::GPU::DataInfo PowerControlDataProvider::getGpuControlData() const
{
    return m_protocolProcessor->sendRequestGPUControlData();
}

LenovoLegionDaemon::PowerControl::CPU::DataInfo PowerControlDataProvider::getCpuControlData() const
{
    return m_protocolProcessor->sendRequestCPUControlData();
}

void PowerControlDataProvider::setGpuControlData(const LenovoLegionDaemon::PowerControl::GPU::DataControl &data)
{
    m_protocolProcessor->sendRequestSetGPUControlData(data);
}

void PowerControlDataProvider::setCpuControlData(const LenovoLegionDaemon::PowerControl::CPU::DataControl &data)
{
    m_protocolProcessor->sendRequestSetCPUControlData(data);
}

}
