// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include <Settings.h>


#include <Core/Application.h>

namespace  LenovoLegionGui {

Settings::Settings(const QString &group):
    m_settings(QString(bj::framework::Application::data_dir).append("/LenovoLegionGui.ini"),QSettings::IniFormat)
{
    m_settings.setValue("LenovoLegion","LenovoLegionGui");
    m_settings.beginGroup(group);
}

Settings::~Settings()
{
    m_settings.endGroup();
}

/*
SettingsLoaderTCPClient::SettingsLoaderTCPClient() :
    Settings("TCPClient")
{}

SettingsLoaderTCPClient &SettingsLoaderTCPClient::loadPort(int &port)
{
    port = m_settings.value("Port").toInt();

    return *this;
}

SettingsLoaderTCPClient &SettingsLoaderTCPClient::loadIPAdress(QString &ipAddress)
{
    ipAddress = m_settings.value("IPAddress").toString();

    return *this;
}


bool SettingsLoaderTCPClient::isCorrectDefined() const {
    return m_settings.value("Port").isValid() &&
           m_settings.value("IPAddress").isValid();
}*/

}
