// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "BateryStatusDataProvider.h"

#include "ProtocolProcessor.h"

namespace  LenovoLegionGui {

BateryStatusDataProvider::BateryStatusDataProvider(ProtocolProcessor * protocolProcessor,QObject *parent)
    : QObject{parent}
    , m_protocolProcessor{protocolProcessor}
{}

LenovoLegionDaemon::Batery::DataInfo BateryStatusDataProvider::getBateryControlData() const
{
    return m_protocolProcessor->sendRequestBateryControlData();
}

}
