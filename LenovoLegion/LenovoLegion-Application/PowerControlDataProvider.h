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


class ProtocolProcessor;

class PowerControlDataProvider : public QObject
{
    Q_OBJECT
public:
    explicit PowerControlDataProvider(ProtocolProcessor * protocolProcessor, QObject *parent = nullptr);

    ~PowerControlDataProvider() = default;

    /*
     * Getters
     */
    LenovoLegionDaemon::PowerControl::GPU::DataInfo          getGpuControlData()       const;
    LenovoLegionDaemon::PowerControl::CPU::DataInfo          getCpuControlData()       const;

    /*
     * Setters
     */
    void setGpuControlData(const LenovoLegionDaemon::PowerControl::GPU::DataControl& data);
    void setCpuControlData(const LenovoLegionDaemon::PowerControl::CPU::DataControl& data);

signals:

private:

    ProtocolProcessor*          m_protocolProcessor;
};

}
