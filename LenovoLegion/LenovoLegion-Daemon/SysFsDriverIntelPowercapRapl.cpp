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


SysFsDriverIntelPowercapRapl::SysFsDriverIntelPowercapRapl(QObject* parrent) : SysFsDriver(DRIVER_NAME,"/sys/class/powercap/intel-rapl:0/",{"powercap"},parrent) {}

void SysFsDriverIntelPowercapRapl::init()
{
    clean();

    if(std::filesystem::exists(std::filesystem::path(m_path)))
    {
        LOG_D(QString("Found Intel Powercap RAPL driver in path: ") + m_path.c_str());

        m_descriptor["powercapCPUEnergy"] = std::filesystem::path(m_path).append("energy_uj");
    }
    else
    {
        LOG_D(QString("Intel Powercap RAPL driver not found in path: ") + m_path.c_str());
    }
}

void SysFsDriverIntelPowercapRapl::handleKernelEvent(const KernelEvent::Event &event)
{
    LOG_D(QString("Kernel event received ACTION=") + event.m_action + ", DRIVER=" + event.m_driver + ", SYSNAME=" + event.m_sysName + ", SUBSYSTEM=" + event.m_subSystem + ", DEVPATH=" + event.m_devPath);

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
            .m_DriverSpecificAction = "reloaded"
        });
    }
}


}
