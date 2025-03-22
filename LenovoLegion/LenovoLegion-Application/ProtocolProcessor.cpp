// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "ProtocolProcessor.h"

#include <ProtocolParser.h>

#include <Core/LoggerHolder.h>

#include <../LenovoLegion-Daemon/Application.h>

namespace LenovoLegionGui {

ProtocolProcessor::ProtocolProcessor(QObject *parent)
    : ProtocolProcessorBase(LenovoLegionDaemon::Application::SOCKET_NAME,parent)
{}

ProtocolProcessor::~ProtocolProcessor()
{}

LenovoLegionDaemon::HWMonitoring::DataInfo ProtocolProcessor::sendRequestSensorData()
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_GET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::ENUMERATION,
        .m_data = {
            .m_enumType = LenovoLegionDaemon::Message::EnumerationType::GET_HW_MONITORING_DATA
        }
    });

    auto msg = receiveMessage();

    if(msg.m_type != LenovoLegionDaemon::Message::RESPONSE_GET_DATA || msg.m_dataType != LenovoLegionDaemon::Message::HW_MONITORING_DATA)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }

    return msg.m_data.m_hwMonitoringData;
}

LenovoLegionDaemon::PowerProfile::Control::DataInfo ProtocolProcessor::sendRequestPoowerProfileControlData()
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_GET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::ENUMERATION,
        .m_data = {
            .m_enumType = LenovoLegionDaemon::Message::EnumerationType::GET_POWER_PROFILE_CONTROL_DATA
        }
    });

    auto msg = receiveMessage();

    if(msg.m_type != LenovoLegionDaemon::Message::RESPONSE_GET_DATA || msg.m_dataType != LenovoLegionDaemon::Message::POWER_PROFILE_CONTROL_DATA)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }

    return msg.m_data.m_powerProfileControlData;

}

LenovoLegionDaemon::PowerControl::CPU::DataInfo ProtocolProcessor::sendRequestCPUControlData()
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_GET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::ENUMERATION,
        .m_data = {
            .m_enumType = LenovoLegionDaemon::Message::EnumerationType::GET_CPU_CONTROL_DATA
        }
    });

    auto msg = receiveMessage();

    if(msg.m_type != LenovoLegionDaemon::Message::RESPONSE_GET_DATA || msg.m_dataType != LenovoLegionDaemon::Message::CPU_CONTROL_DATA)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }

    return msg.m_data.m_cpuControlData;

}

LenovoLegionDaemon::PowerControl::GPU::DataInfo ProtocolProcessor::sendRequestGPUControlData()
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_GET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::ENUMERATION,
        .m_data = {
            .m_enumType = LenovoLegionDaemon::Message::EnumerationType::GET_GPU_CONTROL_DATA
        }
    });

    auto msg = receiveMessage();

    if(msg.m_type != LenovoLegionDaemon::Message::RESPONSE_GET_DATA || msg.m_dataType != LenovoLegionDaemon::Message::GPU_CONTROL_DATA)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }

    return msg.m_data.m_gpuControlData;
}

LenovoLegionDaemon::FanControl::CurveControl::DataInfo ProtocolProcessor::sendRequestFanCurveControlData()
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_GET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::ENUMERATION,
        .m_data = {
            .m_enumType = LenovoLegionDaemon::Message::EnumerationType::GET_FAN_CURVE_CONTROL_DATA
        }
    });

    auto msg = receiveMessage();

    if(msg.m_type != LenovoLegionDaemon::Message::RESPONSE_GET_DATA || msg.m_dataType != LenovoLegionDaemon::Message::FAN_CURVE_CONTROL_DATA)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }

    return msg.m_data.m_fanCurveControlData;

}

LenovoLegionDaemon::FanControl::Control::DataInfo ProtocolProcessor::sendRequestFanControlData()
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_GET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::ENUMERATION,
        .m_data = {
            .m_enumType = LenovoLegionDaemon::Message::EnumerationType::GET_FANCONTROL_DATA
        }
    });

    auto msg = receiveMessage();

    if(msg.m_type != LenovoLegionDaemon::Message::RESPONSE_GET_DATA || msg.m_dataType != LenovoLegionDaemon::Message::FANCONTROL_DATA)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }

    return msg.m_data.m_fanControlData;
}

LenovoLegionDaemon::Batery::DataInfo ProtocolProcessor::sendRequestBateryControlData()
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_GET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::ENUMERATION,
        .m_data = {
            .m_enumType = LenovoLegionDaemon::Message::EnumerationType::GET_BATTERY_CONTROL_DATA
        }
    });

    auto msg = receiveMessage();

    if(msg.m_type != LenovoLegionDaemon::Message::RESPONSE_GET_DATA || msg.m_dataType != LenovoLegionDaemon::Message::BATTERY_CONTROL_DATA)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }

    return msg.m_data.m_bateryControlData;
}

LenovoLegionDaemon::CPUXFreqControl::DataInfo ProtocolProcessor::sendRequestCPUsInfoData()
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_GET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::ENUMERATION,
        .m_data = {
            .m_enumType = LenovoLegionDaemon::Message::EnumerationType::GET_CPUS_FREQ_CONTROL_DATA
        }
    });

    auto msg = receiveMessage();

    if(msg.m_type != LenovoLegionDaemon::Message::RESPONSE_GET_DATA || msg.m_dataType != LenovoLegionDaemon::Message::CPUS_FREQ_CONTROL_DATA)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }

    return msg.m_data.m_cpusFreqControlData;
}

LenovoLegionDaemon::CPUTopology::Heterogeneous::DataInfo ProtocolProcessor::sendRequestCPUHeterogenousTopologyData()
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_GET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::ENUMERATION,
        .m_data = {
            .m_enumType = LenovoLegionDaemon::Message::EnumerationType::GET_CPU_HETEROGENOUS_TOPOLOGY_DATA
        }
    });

    auto msg = receiveMessage();

    if(msg.m_type != LenovoLegionDaemon::Message::RESPONSE_GET_DATA || msg.m_dataType != LenovoLegionDaemon::Message::CPU_HETEROGENOUS_TOPOLOGY_DATA)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }

    return msg.m_data.m_cpuHeteregenousTopologyData;
}

LenovoLegionDaemon::CPUTopology::Homogeneous::DataInfo ProtocolProcessor::sendRequestCPUHomogeneousTopologyData()
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_GET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::ENUMERATION,
        .m_data = {
            .m_enumType = LenovoLegionDaemon::Message::EnumerationType::GET_CPU_HOMOGENOUS_TOPOLOGY_DATA
        }
    });

    auto msg = receiveMessage();

    if(msg.m_type != LenovoLegionDaemon::Message::RESPONSE_GET_DATA || msg.m_dataType != LenovoLegionDaemon::Message::CPU_HOMOGENOUS_TOPOLOGY_DATA)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }

    return msg.m_data.m_cpuHomogenousTopologyData;
}

LenovoLegionDaemon::CPUXControl::DataInfo ProtocolProcessor::sendRequestCPUsInfoControlData()
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_GET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::ENUMERATION,
        .m_data = {
            .m_enumType = LenovoLegionDaemon::Message::EnumerationType::GET_CPUS_CONTROL_DATA
        }
    });

    auto msg = receiveMessage();

    if(msg.m_type != LenovoLegionDaemon::Message::RESPONSE_GET_DATA || msg.m_dataType != LenovoLegionDaemon::Message::CPUS_CONTROL_DATA)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }

    return msg.m_data.m_cpusControlData;
}

LenovoLegionDaemon::CPUSMTControl::DataInfo ProtocolProcessor::sendRequestCPUSMTControlData()
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_GET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::ENUMERATION,
        .m_data = {
            .m_enumType = LenovoLegionDaemon::Message::EnumerationType::GET_CPU_SMT_CONTROL_DATA
        }
    });

    auto msg = receiveMessage();

    if(msg.m_type != LenovoLegionDaemon::Message::RESPONSE_GET_DATA || msg.m_dataType != LenovoLegionDaemon::Message::CPUS_SMT_CONTROL_DATA)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }

    return msg.m_data.m_cpusSmtControlData;
}

void ProtocolProcessor::sendRequestSetPowerProfileControlData(const LenovoLegionDaemon::PowerProfile::Control::DataControl &data)
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_SET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::POWER_PROFILE_CONTROL_DATA,
        .m_data = {
            .m_powerProfileControlDataControl = data
        }
    });

    auto msg = receiveMessage();
    if (msg.m_type != LenovoLegionDaemon::Message::RESPONSE_SET_DATA &&
        msg.m_dataType != LenovoLegionDaemon::Message::ENUMERATION &&
        msg.m_data.m_enumType != LenovoLegionDaemon::Message::EnumerationType::SET_DATA_SUCESS)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }
}

void ProtocolProcessor::sendRequestSetCPUControlData(const LenovoLegionDaemon::PowerControl::CPU::DataControl &data)
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_SET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::CPU_CONTROL_DATA,
        .m_data = {
            .m_cpuControlDataControl = data
        }
    });

    auto msg = receiveMessage();
    if (msg.m_type != LenovoLegionDaemon::Message::RESPONSE_SET_DATA &&
        msg.m_dataType != LenovoLegionDaemon::Message::ENUMERATION &&
        msg.m_data.m_enumType != LenovoLegionDaemon::Message::EnumerationType::SET_DATA_SUCESS)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }
}

void ProtocolProcessor::sendRequestSetGPUControlData(const LenovoLegionDaemon::PowerControl::GPU::DataControl &data)
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_SET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::GPU_CONTROL_DATA,
        .m_data = {
            .m_gpuControlDataControl = data
        }
    });

    auto msg = receiveMessage();
    if (msg.m_type != LenovoLegionDaemon::Message::RESPONSE_SET_DATA &&
        msg.m_dataType != LenovoLegionDaemon::Message::ENUMERATION &&
        msg.m_data.m_enumType != LenovoLegionDaemon::Message::EnumerationType::SET_DATA_SUCESS)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }
}

void ProtocolProcessor::sendRequestSetFanCurveControlData(const  LenovoLegionDaemon::FanControl::CurveControl::DataControl &data)
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_SET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::FAN_CURVE_CONTROL_DATA,
        .m_data = {
            .m_fanCurveControlDataControl = data
        }
    });

    auto msg = receiveMessage();
    if (msg.m_type != LenovoLegionDaemon::Message::RESPONSE_SET_DATA &&
        msg.m_dataType != LenovoLegionDaemon::Message::ENUMERATION &&
        msg.m_data.m_enumType != LenovoLegionDaemon::Message::EnumerationType::SET_DATA_SUCESS)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }
}

void ProtocolProcessor::sendRequestSetFanControlData(const LenovoLegionDaemon::FanControl::Control::DataControl &data)
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_SET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::FANCONTROL_DATA,
        .m_data = {
            .m_fanControlDataControl = data
        }
    });

    auto msg = receiveMessage();
    if (msg.m_type != LenovoLegionDaemon::Message::RESPONSE_SET_DATA &&
        msg.m_dataType != LenovoLegionDaemon::Message::ENUMERATION &&
        msg.m_data.m_enumType != LenovoLegionDaemon::Message::EnumerationType::SET_DATA_SUCESS)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }
}

void ProtocolProcessor::sendRequestSetCPUsFreqControlData(const LenovoLegionDaemon::CPUXFreqControl::DataControl &data)
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_SET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::CPUS_FREQ_CONTROL_DATA,
        .m_data = {
            .m_cpusFreqControlDataControl = data
        }
    });

    auto msg = receiveMessage();
    if (msg.m_type != LenovoLegionDaemon::Message::RESPONSE_SET_DATA &&
        msg.m_dataType != LenovoLegionDaemon::Message::ENUMERATION &&
        msg.m_data.m_enumType != LenovoLegionDaemon::Message::EnumerationType::SET_DATA_SUCESS)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }
}

void ProtocolProcessor::sendRequestSetCPUsControlData(const LenovoLegionDaemon::CPUXControl::DataControl &data)
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_SET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::CPUS_CONTROL_DATA,
        .m_data = {
            .m_cpusControlDataControl = data
        }
    });

    auto msg = receiveMessage();
    if (msg.m_type != LenovoLegionDaemon::Message::RESPONSE_SET_DATA &&
        msg.m_dataType != LenovoLegionDaemon::Message::ENUMERATION &&
        msg.m_data.m_enumType != LenovoLegionDaemon::Message::EnumerationType::SET_DATA_SUCESS)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }
}

void ProtocolProcessor::sendRequestSetCPUSMTControlData(const LenovoLegionDaemon::CPUSMTControl::DataControl &data)
{
    sendMessage(LenovoLegionDaemon::Message {
        .m_type = LenovoLegionDaemon::Message::Type::REQUEST_SET_DATA,
        .m_dataType = LenovoLegionDaemon::Message::DataType::CPUS_SMT_CONTROL_DATA,
        .m_data = {
            .m_cpusSmtControlDataControl = data
        }
    });

    auto msg = receiveMessage();
    if (msg.m_type != LenovoLegionDaemon::Message::RESPONSE_SET_DATA &&
        msg.m_dataType != LenovoLegionDaemon::Message::ENUMERATION &&
        msg.m_data.m_enumType != LenovoLegionDaemon::Message::EnumerationType::SET_DATA_SUCESS)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid response message");
    }
}

}
