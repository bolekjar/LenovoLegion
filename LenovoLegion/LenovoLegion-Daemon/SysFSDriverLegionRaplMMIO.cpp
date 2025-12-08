#include "SysFSDriverLegionRaplMMIO.h"

#include <Core/LoggerHolder.h>

namespace LenovoLegionDaemon {

SysFSDriverLegionRaplMMIO::SysFSDriverLegionRaplMMIO(QObject * parrent) : SysFsDriver(DRIVER_NAME,"/sys/class/legion-rapl-mmio/rapl-mmio-0/",{"wmi",{}},parrent,MODULE_NAME)
{}

void SysFSDriverLegionRaplMMIO::init()
{
    LOG_D(__PRETTY_FUNCTION__);

    clean();

    if(std::filesystem::exists(m_path))
    {
        LOG_D(QString("Legion Rapl MMIO driver found in path: ") + m_path.c_str());

        m_descriptor["is_locked"] = std::filesystem::path(m_path).append("is_locked");
        m_descriptor["is_pl4_locked"] = std::filesystem::path(m_path).append("is_pl4_locked");

        m_descriptor["pl1_current_value"] = std::filesystem::path(m_path).append("pl1_current_value");
        m_descriptor["pl2_current_value"] = std::filesystem::path(m_path).append("pl2_current_value");
        m_descriptor["pl4_current_value"] = std::filesystem::path(m_path).append("pl4_current_value");

        m_descriptor["set_and_lock"] = std::filesystem::path(m_path).append("set_and_lock");
        m_descriptor["set_pl4_and_lock"] = std::filesystem::path(m_path).append("set_pl4_and_lock");
    }
}

}

