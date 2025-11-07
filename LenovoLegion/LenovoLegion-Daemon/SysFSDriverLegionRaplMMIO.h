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

class SysFSDriverLegionRaplMMIO : public SysFsDriver
{
public:

    struct RaplMMIO {

        RaplMMIO(const SysFsDriver::DescriptorType& descriptor) :
            m_is_locked(descriptor["is_locked"]),
            m_is_pl4_locked(descriptor["is_pl4_locked"]),
            m_pl1_current_value(descriptor["pl1_current_value"]),
            m_pl2_current_value(descriptor["pl2_current_value"]),
            m_pl4_current_value(descriptor["pl4_current_value"]),
            m_set_and_lock(descriptor["set_and_lock"]),
            m_set_pl4_and_lock(descriptor["set_pl4_and_lock"])
        {}

        const std::filesystem::path m_is_locked;
        const std::filesystem::path m_is_pl4_locked;

        const std::filesystem::path m_pl1_current_value;
        const std::filesystem::path m_pl2_current_value;
        const std::filesystem::path m_pl4_current_value;

        const std::filesystem::path m_set_and_lock;
        const std::filesystem::path m_set_pl4_and_lock;
    };

public:

    SysFSDriverLegionRaplMMIO(QObject * parrent);

   ~SysFSDriverLegionRaplMMIO() override = default;

    /*
     * Init Driver
     */
    virtual void init() override;

public:

    /*
     * Driver name, system driver __ prefix is used to mark  system driver no modprobe loadable
     */
    static constexpr const char* DRIVER_NAME =  "legion_rapl_mmio";
    static constexpr const char* MODULE_NAME =  LEGION_MODULE_NAME;
};

}
