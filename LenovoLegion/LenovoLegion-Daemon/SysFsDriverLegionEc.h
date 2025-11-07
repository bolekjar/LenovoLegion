// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once


#include "SysFsDriver.h"
#include "SysFsDriverLegion.h"



namespace LenovoLegionDaemon {

class SysFsDriverLegionEc : public SysFsDriver
{
public:
    struct EC {

        struct FanControl {

            FanControl(const SysFsDriver::DescriptorType& descriptor) :
                m_full_speed(descriptor["fan_control_full_speed"]),
                m_lock(descriptor["fan_control_lock"]),
                m_display_name(descriptor["fan_control_display_name"]),
                m_min_curve(descriptor["fan_control_min_curve"])
            {}

            const std::filesystem::path m_full_speed;
            const std::filesystem::path m_lock;
            const std::filesystem::path m_display_name;
            const std::filesystem::path m_min_curve;
        };


        explicit EC(const SysFsDriver::DescriptorType& descriptor) :
            m_fanControl(descriptor)
        {}


        const FanControl         m_fanControl;
    };
public:

    SysFsDriverLegionEc(QObject * parrent);

    ~SysFsDriverLegionEc() override = default;

    /*
     * Init Driver
     */
    virtual void init() override;

public:

    /*
     * Driver name, system driver __ prefix is used to mark  system driver no modprobe loadable
     */
    static constexpr const char* DRIVER_NAME =  "legion_ec";
    static constexpr const char* MODULE_NAME =  LEGION_MODULE_NAME;
};

}
