// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "ProtocolProcessorNotifier.h"

#include <ProtocolParser.h>

#include <Core/LoggerHolder.h>

#include <../LenovoLegion-Daemon/Application.h>

namespace LenovoLegionGui {

ProtocolProcessorNotifier::ProtocolProcessorNotifier(QObject *parent)
    : ProtocolProcessorBase(LenovoLegionDaemon::Application::SOCKET_NAME_NOTIFICATION,parent)
{
     m_timerId = startTimer(50);
}

ProtocolProcessorNotifier::~ProtocolProcessorNotifier()
{}


void ProtocolProcessorNotifier::timerEvent(QTimerEvent *event)
{

    if(m_timerId == event->timerId())
    {
        if(m_socket->bytesAvailable() > 0)
        {
            auto msg = receiveMessageDataReady();

            if(msg.m_type == LenovoLegionDaemon::Message::Type::NOTIFICATION &&  msg.m_dataType == LenovoLegionDaemon::Message::DataType::ENUMERATION)
            {
                if(msg.m_data.m_enumType == LenovoLegionDaemon::Message::EnumerationType::INF_LENOVO_DRIVER_ADDED)
                {
                    emit lenovoDriverAdded();
                    return;
                }

                if(msg.m_data.m_enumType == LenovoLegionDaemon::Message::EnumerationType::INF_LENOVO_DRIVER_REMOVED)
                {
                    emit lenovoDriverRemoved();
                    return;
                }

                if(msg.m_data.m_enumType == LenovoLegionDaemon::Message::EnumerationType::INF_ACPI_PLATFORM_PROFILE_CHANGE)
                {
                    emit powerProfileChanged();
                    return;
                }

                if(msg.m_data.m_enumType == LenovoLegionDaemon::Message::EnumerationType::INF_POWER_SUPPLY_BATTERY0_CHANGE)
                {
                    emit batteryStatusChanged();
                    return;
                }

                if(msg.m_data.m_enumType == LenovoLegionDaemon::Message::EnumerationType::INF_CPU_X_LIST_RELOADED)
                {
                    emit cpuXListReloaded();
                    return;
                }
            }

            THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_MESSAGE,"Invalid message");
        }
    }

    ProtocolProcessorBase::timerEvent(event);
}

}
