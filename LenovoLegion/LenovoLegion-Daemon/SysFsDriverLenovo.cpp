// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "SysFsDriverLenovo.h"

#include <Core/LoggerHolder.h>

namespace LenovoLegionDaemon {



SysFsDriverLenovo::SysFsDriverLenovo(QObject* parrent) : SysFsDriver(DRIVER_NAME,"/sys/module/lenovo_legion/drivers/platform:lenovo-legion/VPC2004:00/",{},parrent) {}

void SysFsDriverLenovo::init()
{
    clean();

    if(std::filesystem::exists(m_path))
    {
        LOG_D(QString("Lenovo driver found in path: ") + m_path.c_str());

        /*
         * Scanning hwmon directory
         */
        for(const auto& entry : std::filesystem::directory_iterator(std::filesystem::path(m_path).append("hwmon")))
        {
            if(entry.is_directory())
            {
                /*
                 * GPU control
                 */
                m_descriptor["gpuBoostLimit"]   = std::filesystem::path(m_path).append("gpu_pwr_boost");
                m_descriptor["gpuTargetLimit"]  = std::filesystem::path(m_path).append("gpu_ctgp_limit");
                m_descriptor["gpuTempLimit"]  = std::filesystem::path(m_path).append("gpu_temp_limit");


                /*
                 * CPU control
                 */
                m_descriptor["cpuShortTermPowerLimit"]   = std::filesystem::path(m_path).append("cpu_st_plimit");
                m_descriptor["cpuLongTermPowerLimit"]   = std::filesystem::path(m_path).append("cpu_lt_plimit");
                m_descriptor["cpuCrossLoadingPowerLimit"]   = std::filesystem::path(m_path).append("cpu_cl_plimit");
                m_descriptor["cpuTempLimit"]  = std::filesystem::path(m_path).append("cpu_temp_limit");
                m_descriptor["cpuL1Tau"]      = std::filesystem::path(m_path).append("cpu_l1_tau");
                m_descriptor["cpuTotalPowerTargetInAC"]     = std::filesystem::path(m_path).append("cpu_ac_tpwr");
                m_descriptor["cpuPowerBoost"]   = std::filesystem::path(m_path).append("cpu_pwr_boost");



                /*
                 *
                 * Fan control
                 */
                m_descriptor["fanMode"]      = std::filesystem::path(m_path).append("fan_mode");
                m_descriptor["lockFanControl"]      = std::filesystem::path(m_path).append("fan_lock");
                m_descriptor["maxFanSpeed"] = std::filesystem::path(m_path).append("fan_fullspeed");


                /*
                 * Power charge
                 */
                m_descriptor["powerChargeMode"] = std::filesystem::path(m_path).append("pwr_chargemode");


                m_descriptor["fan1Speed"] = std::filesystem::path(entry).append("fan1_input");
                m_descriptor["fan2Speed"] = std::filesystem::path(entry).append("fan2_input");
                m_descriptor["fan1MaxSpeed"] = std::filesystem::path(entry).append("fan1_max");
                m_descriptor["fan2MaxSpeed"] = std::filesystem::path(entry).append("fan2_max");

                m_descriptor["cpuTemp"] = std::filesystem::path(entry).append("temp1_input");
                m_descriptor["gpuTemp"] = std::filesystem::path(entry).append("temp2_input");

                m_descriptor["fanCurvePoints"]  = std::filesystem::path(entry).append("pwm1_auto_points_pwm");

                LOG_D(QString("Lenovo driver descriptor added !"));

                break;
            }
        }
    }
    else
    {
        LOG_D(QString("Lenovo driver not found in path: ") + m_path.c_str());
    }

}

}
