// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "ProtocolProcessorNotifier.h"
#include "SysFsDriverLenovo.h"
#include "SysFsDriverACPIPlatformProfile.h"
#include "SysFsDriverPowerSuplyBattery0.h"
#include "SysFsDriverCPUXList.h"
#include "ProtocolParser.h"

#include <Core/LoggerHolder.h>

#include <QCoreApplication>

namespace LenovoLegionDaemon {

ProtocolProcessorNotifier::ProtocolProcessorNotifier(ControlDataProvider* dataProvider,QLocalSocket* clientSocket,QObject* parent) :
    ProtocolProcessorBase(clientSocket,parent),
    m_controlDataProvider(dataProvider)
{}

ProtocolProcessorNotifier::~ProtocolProcessorNotifier()
{
    LOG_D("ProtocolProcessorNotifier stopped !");

    ProtocolProcessorBase::stop();
}

void ProtocolProcessorNotifier::stop()
{
    LOG_D("ProtocolProcessorNotifier stopped !");

    ProtocolProcessorBase::stop();
}

void ProtocolProcessorNotifier::start()
{
    LOG_D("ProtocolProcessorNotifier started !");

    ProtocolProcessorBase::start();
}

void ProtocolProcessorNotifier::readyReadHandler()
{
    LOG_D("ProtocolProcessorNotifier: readyReadHandler");

    THROW_EXCEPTION(exception_T,ERROR_CODES::UNEXPECTED_MESSAGE,"ProtocolProcessorNotifier: Unexpected message !");
}

void ProtocolProcessorNotifier::kernelEventHandler(const LenovoLegionDaemon::SysFsDriver::SubsystemEvent &event)
{
    LOG_D("ProtocolProcessorNotifier: kernelEventHandler " + event.m_driverName + " " + QString::number(static_cast<int>(event.m_action)));

    if(event.m_driverName == SysFsDriverACPIPlatformProfile::DRIVER_NAME)
    {
        if(event.m_action == SysFsDriver::SubsystemEvent::Action::CHANGED)
        {
            m_clientSocket->write(
                ProtocolParser::parseMessage(Message {
                    .m_type = Message::NOTIFICATION,
                    .m_dataType  = Message::ENUMERATION,
                    .m_data      = {
                        .m_enumType = Message::EnumerationType::INF_ACPI_PLATFORM_PROFILE_CHANGE
                    }
                }));
        }
    }

    if(event.m_driverName == SysFsDriverPowerSuplyBattery0::DRIVER_NAME)
    {
        if(event.m_action == SysFsDriver::SubsystemEvent::Action::CHANGED)
        {
            m_clientSocket->write(
                ProtocolParser::parseMessage(Message {
                    .m_type = Message::NOTIFICATION,
                    .m_dataType  = Message::ENUMERATION,
                    .m_data      = {
                        .m_enumType = Message::EnumerationType::INF_POWER_SUPPLY_BATTERY0_CHANGE
                    }
                }));
        }
    }

    if(event.m_driverName == SysFsDriverCPUXList::DRIVER_NAME)
    {
        if(event.m_action == SysFsDriver::SubsystemEvent::Action::RELOADED)
        {
            m_clientSocket->write(
                ProtocolParser::parseMessage(Message {
                    .m_type = Message::NOTIFICATION,
                    .m_dataType  = Message::ENUMERATION,
                    .m_data      = {
                        .m_enumType = Message::EnumerationType::INF_CPU_X_LIST_RELOADED
                    }
                }));
        }
    }
}

void ProtocolProcessorNotifier::moduleSubsystemHandler(const LenovoLegionDaemon::SysFsDriverManager::ModuleSubsystemEvent &event)
{
    LOG_D("ProtocolProcessorNotifier: moduleSubsystemHandler " + event.m_driverName + " " + QString::number(static_cast<int>(event.m_action)));


    if(event.m_driverName == SysFsDriverLenovo::DRIVER_NAME)
    {
        if(event.m_action == SysFsDriverManager::ModuleSubsystemEvent::Action::REMOVE)
        {
            m_clientSocket->write(
            ProtocolParser::parseMessage(Message {
                .m_type = Message::NOTIFICATION,
                .m_dataType  = Message::ENUMERATION,
                .m_data      = {
                    .m_enumType = Message::EnumerationType::INF_LENOVO_DRIVER_REMOVED
                }
            }));
        }

        if(event.m_action == SysFsDriverManager::ModuleSubsystemEvent::Action::ADD)
        {
            m_clientSocket->write(
                ProtocolParser::parseMessage(Message {
                    .m_type = Message::NOTIFICATION,
                    .m_dataType  = Message::ENUMERATION,
                    .m_data      = {
                        .m_enumType = Message::EnumerationType::INF_LENOVO_DRIVER_ADDED
                    }
                }));
        }
    }
}

}
