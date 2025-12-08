// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "SysFSDriverHWMon.h"

#include <Core/LoggerHolder.h>

#include <QFile>
#include <QTextStream>


namespace LenovoLegionDaemon {


SysFSDriverHWMon::SysFSDriverHWMon(QObject *parrent) : SysFsDriver(DRIVER_NAME,"/sys/class/hwmon/",{},parrent,MODULE_NAME)
{}

void SysFSDriverHWMon::init()
{
    LOG_D(__PRETTY_FUNCTION__);

    clean();

    for(const auto& entry : std::filesystem::directory_iterator(std::filesystem::path(m_path)))
    {

        std::filesystem::path  pathToName = std::filesystem::path(entry.path()).append(std::string("name"));

        if(std::filesystem::exists(pathToName))
        {
            QFile file(pathToName);

            if(!file.open(QIODeviceBase::ReadOnly))
            {
                continue;
            }

            QString driverName = QTextStream(&file).readAll();


            if(driverName.trimmed() == "legion")
            {
                LOG_D(QString("Found Legion HWMon driver in path: ") + entry.path().c_str());


                m_descriptor["fan1Speed"] = std::filesystem::path(entry).append("fan1_input");
                m_descriptor["fan1Label"] = std::filesystem::path(entry).append("fan1_label");
                m_descriptor["fan1MaxSpeed"] = std::filesystem::path(entry).append("fan1_max");
                m_descriptor["fan1MinSpeed"] = std::filesystem::path(entry).append("fan1_min");


                m_descriptor["fan2Speed"] = std::filesystem::path(entry).append("fan2_input");
                m_descriptor["fan2Label"] = std::filesystem::path(entry).append("fan2_label");
                m_descriptor["fan2MaxSpeed"] = std::filesystem::path(entry).append("fan2_max");
                m_descriptor["fan2MinSpeed"] = std::filesystem::path(entry).append("fan2_min");

                m_descriptor["temp1"] = std::filesystem::path(entry).append("temp1_input");
                m_descriptor["temp1Label"] = std::filesystem::path(entry).append("temp1_label");

                m_descriptor["temp2"] = std::filesystem::path(entry).append("temp2_input");
                m_descriptor["temp2Label"] = std::filesystem::path(entry).append("temp2_label");

            }
        }

    }
}

}
