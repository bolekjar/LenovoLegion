// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "SysFsDriverIntelPowercapRapl.h"

#include <Core/LoggerHolder.h>

namespace LenovoLegionDaemon {


SysFsDriverIntelPowercapRapl::SysFsDriverIntelPowercapRapl(QObject* parrent) : SysFsDriver(DRIVER_NAME,"/sys/class/powercap/intel-rapl:0/",{"powercap",{}},parrent) {}

void SysFsDriverIntelPowercapRapl::init()
{
    LOG_D(__PRETTY_FUNCTION__);

    clean();

    if(std::filesystem::exists(std::filesystem::path(m_path)))
    {
        LOG_D(QString("Found Intel Powercap RAPL driver in path: ") + m_path.c_str());


        m_descriptor["ltp_max_power_uw"] = std::filesystem::path(m_path).append("constraint_0_max_power_uw");
        m_descriptor["ltp_time_window_us"] = std::filesystem::path(m_path).append("constraint_0_time_window_us");
        m_descriptor["ltp_name"] = std::filesystem::path(m_path).append("constraint_0_name");
        m_descriptor["ltp_power_limit_uw"] = std::filesystem::path(m_path).append("constraint_0_power_limit_uw");

        m_descriptor["stp_max_power_uw"] = std::filesystem::path(m_path).append("constraint_1_max_power_uw");
        m_descriptor["stp_time_window_us"] = std::filesystem::path(m_path).append("constraint_1_time_window_us");
        m_descriptor["stp_name"] = std::filesystem::path(m_path).append("constraint_1_name");
        m_descriptor["stp_power_limit_uw"] = std::filesystem::path(m_path).append("constraint_1_power_limit_uw");

        m_descriptor["pp_max_power_uw"] = std::filesystem::path(m_path).append("constraint_2_max_power_uw");
        m_descriptor["pp_time_window_us"] = std::filesystem::path(m_path).append("constraint_2_time_window_us");
        m_descriptor["pp_name"] = std::filesystem::path(m_path).append("constraint_2_name");
        m_descriptor["pp_power_limit_uw"] = std::filesystem::path(m_path).append("constraint_2_power_limit_uw");

        m_descriptor["max_energy_range"] = std::filesystem::path(m_path).append("max_energy_range_uj");
        m_descriptor["powercapCPUEnergy"] = std::filesystem::path(m_path).append("energy_uj");

        LOG_D(QString("Power Intel Powercap RAPL driver descriptor added !"));
    }
    else
    {
        LOG_D(QString("Intel Powercap RAPL driver not found in path: ") + m_path.c_str());
    }
}

void SysFsDriverIntelPowercapRapl::handleKernelEvent(const KernelEvent::Event &event)
{
    LOG_D(__PRETTY_FUNCTION__ + QString(": Kernel event received ACTION=") + event.m_action + ", DRIVER=" + event.m_driver + ", SYSNAME=" + event.m_sysName + ", SUBSYSTEM=" + event.m_subSystem + ", DEVPATH=" + event.m_devPath);

    if(m_blockKernelEvent)
    {
        LOG_D(QString("Kernel event blocked for driver: ") + m_name);
        return;
    }

    if(event.m_sysName == "intel-rapl:0")
    {
        init();
        validate();

        emit kernelEvent({
            .m_driverName = DRIVER_NAME,
            .m_action = SubsystemEvent::Action::RELOADED,
            .m_DriverSpecificEventType = "reloaded",
            .m_DriverSpecificEventValue = {}
        });
    }
}


}
