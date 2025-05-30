// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include <Settings.h>


#include <Core/Application.h>
namespace  LenovoLegionDaemon {

Settings::Settings(const QString &group):
    m_settings(QCoreApplication::applicationDirPath().append(QDir::separator()).append(bj::framework::Application::data_dir).append(QDir::separator()).append("/LenovoLegion-Daemon.ini"),QSettings::IniFormat)
{
    m_settings.setValue("LenovoLegion","LenovoLegion-Daemon");
    m_settings.beginGroup(group);
}

Settings::~Settings()
{
    m_settings.endGroup();
}

SettingsLoaderPowerProfiles::SettingsLoaderPowerProfiles() :
    Settings("PowerProfileData")
{}

SettingsLoaderPowerProfiles &SettingsLoaderPowerProfiles::loadPowerProfile(LenovoLegionDaemon::PowerProfile::Control::DataControl &profile)
{
    profile.m_data.m_profile = static_cast<LenovoLegionDaemon::PowerProfile::Control::PowerProfiles>(m_settings.value("PowerProfile").toUInt());
    return *this;
}

bool SettingsLoaderPowerProfiles::isCorrectDefined() const
{
    return m_settings.value("PowerProfile").isValid();
}

SettingsSaverPowerProfiles::SettingsSaverPowerProfiles() :
    Settings("PowerProfileData")
{}

SettingsSaverPowerProfiles &SettingsSaverPowerProfiles::saverPowerProfile(const LenovoLegionDaemon::PowerProfile::Control::DataControl &profile)
{
    m_settings.setValue("PowerProfile",static_cast<quint32>(profile.m_data.m_profile));
    return *this;
}

SettingsLoaderCPUControlData::SettingsLoaderCPUControlData() :
    Settings("CPUsControlData")
{

}

SettingsLoaderCPUControlData &SettingsLoaderCPUControlData::loadPowerProfile(CPUXControl::DataControl &profile)
{
    size_t size = m_settings.beginReadArray("CPUControl");
    for (size_t i = 0; i < std::min(size,profile.m_data.m_cpus.size()); ++i) {
        std::memset(profile.m_data.m_cpus.at(i).m_Governor,0,sizeof(profile.m_data.m_cpus.at(i).m_Governor) * sizeof(profile.m_data.m_cpus.at(i).m_Governor[0]));

        m_settings.setArrayIndex(i);

        profile.m_data.m_cpus.at(i).m_cpuOnline =  m_settings.value("online").toBool();
        std::strncpy(profile.m_data.m_cpus.at(i).m_Governor,m_settings.value("governor").toString().toStdString().c_str(),sizeof(profile.m_data.m_cpus.at(i).m_Governor) * sizeof(profile.m_data.m_cpus.at(i).m_Governor[0]) - 1);
    }
    m_settings.endArray();
    return *this;
}

bool SettingsLoaderCPUControlData::isCorrectDefined()
{
    size_t size = m_settings.beginReadArray("CPUControl");
    m_settings.endArray();

    return size == CPUXControl::DataControl().m_data.m_cpus.size();
}

SettingsSaverCPUControlData::SettingsSaverCPUControlData() :
    Settings("CPUsControlData")
{

}

SettingsSaverCPUControlData &SettingsSaverCPUControlData::saverPowerProfile(const CPUXControl::DataControl &profile)
{
    m_settings.beginWriteArray("CPUControl");
    for (size_t i = 0; i < profile.m_data.m_cpus.size(); ++i) {
        m_settings.setArrayIndex(i);
        m_settings.setValue("online",profile.m_data.m_cpus[i].m_cpuOnline);
        m_settings.setValue("governor",QString(profile.m_data.m_cpus[i].m_Governor).trimmed());
    }
    m_settings.endArray();
    return *this;
}

}
