// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include "ProtocolProcessorBase.h"

#include <Core/ExceptionBuilder.h>

#include <ControlData.h>
#include <Message.h>

#include <QObject>
#include <QLocalSocket>

namespace LenovoLegionGui {


class ProtocolProcessor : public ProtocolProcessorBase
{
    Q_OBJECT


public:

    enum ERROR_CODES : int {
        TIMEOUT_ERROR     = -1,
        INVALID_MESSAGE   = -2
    };


    DEFINE_EXCEPTION(ProtocolProcessor);

public:

    explicit ProtocolProcessor(QObject *parent = nullptr);
    virtual ~ProtocolProcessor();


    /*
     * RequestResponse
     */
    LenovoLegionDaemon::HWMonitoring::DataInfo                  sendRequestSensorData();
    LenovoLegionDaemon::PowerProfile::Control::DataInfo         sendRequestPoowerProfileControlData();

    LenovoLegionDaemon::PowerControl::CPU::DataInfo             sendRequestCPUControlData();
    LenovoLegionDaemon::PowerControl::GPU::DataInfo             sendRequestGPUControlData();
    LenovoLegionDaemon::FanControl::CurveControl::DataInfo      sendRequestFanCurveControlData();
    LenovoLegionDaemon::FanControl::Control::DataInfo           sendRequestFanControlData();
    LenovoLegionDaemon::Batery::DataInfo                        sendRequestBateryControlData();

    LenovoLegionDaemon::CPUXFreqControl::DataInfo               sendRequestCPUsInfoData();
    LenovoLegionDaemon::CPUTopology::Heterogeneous::DataInfo    sendRequestCPUHeterogenousTopologyData();
    LenovoLegionDaemon::CPUTopology::Homogeneous::DataInfo      sendRequestCPUHomogeneousTopologyData();

    LenovoLegionDaemon::CPUXControl::DataInfo                   sendRequestCPUsInfoControlData();

    LenovoLegionDaemon::CPUSMTControl::DataInfo                 sendRequestCPUSMTControlData();

    /*
     * Request without response
     */
    void sendRequestSetPowerProfileControlData(const LenovoLegionDaemon::PowerProfile::Control::DataControl &data);
    void sendRequestSetCPUControlData(const LenovoLegionDaemon::PowerControl::CPU::DataControl &data);
    void sendRequestSetGPUControlData(const LenovoLegionDaemon::PowerControl::GPU::DataControl &data);
    void sendRequestSetFanCurveControlData(const LenovoLegionDaemon::FanControl::CurveControl::DataControl &data);
    void sendRequestSetFanControlData(const LenovoLegionDaemon::FanControl::Control::DataControl &data);
    void sendRequestSetCPUsFreqControlData(const LenovoLegionDaemon::CPUXFreqControl::DataControl &data);
    void sendRequestSetCPUsControlData(const LenovoLegionDaemon::CPUXControl::DataControl &data);
    void sendRequestSetCPUSMTControlData(const LenovoLegionDaemon::CPUSMTControl::DataControl &data);
};


}
