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

#include <QObject>


namespace LenovoLegionGui {

class PowerProfileControlDataProvider : public QObject
{
    Q_OBJECT
public:
    explicit PowerProfileControlDataProvider(ProtocolProcessor* protocolProcessor,QObject *parent = nullptr);

    LenovoLegionDaemon::PowerProfile::Control::DataInfo getPowerProfileData()  const;
    LenovoLegionDaemon::Batery::DataInfo                getBateryControlData() const;

    void setPowerProfileData(const LenovoLegionDaemon::PowerProfile::Control::DataControl& data);

signals:


private:
    ProtocolProcessor* m_protocolProcessor;
};

}
