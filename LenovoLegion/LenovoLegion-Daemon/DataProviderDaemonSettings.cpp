// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "DataProviderDaemonSettings.h"
#include "DaemonSettingsManager.h"
#include "DataProviderManager.h"

#include <Core/LoggerHolder.h>

namespace LenovoLegionDaemon {

DataProviderDaemonSettings::DataProviderDaemonSettings(DataProviderManager* dataProviderManager) : 
    DataProvider(dataProviderManager, dataType),
    m_dataProviderManager(dataProviderManager)
{}

QByteArray DataProviderDaemonSettings::serializeAndGetData() const
{
    legion::messages::DaemonSettings daemonSettings;
    QByteArray byteArray;

    LOG_D(__PRETTY_FUNCTION__);

    // Get current daemon settings from singleton
    daemonSettings = DaemonSettingsManager::getInstance().getDaemonSettings();

    byteArray.resize(daemonSettings.ByteSizeLong());
    if(!daemonSettings.SerializeToArray(byteArray.data(), byteArray.size()))
    {
        THROW_EXCEPTION(exception_T, ERROR_CODES::SERIALIZE_ERROR, "Serialize of data message error !");
    }

    return byteArray;
}

QByteArray DataProviderDaemonSettings::deserializeAndSetData(const QByteArray& data)
{
    legion::messages::DaemonSettings newSettings;

    LOG_D(__PRETTY_FUNCTION__);

    if(!newSettings.ParseFromArray(data.data(), data.size()))
    {
        THROW_EXCEPTION(exception_T, DataProvider::ERROR_CODES::INVALID_DATA, "Parse of data message error !");
    }

    // Get old settings before updating to detect changes
    legion::messages::DaemonSettings oldSettings = DaemonSettingsManager::getInstance().getDaemonSettings();
    
    // Check if save_now command is set
    if(newSettings.has_save_now() && newSettings.save_now())
    {
        LOG_D("Save current configuration command received - saving all settings");
        DaemonSettingsManager::getInstance().saveAllSettings(m_dataProviderManager);
        
        // Reset the command flag so it doesn't persist
        newSettings.set_save_now(false);
    }
    
    // Check if debug_logging changed
    bool oldDebugLogging = oldSettings.debug_logging();
    bool newDebugLogging = newSettings.debug_logging();
    
    if(oldDebugLogging != newDebugLogging)
    {
        LOG_D(QString("Debug logging setting changed from ") + QString::number(oldDebugLogging) + " to " + QString::number(newDebugLogging));
        
        // Apply new logging level immediately
        bj::framework::Logger::SEVERITY_BITSET severity;
        if(newDebugLogging)
        {
            // Enable debug logging
            severity = bj::framework::Logger::SEVERITY_BITSET()
                     .set(bj::framework::Logger::ERROR)
                     .set(bj::framework::Logger::WARNING)
                     .set(bj::framework::Logger::INFO)
                     .set(bj::framework::Logger::DEBUG);
        }
        else
        {
            // Disable debug logging
            severity = bj::framework::Logger::SEVERITY_BITSET()
                     .set(bj::framework::Logger::ERROR)
                     .set(bj::framework::Logger::WARNING)
                     .set(bj::framework::Logger::INFO);
        }
        
        // Apply the new severity level
       LoggerHolder::getInstance().setSeverity(severity);
    }
    
    // Update daemon settings in singleton
    DaemonSettingsManager::getInstance().setDaemonSettings(newSettings);
    
    // Also save daemon settings to persistent storage
    DaemonSettingsManager::getInstance().saveDaemonSettings();

    return {};
}

void DataProviderDaemonSettings::init()
{
    LOG_D(__PRETTY_FUNCTION__);
}

void DataProviderDaemonSettings::clean()
{
    LOG_D(__PRETTY_FUNCTION__);
}

}
