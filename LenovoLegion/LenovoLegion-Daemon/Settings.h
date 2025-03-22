// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include "ControlData.h"

#include <QSettings>


namespace LenovoLegionDaemon {


class Settings {

protected:


public:

    explicit Settings(const QString& group);
    virtual ~Settings();

    Settings(Settings&)  = delete;
    Settings(Settings&&) = delete;

    Settings& operator=(Settings&)  = delete;
    Settings& operator=(Settings&&) = delete;


protected:

    QSettings m_settings;
};


class SettingsLoaderPowerProfiles: protected Settings
{

public:

    explicit SettingsLoaderPowerProfiles();

    SettingsLoaderPowerProfiles& loadPowerProfile(LenovoLegionDaemon::PowerProfile::Control::DataControl& profile);

    bool isCorrectDefined() const;
};


class SettingsSaverPowerProfiles: protected Settings
{

public:

    explicit SettingsSaverPowerProfiles();

    SettingsSaverPowerProfiles& saverPowerProfile(const LenovoLegionDaemon::PowerProfile::Control::DataControl& profile);
};




class SettingsLoaderCPUControlData: protected Settings
{
public:

    explicit SettingsLoaderCPUControlData();

    SettingsLoaderCPUControlData& loadPowerProfile(CPUXControl::DataControl& profile);

    bool isCorrectDefined();
};

class SettingsSaverCPUControlData: protected Settings
{

public:

    explicit SettingsSaverCPUControlData();

    SettingsSaverCPUControlData& saverPowerProfile(const CPUXControl::DataControl &profile);
};


}
