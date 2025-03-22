// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "ProtocolProcessor.h"
#include "ProtocolParser.h"

#include <Core/LoggerHolder.h>

#include <QCoreApplication>

namespace LenovoLegionDaemon {

ProtocolProcessor::ProtocolProcessor(ControlDataProvider* dataProvider,QLocalSocket* clientSocket,QObject* parent) :
    ProtocolProcessorBase(clientSocket,parent),
    m_controlDataProvider(dataProvider)
{}

ProtocolProcessor::~ProtocolProcessor()
{
}

void ProtocolProcessor::stop()
{
    LOG_D("ProtocolProcessor stopped !");

    ProtocolProcessorBase::stop();
}

void ProtocolProcessor::start()
{
    LOG_D("ProtocolProcessor started !");

    ProtocolProcessorBase::start();
}

void ProtocolProcessor::readyReadHandler()
{

    ProtocolParser::parseMessage(*m_clientSocket,[this](const Message& msg){

        LOG_D(QString("Message received: type=").append(QString::number(msg.m_type)).append(", dataType=").append(QString::number(msg.m_dataType)));

        if(msg.m_type == Message::REQUEST_GET_DATA && msg.m_dataType == Message::ENUMERATION)
        {

            if(msg.m_data.m_enumType == Message::EnumerationType::GET_POWER_PROFILE_CONTROL_DATA)
            {
                m_clientSocket->write(
                    ProtocolParser::parseMessage(Message {
                        .m_type = Message::RESPONSE_GET_DATA,
                        .m_dataType  = Message::POWER_PROFILE_CONTROL_DATA,
                        .m_data      = {
                                   .m_powerProfileControlData =   m_controlDataProvider->getPowerProfileData()
                               }
                    })
                    );
            }
            else if(msg.m_data.m_enumType == Message::EnumerationType::GET_CPU_CONTROL_DATA)
            {
                m_clientSocket->write(
                    ProtocolParser::parseMessage(Message {
                        .m_type = Message::RESPONSE_GET_DATA,
                        .m_dataType  = Message::CPU_CONTROL_DATA,
                        .m_data      = {
                                   .m_cpuControlData =   m_controlDataProvider->getCpuControlData()
                               }
                    })
                    );
            }
            else if(msg.m_data.m_enumType == Message::EnumerationType::GET_GPU_CONTROL_DATA)
            {
                m_clientSocket->write(
                    ProtocolParser::parseMessage(Message {
                        .m_type = Message::RESPONSE_GET_DATA,
                        .m_dataType  = Message::GPU_CONTROL_DATA,
                        .m_data      = {
                                   .m_gpuControlData =   m_controlDataProvider->getGpuControlData()
                               }
                    })
                    );
            }
            else if(msg.m_data.m_enumType == Message::EnumerationType::GET_FANCONTROL_DATA)
            {
                m_clientSocket->write(
                    ProtocolParser::parseMessage(Message {
                        .m_type = Message::RESPONSE_GET_DATA,
                        .m_dataType  = Message::FANCONTROL_DATA,
                        .m_data      = {
                                   .m_fanControlData =   m_controlDataProvider->getFanControlData()
                               }
                    })
                    );
            }
            else if(msg.m_data.m_enumType == Message::EnumerationType::GET_FAN_CURVE_CONTROL_DATA)
            {
                m_clientSocket->write(
                    ProtocolParser::parseMessage(Message {
                        .m_type = Message::RESPONSE_GET_DATA,
                        .m_dataType  = Message::FAN_CURVE_CONTROL_DATA,
                        .m_data      = {
                                   .m_fanCurveControlData =   m_controlDataProvider->getFanCurveControlData()
                               }
                    })
                    );
            }
            else if(msg.m_data.m_enumType == Message::EnumerationType::GET_HW_MONITORING_DATA)
            {
                m_clientSocket->write(
                    ProtocolParser::parseMessage(Message {
                        .m_type = Message::RESPONSE_GET_DATA,
                        .m_dataType  = Message::HW_MONITORING_DATA,
                        .m_data      = {
                                   .m_hwMonitoringData =   m_controlDataProvider->getHWMonitoringData()
                               }
                    })
                    );
            }
            else if(msg.m_data.m_enumType == Message::EnumerationType::GET_BATTERY_CONTROL_DATA)
            {
                m_clientSocket->write(
                    ProtocolParser::parseMessage(Message {
                        .m_type = Message::RESPONSE_GET_DATA,
                        .m_dataType  = Message::BATTERY_CONTROL_DATA,
                        .m_data      = {
                                   .m_bateryControlData =   m_controlDataProvider->getBateryControlData()
                               }
                    })
                    );
            }
            else if (msg.m_data.m_enumType == Message::EnumerationType::GET_CPU_HETEROGENOUS_TOPOLOGY_DATA)
            {
                m_clientSocket->write(
                    ProtocolParser::parseMessage(Message {
                        .m_type = Message::RESPONSE_GET_DATA,
                        .m_dataType  = Message::CPU_HETEROGENOUS_TOPOLOGY_DATA,
                        .m_data      = {
                                   .m_cpuHeteregenousTopologyData =   m_controlDataProvider->getCPUTopologyHeterogenousData()
                               }
                    })
                    );
            }
            else if(msg.m_data.m_enumType == Message::EnumerationType::GET_CPUS_FREQ_CONTROL_DATA)
            {
                m_clientSocket->write(
                    ProtocolParser::parseMessage(Message {
                        .m_type = Message::RESPONSE_GET_DATA,
                        .m_dataType  = Message::CPUS_FREQ_CONTROL_DATA,
                        .m_data      = {
                                   .m_cpusFreqControlData =   m_controlDataProvider->getCPUsInfoData()
                               }
                    })
                    );
            }
            else if(msg.m_data.m_enumType == Message::EnumerationType::GET_CPUS_CONTROL_DATA)
            {
                m_clientSocket->write(
                    ProtocolParser::parseMessage(Message {
                        .m_type = Message::RESPONSE_GET_DATA,
                        .m_dataType  = Message::CPUS_CONTROL_DATA,
                        .m_data      = {
                                   .m_cpusControlData =   m_controlDataProvider->getCPUsInfoControlData()
                               }
                    })
                    );
            }
            else if(msg.m_data.m_enumType == Message::GET_CPU_SMT_CONTROL_DATA)
            {
                m_clientSocket->write(
                    ProtocolParser::parseMessage(Message {
                        .m_type = Message::RESPONSE_GET_DATA,
                        .m_dataType  = Message::CPUS_SMT_CONTROL_DATA,
                        .m_data      = {
                                   .m_cpusSmtControlData =   m_controlDataProvider->getCPUSMTControlData()
                               }
                    })
                    );
            }
            else if(msg.m_data.m_enumType == Message::EnumerationType::GET_CPU_HOMOGENOUS_TOPOLOGY_DATA)
            {
                m_clientSocket->write(
                    ProtocolParser::parseMessage(Message {
                        .m_type = Message::RESPONSE_GET_DATA,
                        .m_dataType  = Message::CPU_HOMOGENOUS_TOPOLOGY_DATA,
                        .m_data      = {
                                   .m_cpuHomogenousTopologyData =   m_controlDataProvider->getCPUTopologyHomogenousData()
                               }
                    })
                    );
            }
        }
        else if(msg.m_type == Message::REQUEST_SET_DATA)
        {
            if(msg.m_dataType == Message::CPU_CONTROL_DATA)
            {
                m_controlDataProvider->setCpuControlData(msg.m_data.m_cpuControlDataControl);
            }
            else if(msg.m_dataType == Message::GPU_CONTROL_DATA)
            {
                m_controlDataProvider->setGpuControlData(msg.m_data.m_gpuControlDataControl);
            }
            else if(msg.m_dataType == Message::POWER_PROFILE_CONTROL_DATA)
            {
                m_controlDataProvider->setPowerProfileData(msg.m_data.m_powerProfileControlDataControl);
            }
            else if(msg.m_dataType == Message::FANCONTROL_DATA)
            {
                m_controlDataProvider->setFanControlData(msg.m_data.m_fanControlDataControl);
            }
            else if(msg.m_dataType == Message::FAN_CURVE_CONTROL_DATA)
            {
                m_controlDataProvider->setFanCurveControlData(msg.m_data.m_fanCurveControlDataControl);
            }
            else if(msg.m_dataType == Message::CPUS_FREQ_CONTROL_DATA)
            {
                m_controlDataProvider->setCpuFreqControlData(msg.m_data.m_cpusFreqControlDataControl);
            }
            else if(msg.m_dataType == Message::CPUS_CONTROL_DATA)
            {
                m_controlDataProvider->setCpuControlData(msg.m_data.m_cpusControlDataControl);
            }
            else if(msg.m_dataType == Message::CPUS_SMT_CONTROL_DATA)
            {
                m_controlDataProvider->setCPUSMTControlData(msg.m_data.m_cpusSmtControlDataControl);
            }
            m_clientSocket->write(
                ProtocolParser::parseMessage(Message {
                    .m_type = Message::RESPONSE_SET_DATA,
                    .m_dataType  = Message::ENUMERATION,
                    .m_data      = {.m_enumType = Message::EnumerationType::SET_DATA_SUCESS}
                })
                );
        }

        LOG_D(QString("Message received: done !"));
    });
}

}
