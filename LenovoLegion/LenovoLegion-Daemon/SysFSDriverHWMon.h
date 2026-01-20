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


class SysFSDriverHWMon : public SysFsDriver
{
public:


    struct HWMon {

        struct Legion {

            struct Fan {
                const std::filesystem::path m_input;
                const std::filesystem::path m_min;
                const std::filesystem::path m_max;
                const std::filesystem::path m_label;
            };

            Legion(const SysFsDriver::DescriptorType& descriptor,const SysFsDriver::DescriptorsInVectorType& descriptorInVector) :
                m_temp1Temp(descriptor["temp1"])        ,
                m_temp2Temp(descriptor["temp2"])        ,
                m_temp1Label(descriptor["temp1Label"])  ,
                m_temp2Label(descriptor["temp2Label"])  ,
                m_fans([&descriptorInVector](){
                    std::vector<Fan> fans;

                    for(const auto& desc : descriptorInVector)
                    {
                        fans.emplace_back(Fan{
                            .m_input    = desc.value("fan_input"),
                            .m_min = desc.value("fan_min"),
                            .m_max = desc.value("fan_max"),
                            .m_label    = desc.value("fan_label")
                        });
                    }

                    return  fans;
                }())
            {}

            const std::filesystem::path m_temp1Temp;
            const std::filesystem::path m_temp2Temp;
            const std::filesystem::path m_temp1Label;
            const std::filesystem::path m_temp2Label;

            const std::vector<Fan> m_fans;
        };


        explicit HWMon(const SysFsDriver::DescriptorType& descriptor,const SysFsDriver::DescriptorsInVectorType& descriptorInVector) :
            m_legion(descriptor,descriptorInVector)
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
    static constexpr const char* MODULE_NAME =  LEGION_MODULE_NAME;

};

}
