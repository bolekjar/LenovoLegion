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


class SysFSDriverHWMon : public SysFsDriver
{
public:


    struct HWMon {

        struct Legion {

            Legion(const SysFsDriver::DescriptorType& descriptor) :
                m_fan1Speed(descriptor["fan1Speed"]),
                m_fan2Speed(descriptor["fan2Speed"]),
                m_fan1SpeedMin(descriptor["fan1MinSpeed"]),
                m_fan2SpeedMin(descriptor["fan2MinSpeed"]),
                m_fan1SpeedMax(descriptor["fan1MaxSpeed"]),
                m_fan2SpeedMax(descriptor["fan2MaxSpeed"]),
                m_fan1Label(descriptor["fan1Label"]),
                m_fan2Label(descriptor["fan2Label"]),
                m_temp1Temp(descriptor["temp1"])    ,
                m_temp2Temp(descriptor["temp2"])    ,
                m_temp1Label(descriptor["temp1Label"])    ,
                m_temp2Label(descriptor["temp2Label"])
            {}

            const std::filesystem::path m_fan1Speed;
            const std::filesystem::path m_fan2Speed;
            const std::filesystem::path m_fan1SpeedMin;
            const std::filesystem::path m_fan2SpeedMin;
            const std::filesystem::path m_fan1SpeedMax;
            const std::filesystem::path m_fan2SpeedMax;
            const std::filesystem::path m_fan1Label;
            const std::filesystem::path m_fan2Label;

            const std::filesystem::path m_temp1Temp;
            const std::filesystem::path m_temp2Temp;
            const std::filesystem::path m_temp1Label;
            const std::filesystem::path m_temp2Label;
        };


        explicit HWMon(const SysFsDriver::DescriptorType& descriptor) :
            m_legion(descriptor)
        {}


        const Legion         m_legion;
    };

public:

    SysFSDriverHWMon(QObject * parrent);

    ~SysFSDriverHWMon() override = default;

    /*
     * Init Driver
     */
    virtual void init() override;

public:

    /*
     * Driver name, system driver __ prefix is used to mark  system driver no modprobe loadable
     */
    static constexpr const char* DRIVER_NAME =  "hwmon";
    static constexpr const char* MODULE_NAME =  "lenovo_legion";


};

}
