// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "PowerProfileControlDataProvider.h"

#include "ProtocolProcessor.h"

namespace LenovoLegionGui {

PowerProfileControlDataProvider::PowerProfileControlDataProvider(ProtocolProcessor* protocolProcessor,QObject *parent)
    : QObject{parent},
      m_protocolProcessor{protocolProcessor}
{}

LenovoLegionDaemon::PowerProfile::Control::DataInfo PowerProfileControlDataProvider::getPowerProfileData() const
{
    return m_protocolProcessor->sendRequestPoowerProfileControlData();
}

LenovoLegionDaemon::Batery::DataInfo PowerProfileControlDataProvider::getBateryControlData() const
{
    return m_protocolProcessor->sendRequestBateryControlData();
}

void PowerProfileControlDataProvider::setPowerProfileData(const LenovoLegionDaemon::PowerProfile::Control::DataControl &data)
{
    m_protocolProcessor->sendRequestSetPowerProfileControlData(data);
}

}
