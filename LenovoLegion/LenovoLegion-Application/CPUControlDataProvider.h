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

class CPUControlDataProvider : public QObject
{
    Q_OBJECT
public:
    explicit CPUControlDataProvider(ProtocolProcessor * protocolProcessor,QObject *parent = nullptr);

    LenovoLegionDaemon::CPUXControl::DataInfo                        getCPUXInfoControlData()              const;
    LenovoLegionDaemon::CPUTopology::Heterogeneous::DataInfo         getCPUHeterogenousTopologyData()      const;
     LenovoLegionDaemon::CPUTopology::Homogeneous::DataInfo          getCPUHomogeneousTopologyData()       const;
    LenovoLegionDaemon::CPUSMTControl::DataInfo                      getCPUSMTControlData()                const;

    void setCpuSControlData(const LenovoLegionDaemon::CPUXControl::DataControl &data);
    void setCpuSMTControlData(const LenovoLegionDaemon::CPUSMTControl::DataControl &data);

signals:

private:

    ProtocolProcessor*  m_protocolProcessor;
};

}
