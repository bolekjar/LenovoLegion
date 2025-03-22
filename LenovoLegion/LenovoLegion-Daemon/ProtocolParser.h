// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include "Message.h"

#include <Core/ExceptionBuilder.h>


#include <QByteArray>
#include <QLocalSocket>

namespace LenovoLegionDaemon {

struct ProtocolParser
{

    DEFINE_EXCEPTION(Protocol)

    enum ERROR_CODES : int {
        PROTOCOL_SIZE_ERROR             = -1,
        PROTOCOL_ERROR_TYPE             = -2,
        PROTOCOL_ERROR_DATA_TYPE        = -3,

        DATA_NOT_READY                  = 4
    };


    static void           parseMessage(QLocalSocket& socket,std::function<void(const Message& message)> callback);
    static QByteArray     parseMessage(const Message& message);

private:

    static QByteArray     readDataWithTimeout(QLocalSocket& socket,int size);

    static void           parseMessage(QLocalSocket &socket,HWMonitoring::DataInfo& message);

    static void           parseMessage(QLocalSocket &socket,FanControl::CurveControl::DataInfo& message);
    static void           parseMessage(QLocalSocket &socket,FanControl::CurveControl::DataControl& message);
    static void           parseMessage(QLocalSocket &socket,FanControl::CurveControl::Data& message);

    static void           parseMessage(QLocalSocket &socket,FanControl::Control::DataInfo& message);
    static void           parseMessage(QLocalSocket &socket,FanControl::Control::DataControl& message);
    static void           parseMessage(QLocalSocket &socket,FanControl::Control::Data& message);

    static void           parseMessage(QLocalSocket &socket,PowerControl::CPU::DataInfo& message);
    static void           parseMessage(QLocalSocket &socket,PowerControl::CPU::DataControl& message);
    static void           parseMessage(QLocalSocket &socket,PowerControl::CPU::Data& message);

    static void           parseMessage(QLocalSocket &socket,PowerControl::GPU::DataInfo& message);
    static void           parseMessage(QLocalSocket &socket,PowerControl::GPU::DataControl& message);
    static void           parseMessage(QLocalSocket &socket,PowerControl::GPU::Data& message);




    static void           parseMessage(QLocalSocket &socket,PowerProfile::Control::DataInfo& message);
    static void           parseMessage(QLocalSocket &socket,PowerProfile::Control::DataControl& message);
    static void           parseMessage(QLocalSocket &socket,PowerProfile::Control::Data& message);

    static void           parseMessage(QLocalSocket &socket,Batery::DataInfo& message);


    static void           parseMessage(QLocalSocket &socket,Text& message);
    static void           parseMessage(QLocalSocket &socket,Message::EnumerationType& message);

    static void           parseMessage(QLocalSocket &socket,CPUTopology::Heterogeneous::DataInfo& message);
    static void           parseMessage(QLocalSocket &socket,CPUTopology::Homogeneous::DataInfo& message);



    static void           parseMessage(QLocalSocket &socket,CPUXFreqControl::DataInfo& message);
    static void           parseMessage(QLocalSocket &socket,CPUXFreqControl::DataControl& message);






    static void           parseMessage(QLocalSocket &socket,CPUXControl::DataInfo& message);
    static void           parseMessage(QLocalSocket &socket,CPUXControl::DataControl& message);


    static void           parseMessage(QLocalSocket &socket,CPUSMTControl::DataInfo& message);
    static void           parseMessage(QLocalSocket &socket,CPUSMTControl::DataControl& message);
};


}
