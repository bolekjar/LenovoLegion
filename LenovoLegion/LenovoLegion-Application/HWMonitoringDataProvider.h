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


namespace  LenovoLegionGui {

class ProtocolProcessor;

class HWMonitoringDataProvider : public QObject
{
    Q_OBJECT
public:

    explicit HWMonitoringDataProvider(ProtocolProcessor * protocolProcessor,QObject *parent = nullptr);

    LenovoLegionDaemon::HWMonitoring::DataInfo                getHWMonitoringData()             const;
    LenovoLegionDaemon::CPUTopology::Heterogeneous::DataInfo  getCPUHeterogenousTopologyData()  const;
    LenovoLegionDaemon::CPUTopology::Homogeneous::DataInfo    getCPUHomogeneousTopologyData()   const;

signals:

private:

    ProtocolProcessor *m_dataProcessor;
};

}
