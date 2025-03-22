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

class CPUFrequencyControlDataProvider : public QObject
{
    Q_OBJECT

public:

    explicit CPUFrequencyControlDataProvider(ProtocolProcessor * protocolProcessor,QObject *parent = nullptr);

    LenovoLegionDaemon::CPUTopology::Heterogeneous::DataInfo        getCPUHeterogenousTopologyData()    const;
    LenovoLegionDaemon::CPUTopology::Homogeneous::DataInfo          getCPUHomogeneousTopologyData()     const;

    LenovoLegionDaemon::CPUXFreqControl::DataInfo                   getCPUsInfoData()       const;

    void setCpuFreqControlData(const LenovoLegionDaemon::CPUXFreqControl::DataControl& data);

signals:

private:

    ProtocolProcessor*  m_protocolProcessor;
};

}
