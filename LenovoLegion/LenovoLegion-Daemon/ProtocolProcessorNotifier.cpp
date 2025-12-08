// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "ProtocolProcessorNotifier.h"
#include "ProtocolParser.h"
#include "Computer.h"
#include "SysFsDriverManager.h"

#include <Core/LoggerHolder.h>

#include "SysFsDriverACPIPlatformProfile.h"
#include "SysFsDriverPowerSuplyBattery0.h"
#include "SysFsDriverCPUXList.h"
#include "SysFsDriverLegion.h"
#include "SysFsDriverLegionEvents.h"


#include "../LenovoLegion-PrepareBuild/Notification.pb.h"

#include <QCoreApplication>

namespace LenovoLegionDaemon {

ProtocolProcessorNotifier::ProtocolProcessorNotifier(SysFsDriverManager* sysFsDriverManager,QLocalSocket* clientSocket, QObject* parent) :
    ProtocolProcessorBase(clientSocket,parent),
    m_sysfsDriverManager(sysFsDriverManager)
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
    LOG_D("ProtocolProcessorNotifier readyReadHandler");

    THROW_EXCEPTION(exception_T,ERROR_CODES::UNEXPECTED_MESSAGE,"ProtocolProcessorNotifier: Unexpected message !");
}

void ProtocolProcessorNotifier::disconnectedHandler()
{
    LOG_D("ProtocolProcessorNotifier client disconnected !");
    ProtocolProcessorBase::stop();
    ProtocolProcessorBase::waitForExit();

    emit clientDisconnected();
}

void ProtocolProcessorNotifier::kernelEventHandler(const LenovoLegionDaemon::SysFsDriver::SubsystemEvent &event)
{
    LOG_D("ProtocolProcessorNotifier: kernelEventHandler m_driverName=" + event.m_driverName + ", event.m_action=" + QString::number(static_cast<int>(event.m_action)) + ", m_DriverSpecificAction=" + QString(event.m_DriverSpecificEventType.data()) + ", m_DriverSpecificValue=" + event.m_DriverSpecificEventValue.data());

    if(!isRunning())
    {
        LOG_D("ProtocolProcessorNotifier is not running, ignoring kernel event !");
        return;
    }

    legion::messages::Notification msg;



    if(event.m_driverName == SysFsDriverACPIPlatformProfile::DRIVER_NAME)
    {
        if(event.m_action == SysFsDriver::SubsystemEvent::Action::CHANGED)
        {
            msg.set_action(legion::messages::Notification::ACPI_PLATFORM_PROFILE_CHANGE);
        }
    }

    if(event.m_driverName == SysFsDriverPowerSuplyBattery0::DRIVER_NAME)
    {
        if(event.m_action == SysFsDriver::SubsystemEvent::Action::CHANGED)
        {
            msg.set_action(legion::messages::Notification::POWER_SUPPLY_BATTERY0_CHANGE);
        }
    }

    if(event.m_driverName == SysFsDriverCPUXList::DRIVER_NAME)
    {
        if(event.m_action == SysFsDriver::SubsystemEvent::Action::RELOADED)
        {
            msg.set_action(legion::messages::Notification::CPU_X_LIST_RELOADED);
        }
    }

    if(event.m_driverName == SysFsDriverLegionEvents::DRIVER_NAME)
    {
        if(event.m_action == SysFsDriver::SubsystemEvent::Action::CHANGED)
        {
            if(static_cast<SysFsDriverLegionEvents::LegionVmiEventType>(QString(event.m_DriverSpecificEventType.data()).toInt()) == SysFsDriverLegionEvents::LegionVmiEventType::LEGION_WMI_EVENT_KEYLOCK_STATUS)
            {
                msg.set_action(legion::messages::Notification::KEYLOCK_STATUS_CHANGE);
            }
        }
    }


    if(msg.has_action())
    {
        QByteArray data;

        data.resize(msg.ByteSizeLong());

        if(!msg.SerializeToArray(data.data(),data.size()))
        {
            THROW_EXCEPTION(exception_T,ERROR_CODES::SERIALIZE_ERROR,"Serialize of data message error !");
        }


        m_clientSocket->write(ProtocolParser::parseMessage(MessageHeader{
            .m_type         = MessageHeader::NOTIFICATION,
            .m_dataType     = m_dataType,
            .m_dataLength   = data.length()
        },data));

        LOG_D("ProtocolProcessorNotifier: Notification sent !");
    }
}

void ProtocolProcessorNotifier::moduleSubsystemHandler(const LenovoLegionDaemon::SysFsDriverManager::ModuleSubsystemEvent &event)
{
    LOG_D("ProtocolProcessorNotifier: moduleSubsystemHandler " + event.m_moduleName + " " + QString::number(static_cast<int>(event.m_action)));

    legion::messages::Notification msg;


    if(!isRunning())
    {
        LOG_D("ProtocolProcessorNotifier is not running, ignoring module subsystem event !");
        return;
    }


    if(event.m_moduleName == LEGION_MODULE_NAME)
    {
        if(event.m_action == SysFsDriverManager::ModuleSubsystemEvent::Action::REMOVE)
        {
            msg.set_action(legion::messages::Notification::LENOVO_DRIVER_REMOVED);
        }

        if(event.m_action == SysFsDriverManager::ModuleSubsystemEvent::Action::ADD)
        {
            Computer(*m_sysfsDriverManager,this).initComputer();

            msg.set_action(legion::messages::Notification::LENOVO_DRIVER_ADDED);
        }
    }

    if(msg.has_action())
    {
        QByteArray data;

        data.resize(msg.ByteSizeLong());

        if(!msg.SerializeToArray(data.data(),data.size()))
        {
            THROW_EXCEPTION(exception_T,ERROR_CODES::SERIALIZE_ERROR,"Serialize of data message error !");
        }


        m_clientSocket->write(ProtocolParser::parseMessage(MessageHeader{
                                                               .m_type         = MessageHeader::NOTIFICATION,
                                                               .m_dataType     = m_dataType,
                                                               .m_dataLength   = data.length()
                                                           },data));
    }
}

}
