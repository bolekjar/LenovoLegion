// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include "ControlData.h"

#include <QList>

namespace LenovoLegionDaemon {



class ControlDataProvider
{

public:

    virtual ~ControlDataProvider() =  default;

    /*
     * Getters
     */
    virtual PowerProfile::Control::DataInfo        getPowerProfileData()            const = 0;
    virtual PowerControl::GPU::DataInfo            getGpuControlData()              const = 0;
    virtual PowerControl::CPU::DataInfo            getCpuControlData()              const = 0;
    virtual HWMonitoring::DataInfo                 getHWMonitoringData()            const = 0;

    virtual CPUTopology::Heterogeneous::DataInfo   getCPUTopologyHeterogenousData() const = 0;
    virtual CPUTopology::Homogeneous::DataInfo     getCPUTopologyHomogenousData()   const = 0;

    virtual CPUXFreqControl::DataInfo              getCPUsInfoData()                const = 0;
    virtual CPUXControl::DataInfo                  getCPUsInfoControlData()         const = 0;

    virtual FanControl::Control::DataInfo          getFanControlData()              const = 0;
    virtual FanControl::CurveControl::DataInfo     getFanCurveControlData()         const = 0;


    virtual Batery::DataInfo                       getBateryControlData()           const = 0;
    virtual CPUSMTControl::DataInfo                getCPUSMTControlData()           const = 0;


    /*
     * Setters
     */
    virtual void setPowerProfileData(const PowerProfile::Control::DataControl& data)         = 0;
    virtual void setGpuControlData(const PowerControl::GPU::DataControl& data)               = 0;
    virtual void setCpuControlData(const PowerControl::CPU::DataControl& data)               = 0;
    virtual void setCpuFreqControlData(const CPUXFreqControl::DataControl& data)             = 0;


    virtual void setCpuControlData(const CPUXControl::DataControl& data)                     = 0;

    virtual void setFanControlData(const FanControl::Control::DataControl& data)             = 0;
    virtual void setFanCurveControlData(const FanControl::CurveControl::DataControl& data)   = 0;

    virtual void setCPUSMTControlData(const CPUSMTControl::DataControl& data)                = 0;

};


} // namespace LenovoLegionDaemon
