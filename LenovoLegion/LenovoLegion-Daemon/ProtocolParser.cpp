// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "ProtocolParser.h"

#include <Core/LoggerHolder.h>

#include <thread>
namespace LenovoLegionDaemon {


void ProtocolParser::parseMessage(QLocalSocket &socket, std::function<void (const Message &)> callback)
{
    QByteArray header = readDataWithTimeout(socket,2);

    LOG_D(QString("Message header = (").append(header.toHex(':').toStdString()).append(")"));

    Message message;

    message.m_type =  (Message::Type)header[0];
    message.m_dataType = (Message::DataType)header[1];



    switch (message.m_dataType)
    {
        case Message::DataType::CPU_CONTROL_DATA:
        {
            parseMessage(socket,message.m_data.m_cpuControlData);
            break;
        }
        case Message::DataType::FAN_CURVE_CONTROL_DATA:
        {
            parseMessage(socket,message.m_data.m_fanCurveControlData);
            break;
        }
        case Message::DataType::FANCONTROL_DATA:
        {
            parseMessage(socket,message.m_data.m_fanControlData);
            break;
        }
        case Message::DataType::GPU_CONTROL_DATA:
        {
            parseMessage(socket,message.m_data.m_gpuControlData);
            break;
        }
        case Message::DataType::POWER_PROFILE_CONTROL_DATA:
        {
            parseMessage(socket,message.m_data.m_powerProfileControlData);
            break;
        }
        case Message::DataType::HW_MONITORING_DATA:
        {
            parseMessage(socket,message.m_data.m_hwMonitoringData);
            break;
        }
        case Message::DataType::TEXT:
        {
            parseMessage(socket,message.m_data.m_text);
            break;
        }
        case Message::DataType::ENUMERATION:
        {
            parseMessage(socket,message.m_data.m_enumType);
            break;
        }
        case Message::DataType::BATTERY_CONTROL_DATA:
        {
            parseMessage(socket,message.m_data.m_bateryControlData);
            break;
        }
        case Message::DataType::CPU_HETEROGENOUS_TOPOLOGY_DATA:
        {
            parseMessage(socket,message.m_data.m_cpuHeteregenousTopologyData);
            break;
        }
        case Message::DataType::CPUS_FREQ_CONTROL_DATA:
        {
            parseMessage(socket,message.m_data.m_cpusFreqControlData);
            break;
        }
        case Message::DataType::CPUS_CONTROL_DATA:
        {
            parseMessage(socket,message.m_data.m_cpusControlData);
            break;
        }
        case Message::DataType::CPUS_SMT_CONTROL_DATA:
        {
            parseMessage(socket,message.m_data.m_cpusSmtControlData);
            break;
        }
        case Message::DataType::CPU_HOMOGENOUS_TOPOLOGY_DATA:
        {
            parseMessage(socket,message.m_data.m_cpuHomogenousTopologyData);
            break;
        }
        default:
        {
            THROW_EXCEPTION(exception_T,ERROR_CODES::PROTOCOL_ERROR_DATA_TYPE,"Protocol error = unknown data type !")
        }
    }

    callback(message);
}

QByteArray ProtocolParser::parseMessage(const Message &message)
{
    QByteArray rawMessage;

    rawMessage.append(message.m_type);
    rawMessage.append(message.m_dataType);


    switch (message.m_dataType)
    {
    case Message::DataType::CPU_CONTROL_DATA:
    {
        rawMessage.append(reinterpret_cast<const char*>(&message.m_data.m_cpuControlData),sizeof(message.m_data.m_cpuControlData));
        break;
    }
    case Message::DataType::FAN_CURVE_CONTROL_DATA:
    {
        rawMessage.append(reinterpret_cast<const char*>(message.m_data.m_fanCurveControlData.m_data.m_points.data()),message.m_data.m_fanCurveControlData.m_data.m_points.size());
        rawMessage.append(static_cast<quint8>(message.m_data.m_fanCurveControlData.m_isAvailable));
        break;
    }
    case Message::DataType::FANCONTROL_DATA:
    {
        rawMessage.append(reinterpret_cast<const char*>(&message.m_data.m_fanControlData),sizeof(message.m_data.m_fanControlData));
        break;
    }
    case Message::DataType::GPU_CONTROL_DATA:
    {
        rawMessage.append(reinterpret_cast<const char*>(&message.m_data.m_gpuControlData),sizeof(message.m_data.m_gpuControlData));
        break;
    }
    case Message::DataType::POWER_PROFILE_CONTROL_DATA:
    {
        rawMessage.append(reinterpret_cast<const char*>(&message.m_data.m_powerProfileControlData),sizeof(message.m_data.m_powerProfileControlData));
        break;
    }
    case Message::DataType::HW_MONITORING_DATA:
    {
        rawMessage.append(reinterpret_cast<const char*>(&message.m_data.m_hwMonitoringData.m_lenovo),sizeof(message.m_data.m_hwMonitoringData.m_lenovo));
        rawMessage.append(reinterpret_cast<const char*>(&message.m_data.m_hwMonitoringData.m_intelPowerapRapl),sizeof(message.m_data.m_hwMonitoringData.m_intelPowerapRapl));
        rawMessage.append(reinterpret_cast<const char*>(message.m_data.m_hwMonitoringData.m_cpusFreq.data()),message.m_data.m_hwMonitoringData.m_cpusFreq.size() * sizeof(message.m_data.m_hwMonitoringData.m_cpusFreq[0]));
        break;
    }
    case Message::DataType::TEXT:
    {
        rawMessage.append(reinterpret_cast<const char*>(message.m_data.m_text.data()),message.m_data.m_text.size());
        break;
    }
    case Message::DataType::ENUMERATION:
    {
        rawMessage.append(static_cast<quint8>(message.m_data.m_enumType));
        break;
    }
    case Message::DataType::BATTERY_CONTROL_DATA:
    {
        rawMessage.append(reinterpret_cast<const char*>(&message.m_data.m_bateryControlData),sizeof(message.m_data.m_bateryControlData));
        break;
    }
    case Message::DataType::CPU_HETEROGENOUS_TOPOLOGY_DATA:
    {
        rawMessage.append(reinterpret_cast<const char*>(message.m_data.m_cpuHeteregenousTopologyData.m_data.m_ActiveCpusCore.data()),message.m_data.m_cpuHeteregenousTopologyData.m_data.m_ActiveCpusCore.size() * sizeof(message.m_data.m_cpuHeteregenousTopologyData.m_data.m_ActiveCpusCore[0]));
        rawMessage.append(reinterpret_cast<const char*>(message.m_data.m_cpuHeteregenousTopologyData.m_data.m_ActiveCpusAtom.data()),message.m_data.m_cpuHeteregenousTopologyData.m_data.m_ActiveCpusAtom.size() * sizeof(message.m_data.m_cpuHeteregenousTopologyData.m_data.m_ActiveCpusAtom[0]));
        rawMessage.append(static_cast<quint8>(message.m_data.m_cpuHeteregenousTopologyData.m_isAvailable));
        break;
    }
    case Message::DataType::CPUS_FREQ_CONTROL_DATA:
    {
        rawMessage.append(reinterpret_cast<const char*>(message.m_data.m_cpusFreqControlData.m_data.m_cpus.data()),message.m_data.m_cpusFreqControlData.m_data.m_cpus.size() * sizeof(message.m_data.m_cpusFreqControlData.m_data.m_cpus[0]));
        rawMessage.append(static_cast<quint8>(message.m_data.m_cpusFreqControlData.m_isAvailable));
        break;
    }
    case Message::DataType::CPUS_CONTROL_DATA:
    {
        rawMessage.append(reinterpret_cast<const char*>(message.m_data.m_cpusControlData.m_data.m_cpus.data()),message.m_data.m_cpusControlData.m_data.m_cpus.size() * sizeof(message.m_data.m_cpusControlData.m_data.m_cpus[0]));
        rawMessage.append(static_cast<quint8>(message.m_data.m_cpusControlData.m_isAvailable));
        break;
    }
    case Message::DataType::CPUS_SMT_CONTROL_DATA:
    {
        rawMessage.append(reinterpret_cast<const char*>(&message.m_data.m_cpusSmtControlData),sizeof(message.m_data.m_cpusSmtControlData));
        break;
    }
    case Message::DataType::CPU_HOMOGENOUS_TOPOLOGY_DATA:
    {
        rawMessage.append(reinterpret_cast<const char*>(message.m_data.m_cpuHomogenousTopologyData.m_data.m_ActiveCpus.data()),message.m_data.m_cpuHomogenousTopologyData.m_data.m_ActiveCpus.size() * sizeof(message.m_data.m_cpuHomogenousTopologyData.m_data.m_ActiveCpus[0]));
        rawMessage.append(static_cast<quint8>(message.m_data.m_cpuHomogenousTopologyData.m_isAvailable));
        break;
    }
    }

    return rawMessage;
}

QByteArray  ProtocolParser::readDataWithTimeout(QLocalSocket &socket, int size)
{
    QByteArray data;

    int atempt = 0;
    do {

        data.append(socket.read(size-data.size()));

        if(data.size() == size)
        {
            break;
        }

        if(atempt > 100)
        {
            THROW_EXCEPTION(exception_T,ERROR_CODES::DATA_NOT_READY,"Data not ready !");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        atempt++;

    } while(data.size() < size);


    return data;
}

void ProtocolParser::parseMessage(QLocalSocket &socket, HWMonitoring::DataInfo &message)
{
    QByteArray data = readDataWithTimeout(socket,sizeof(message.m_lenovo));

    memcpy(&message.m_lenovo,data.data(),sizeof(message.m_lenovo));

    data = readDataWithTimeout(socket,sizeof(message.m_intelPowerapRapl));

    memcpy(&message.m_intelPowerapRapl,data.data(),sizeof(message.m_intelPowerapRapl));

    data = readDataWithTimeout(socket,message.m_cpusFreq.size() * sizeof(message.m_cpusFreq[0]));

    memcpy(message.m_cpusFreq.data(),data.data(),message.m_cpusFreq.size() * sizeof(message.m_cpusFreq[0]));
}

void ProtocolParser::parseMessage(QLocalSocket &socket, FanControl::CurveControl::DataInfo &message)
{
    parseMessage(socket,message.m_data);

    QByteArray data = readDataWithTimeout(socket,1);
    message.m_isAvailable = data[0];
}

void ProtocolParser::parseMessage(QLocalSocket &socket, FanControl::CurveControl::DataControl &message)
{
    parseMessage(socket,message.m_data);
}

void ProtocolParser::parseMessage(QLocalSocket &socket, FanControl::CurveControl::Data &message)
{
    QByteArray data = readDataWithTimeout(socket,message.m_points.size());
    memcpy(&message.m_points,data.data(),message.m_points.size());
}

void ProtocolParser::parseMessage(QLocalSocket &socket, FanControl::Control::DataInfo &message)
{
    parseMessage(socket,message.m_data);

    QByteArray data = readDataWithTimeout(socket,1);
    message.m_isAvailable = data[0];
}

void ProtocolParser::parseMessage(QLocalSocket &socket, FanControl::Control::DataControl &message)
{
    parseMessage(socket,message.m_data);
}

void ProtocolParser::parseMessage(QLocalSocket &socket, FanControl::Control::Data &message)
{
    QByteArray data = readDataWithTimeout(socket,sizeof(message));
    memcpy(&message,data.data(),sizeof(message));
}

void ProtocolParser::parseMessage(QLocalSocket &socket, PowerControl::CPU::DataInfo &message)
{
    parseMessage(socket,message.m_data);

    QByteArray data = readDataWithTimeout(socket,1);
    message.m_isAvailable = data[0];
}

void ProtocolParser::parseMessage(QLocalSocket &socket, PowerControl::CPU::DataControl &message)
{
    parseMessage(socket,message.m_data);
}

void ProtocolParser::parseMessage(QLocalSocket &socket, PowerControl::CPU::Data &message)
{
    QByteArray data = readDataWithTimeout(socket,sizeof(message));
    memcpy(&message,data.data(),sizeof(message));
}


void ProtocolParser::parseMessage(QLocalSocket &socket, PowerControl::GPU::DataInfo &message)
{
    parseMessage(socket,message.m_data);

    QByteArray data = readDataWithTimeout(socket,1);
    message.m_isAvailable = data[0];
}

void ProtocolParser::parseMessage(QLocalSocket &socket, PowerControl::GPU::DataControl &message)
{
    parseMessage(socket,message.m_data);
}

void ProtocolParser::parseMessage(QLocalSocket &socket, PowerControl::GPU::Data &message)
{
    QByteArray data = readDataWithTimeout(socket,sizeof(message));
    memcpy(&message,data.data(),sizeof(message));
}

void ProtocolParser::parseMessage(QLocalSocket &socket, PowerProfile::Control::DataInfo &message)
{
    parseMessage(socket,message.m_data);
    QByteArray data = readDataWithTimeout(socket,1);
    message.m_isAvailable = data[0];
}

void ProtocolParser::parseMessage(QLocalSocket &socket, PowerProfile::Control::DataControl &message)
{
     parseMessage(socket,message.m_data);
}

void ProtocolParser::parseMessage(QLocalSocket &socket, PowerProfile::Control::Data &message)
{
    QByteArray data = readDataWithTimeout(socket,sizeof(message));
    memcpy(&message,data.data(),sizeof(message));
}

void ProtocolParser::parseMessage(QLocalSocket &socket, Text &message)
{
    QByteArray data = readDataWithTimeout(socket,message.size());

    std::copy(data.begin(), data.end(), message.begin());
}

void ProtocolParser::parseMessage(QLocalSocket &socket, Batery::DataInfo &message)
{
    QByteArray data = readDataWithTimeout(socket,sizeof(message));
    memcpy(&message,data.data(),sizeof(message));
}

void ProtocolParser::parseMessage(QLocalSocket &socket, Message::EnumerationType &message)
{
    QByteArray data = readDataWithTimeout(socket,1);

    message = static_cast<Message::EnumerationType>(data[0]);
}

void ProtocolParser::parseMessage(QLocalSocket &socket, CPUTopology::Heterogeneous::DataInfo &message)
{
    QByteArray data = readDataWithTimeout(socket,(message.m_data.m_ActiveCpusCore.size() + message.m_data.m_ActiveCpusAtom.size()) * sizeof(message.m_data.m_ActiveCpusAtom[0]));

    memcpy(message.m_data.m_ActiveCpusCore.data(),data.data(),message.m_data.m_ActiveCpusCore.size() * sizeof(message.m_data.m_ActiveCpusCore[0]));
    memcpy(message.m_data.m_ActiveCpusAtom.data(),data.data() + (message.m_data.m_ActiveCpusCore.size() * sizeof(message.m_data.m_ActiveCpusCore[0])),message.m_data.m_ActiveCpusAtom.size() * sizeof(message.m_data.m_ActiveCpusAtom[0]));

    data = readDataWithTimeout(socket,1);
    message.m_isAvailable = data[0];
}

void ProtocolParser::parseMessage(QLocalSocket &socket,CPUTopology::Homogeneous::DataInfo& message)
{
    QByteArray data = readDataWithTimeout(socket,message.m_data.m_ActiveCpus.size() * sizeof(message.m_data.m_ActiveCpus[0]));
    memcpy(message.m_data.m_ActiveCpus.data(),data.data(),message.m_data.m_ActiveCpus.size() * sizeof(message.m_data.m_ActiveCpus[0]));

    data = readDataWithTimeout(socket,1);
    message.m_isAvailable = data[0];
}

void ProtocolParser::parseMessage(QLocalSocket &socket, CPUXFreqControl::DataInfo &message)
{
    QByteArray data = readDataWithTimeout(socket,message.m_data.m_cpus.size() * sizeof(message.m_data.m_cpus[0]));

    memcpy(message.m_data.m_cpus.data(),data.data(),data.size());
    data = readDataWithTimeout(socket,1);
    message.m_isAvailable = data[0];
}

void ProtocolParser::parseMessage(QLocalSocket &socket, CPUXFreqControl::DataControl &message)
{
    QByteArray data = readDataWithTimeout(socket,message.m_data.m_cpus.size() * sizeof(message.m_data.m_cpus[0]));

    memcpy(message.m_data.m_cpus.data(),data.data(),data.size());
}

void ProtocolParser::parseMessage(QLocalSocket &socket, CPUXControl::DataInfo &message)
{
    QByteArray data = readDataWithTimeout(socket,message.m_data.m_cpus.size() * sizeof(message.m_data.m_cpus[0]));

    memcpy(message.m_data.m_cpus.data(),data.data(),data.size());

    data = readDataWithTimeout(socket,1);
    message.m_isAvailable = data[0];
}

void ProtocolParser::parseMessage(QLocalSocket &socket, CPUXControl::DataControl &message)
{
    QByteArray data = readDataWithTimeout(socket,message.m_data.m_cpus.size() * sizeof(message.m_data.m_cpus[0]));

    memcpy(message.m_data.m_cpus.data(),data.data(),data.size());
}

void ProtocolParser::parseMessage(QLocalSocket &socket, CPUSMTControl::DataInfo &message)
{
    QByteArray data = readDataWithTimeout(socket,sizeof(message));

    memcpy(&message,data.data(),sizeof(message));
}

void ProtocolParser::parseMessage(QLocalSocket &socket, CPUSMTControl::DataControl &message)
{
    QByteArray data = readDataWithTimeout(socket,sizeof(message));

    memcpy(&message,data.data(),sizeof(message));
}

}
