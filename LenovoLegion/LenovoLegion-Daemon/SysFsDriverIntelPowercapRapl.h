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


class SysFsDriverIntelPowercapRapl : public SysFsDriver
{

public:

    struct IntelPowercapRapl {

        IntelPowercapRapl(const SysFsDriver::DescriptorType& descriptor) :
            m_powercapCPUEnergy(descriptor["powercapCPUEnergy"])
        {}

        const std::filesystem::path m_powercapCPUEnergy;
    };

public:

    SysFsDriverIntelPowercapRapl(QObject* parrent);


    ~SysFsDriverIntelPowercapRapl() override = default;


    /*
     * Init Driver
     */
    virtual void init() override;

    /*
     * Handle kernel event
     */
    virtual void handleKernelEvent(const KernelEvent::Event &event) override;


    /*
     * Driver name, system driver __ prefix is used to mark  system driver no modprobe loadable
     */
    static constexpr const char* DRIVER_NAME =  "intel-rapl";
};

}
