// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "FanControlDataProvider.h"

#include "ProtocolProcessor.h"

namespace  LenovoLegionGui {

FanControlDataProvider::FanControlDataProvider(ProtocolProcessor * protocolProcessor,QObject *parent)
    : QObject{parent},
      m_protocolProcessor(protocolProcessor)
{}

LenovoLegionDaemon::FanControl::Control::DataInfo FanControlDataProvider::getFanControlData() const
{
    return m_protocolProcessor->sendRequestFanControlData();
}

LenovoLegionDaemon::FanControl::CurveControl::DataInfo FanControlDataProvider::getFanCurveControlData() const
{
    return m_protocolProcessor->sendRequestFanCurveControlData();
}

void FanControlDataProvider::setFanControlData(const LenovoLegionDaemon::FanControl::Control::DataControl &data)
{
    m_protocolProcessor->sendRequestSetFanControlData(data);
}

void FanControlDataProvider::setFanCurveControlData(const LenovoLegionDaemon::FanControl::CurveControl::DataControl &data)
{
    m_protocolProcessor->sendRequestSetFanCurveControlData(data);
}

}
