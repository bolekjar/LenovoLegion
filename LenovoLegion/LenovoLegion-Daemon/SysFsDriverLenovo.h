// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include "SysFsDriver.h"

namespace LenovoLegionDaemon {


class SysFsDriverLenovo : public SysFsDriver
{

public:

    struct Lenovo {

        struct HWMon {

            HWMon(const SysFsDriver::DescriptorType& descriptor) :
                m_fan1Speed(descriptor["fan1Speed"]),
                m_fan2Speed(descriptor["fan2Speed"]),
                m_fan1SpeedMax(descriptor["fan1MaxSpeed"]),
                m_fan2SpeedMax(descriptor["fan2MaxSpeed"]),
                m_cpuTemp(descriptor["cpuTemp"])    ,
                m_gpuTemp(descriptor["gpuTemp"])    ,
                m_fanCurvePoints(descriptor["fanCurvePoints"])
            {}

            const std::filesystem::path m_fan1Speed;
            const std::filesystem::path m_fan2Speed;
            const std::filesystem::path m_fan1SpeedMax;
            const std::filesystem::path m_fan2SpeedMax;

            const std::filesystem::path m_cpuTemp;
            const std::filesystem::path m_gpuTemp;
            const std::filesystem::path m_fanCurvePoints;
        };


        struct GPUControl {

            GPUControl(const  SysFsDriver::DescriptorType& descriptor) :
                m_gpuBoostLimit(descriptor["gpuBoostLimit"]),
                m_gpuTargetLimit(descriptor["gpuTargetLimit"]),
                m_gpuTempLimit(descriptor["gpuTempLimit"])
            {}

            const std::filesystem::path m_gpuBoostLimit;
            const std::filesystem::path m_gpuTargetLimit;
            const std::filesystem::path m_gpuTempLimit;
        };


        struct CPUControl {

            CPUControl(const SysFsDriver::DescriptorType& descriptor) :
                m_cpuShortTermPowerLimit(descriptor["cpuShortTermPowerLimit"]),
                m_cpuLongTermPowerLimit(descriptor["cpuLongTermPowerLimit"]),
                m_cpuCrossLoadingPowerLimit(descriptor["cpuCrossLoadingPowerLimit"]),
                m_cpuTempLimit(descriptor["cpuTempLimit"]),
                m_cpuL1Tau(descriptor["cpuL1Tau"]),
                m_cpuTotalPowerTargetInAC(descriptor["cpuTotalPowerTargetInAC"]),
                m_cpuPowerBoost(descriptor["cpuPowerBoost"])
            {}

            const std::filesystem::path m_cpuShortTermPowerLimit;
            const std::filesystem::path m_cpuLongTermPowerLimit;
            const std::filesystem::path m_cpuCrossLoadingPowerLimit;
            const std::filesystem::path m_cpuTempLimit;
            const std::filesystem::path m_cpuL1Tau;
            const std::filesystem::path m_cpuTotalPowerTargetInAC;
            const std::filesystem::path m_cpuPowerBoost;
        };

        struct FanControl {

            FanControl(const SysFsDriver::DescriptorType& descriptor) :
                m_maxFanSpeed(descriptor["maxFanSpeed"]),
                m_lockFanControl(descriptor["lockFanControl"]),
                m_fanMode(descriptor["fanMode"])
            {}

            const std::filesystem::path  m_maxFanSpeed;
            const std::filesystem::path  m_lockFanControl;
            const std::filesystem::path  m_fanMode;
        };

        struct PowerCharge {

            PowerCharge(const SysFsDriver::DescriptorType& descriptor) :
                m_powerChargeMode(descriptor["powerChargeMode"])
            {}

            const std::filesystem::path  m_powerChargeMode;
        };


        explicit Lenovo(const  SysFsDriver::DescriptorType& descriptor) :
            m_hwmon(descriptor),
            m_gpuControl(descriptor),
            m_cpuControl(descriptor),
            m_fanControl(descriptor),
            m_powerCharge(descriptor)
        {}


        /*
        * HW Monitoring
        */
        const HWMon            m_hwmon;


        /*
        * Controls
        */
        const GPUControl        m_gpuControl;
        const CPUControl        m_cpuControl;
        const FanControl        m_fanControl;
        const PowerCharge       m_powerCharge;
    };


    /*
     * Driver name
     */
    static constexpr const char* DRIVER_NAME = "lenovo_legion";

public:

    SysFsDriverLenovo(QObject* parrent);

    ~SysFsDriverLenovo() override = default;


    /*
     * Init Driver
     */
    virtual void init() override;
};

}
