// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <QSettings>
#include <QObject>
#include <QStringList>


#include "../LenovoLegion-PrepareBuild/PowerProfile.pb.h"
#include "../LenovoLegion-PrepareBuild/CPUOptions.pb.h"
#include "../LenovoLegion-PrepareBuild/CPUFrequency.pb.h"
#include "../LenovoLegion-PrepareBuild/FanControl.pb.h"
#include "../LenovoLegion-PrepareBuild/CpuPower.pb.h"
#include "../LenovoLegion-PrepareBuild/GPUPower.pb.h"
#include "../LenovoLegion-PrepareBuild/CpuPowerRapl.pb.h"
#include "../LenovoLegion-PrepareBuild/NvidiaNvml.pb.h"
#include "../LenovoLegion-PrepareBuild/CpuIntelMSR.pb.h"
#include "../LenovoLegion-PrepareBuild/Other.pb.h"

namespace LenovoLegionGui {


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


class ApplicationSettings : public QObject, protected Settings
{
    Q_OBJECT

public:
    enum class SettingType {
        StartMinimized,
        MinimizeToTray,
        AppDebugLogging,
        SaveSettingsOnDaemonExit,
        DaemonDebugLogging
    };
    Q_ENUM(SettingType)

    explicit ApplicationSettings(QObject* parent = nullptr);

    // Singleton accessor
    static ApplicationSettings* instance();

    // Load settings
    ApplicationSettings& loadStartMinimized(bool& value);
    ApplicationSettings& loadMinimizeToTray(bool& value);
    ApplicationSettings& loadAppDebugLogging(bool& value);
    ApplicationSettings& loadSaveSettingsOnDaemonExit(bool& value);
    ApplicationSettings& loadDaemonDebugLogging(bool& value);

    // Save settings
    ApplicationSettings& saveStartMinimized(bool value);
    ApplicationSettings& saveMinimizeToTray(bool value);
    ApplicationSettings& saveAppDebugLogging(bool value);
    ApplicationSettings& saveSaveSettingsOnDaemonExit(bool value);
    ApplicationSettings& saveDaemonDebugLogging(bool value);

    // Reset to defaults
    void resetToDefaults();

signals:
    void settingChanged(ApplicationSettings::SettingType setting, bool value);
};


class ProfileSettings : protected Settings
{
public:
    explicit ProfileSettings(const QString& profileName);
    
    // Profile metadata
    void saveDescription(const QString& description);
    QString loadDescription();
    
    // Generic save/load methods for protobuf messages
    void savePowerProfile(const legion::messages::PowerProfile& msg);
    void saveCPUOptions(const legion::messages::CPUOptions& msg);
    void saveCPUFrequency(const legion::messages::CPUFrequency& msg);
    void saveFanCurve(const legion::messages::FanCurve& msg);
    void saveFanOption(const legion::messages::FanOption& msg);
    void saveCPUSMT(const legion::messages::CPUSMT& msg);
    void saveCPUPower(const legion::messages::CPUPower& msg);
    void saveGPUPower(const legion::messages::GPUPower& msg);
    void saveCPUPowerRapl(const legion::messages::CPUPowerRapl& msg);
    void saveNvidiaNvml(const legion::messages::NvidiaNvml& msg);
    void saveIntelMSR(const legion::messages::CpuIntelMSR& msg);
    void saveOther(const legion::messages::OtherSettings& msg);
    
    void loadPowerProfile(legion::messages::PowerProfile& msg);
    void loadCPUOptions(legion::messages::CPUOptions& msg);
    void loadCPUFrequency(legion::messages::CPUFrequency& msg);
    void loadFanCurve(legion::messages::FanCurve& msg);
    void loadFanOption(legion::messages::FanOption& msg);
    void loadCPUSMT(legion::messages::CPUSMT& msg);
    void loadCPUPower(legion::messages::CPUPower& msg);
    void loadGPUPower(legion::messages::GPUPower& msg);
    void loadCPUPowerRapl(legion::messages::CPUPowerRapl& msg);
    void loadNvidiaNvml(legion::messages::NvidiaNvml& msg);
    void loadIntelMSR(legion::messages::CpuIntelMSR& msg);
    void loadOther(legion::messages::OtherSettings& msg);
};


class ProfileManager
{
public:
    static QStringList listProfiles();
    static bool profileExists(const QString& profileName);
    static bool deleteProfile(const QString& profileName);
    static QString getProfileDescription(const QString& profileName);
};


}
