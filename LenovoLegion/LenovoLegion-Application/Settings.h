// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <QSettings>

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


/*
class SettingsLoaderTCPClient: protected Settings
{

public:

    explicit SettingsLoaderTCPClient();

    SettingsLoaderTCPClient& loadPort(int& port);
    SettingsLoaderTCPClient& loadIPAdress(QString& ipAddress);

   bool isCorrectDefined() const;
};*/

}
