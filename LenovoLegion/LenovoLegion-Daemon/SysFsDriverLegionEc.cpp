// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "SysFsDriverLegionEc.h"

#include <Core/LoggerHolder.h>

#include <QFile>
#include <QTextStream>

namespace LenovoLegionDaemon {

SysFsDriverLegionEc::SysFsDriverLegionEc(QObject *parrent): SysFsDriver(DRIVER_NAME,"/sys/class/legion-firmware-attributes/legion-ec-0/attributes/",{},parrent,MODULE_NAME)
{}

void SysFsDriverLegionEc::init()
{
    LOG_D(__PRETTY_FUNCTION__);

    clean();

    std::filesystem::path path = std::filesystem::path(m_path).append("fan_control");
    if(std::filesystem::exists(path))
    {
        LOG_D(QString("Found Legion EC fan control driver in path: ") + path.c_str());

        m_descriptor["fan_control_full_speed"] = std::filesystem::path(path).append("full_speed");
        m_descriptor["fan_control_lock"] = std::filesystem::path(path).append("lock");
        m_descriptor["fan_control_display_name"] = std::filesystem::path(path).append("display_name");
        m_descriptor["fan_control_min_curve"] = std::filesystem::path(path).append("min_curve");
    }
}

}
