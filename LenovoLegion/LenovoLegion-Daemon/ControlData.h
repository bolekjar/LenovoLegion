// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <Core/ExceptionBuilder.h>

#include <qtypes.h>

#include <array>
#include <limits>
#include <functional>
#include <cstring>

namespace LenovoLegionDaemon {

constexpr quint32 MAX_SUPPORTED_CPUS = 32;


struct ControlDataException : public bj::framework::exception::Exception {
    enum ERROR_CODES : int {
        NOT_AVAILABLE = 0
    };
};


struct HWMonitoring {
    struct __attribute__ ((__packed__)) Lenovo {

        struct  __attribute__ ((__packed__)) Data {
            quint32  m_cpuTemp;
            quint32  m_gpuTemp;
            quint32  m_fan1Speed;
            quint32  m_fan2Speed;
            quint32  m_fan1MaxSpeed;
            quint32  m_fan2MaxSpeed;
        };

        struct __attribute__ ((__packed__)) DataInfo  {
            Data m_data;
            bool m_isAvailable;
        };

        struct __attribute__ ((__packed__)) DataControl  {
        };

    };

    struct __attribute__ ((__packed__)) IntelPowerapRapl {

        struct __attribute__ ((__packed__)) Data {
            quint64 m_powercapCPUEnergy;
        };

        struct __attribute__ ((__packed__)) DataInfo {
            Data m_data;
            bool m_isAvailable;
        };

        struct __attribute__ ((__packed__)) DataControl {
        };
    };

    struct __attribute__ ((__packed__)) CPUXFreq {

        struct __attribute__ ((__packed__)) Data {
            quint32 m_cpuBaseFreq;
            quint32 m_cpuInfoMinFreq;
            quint32 m_cpuInfoMaxFreq;
            quint32 m_cpuScalingCurFreq;
            quint32 m_cpuScalingMinFreq;
            quint32 m_cpuScalingMaxFreq;
        };

        struct __attribute__ ((__packed__)) DataInfo {
            Data m_data;
            bool m_isAvailable;
        };

        struct __attribute__ ((__packed__)) DataControl {
        };
    };

    struct DataInfo {

        HWMonitoring::Lenovo::DataInfo                                  m_lenovo;
        HWMonitoring::IntelPowerapRapl::DataInfo                        m_intelPowerapRapl;
        std::array<HWMonitoring::CPUXFreq::DataInfo,MAX_SUPPORTED_CPUS> m_cpusFreq;
    };
};


struct FanControl {

    struct CurveControl {
        static constexpr quint8 MAX_FANCURVE_SIZE = 10;

        struct  __attribute__ ((__packed__)) Data {
            std::array<quint8,MAX_FANCURVE_SIZE> m_points;
        };

        struct __attribute__ ((__packed__)) DataInfo {
            Data m_data;
            bool m_isAvailable;
        };

        struct __attribute__ ((__packed__)) DataControl {
            Data m_data;
        };
    };


    struct __attribute__ ((__packed__)) Control {

        struct  __attribute__ ((__packed__)) Data {
            bool m_maxFanSpeed;
            bool m_LockFan;
        };

        struct __attribute__ ((__packed__)) DataInfo {
            Data m_data;
            bool m_isAvailable;
        };

        struct __attribute__ ((__packed__)) DataControl {
            Data m_data;
        };
    };
};



struct PowerControl {

    struct __attribute__ ((__packed__)) GPU {

        struct  __attribute__ ((__packed__)) Data {
            quint8 m_boostLimit;
            quint8 m_targetLimit;
            quint8 m_tempLimit;
        };

        struct __attribute__ ((__packed__)) DataInfo {
            Data m_data;
            bool m_isAvailable;
        };

        struct __attribute__ ((__packed__)) DataControl {
            Data m_data;
        };
    };

    struct  __attribute__ ((__packed__))  CPU {

        struct __attribute__ ((__packed__)) Data {
            quint8 m_shortTermPowerLimit;
            quint8 m_longTermPowerLimit;
            quint8 m_crossLoadingPowerLimit;
            quint8 m_tempLimit;
            quint8 m_l1Tau;
            quint8 m_totalPowerTargetInAC;
            quint8 m_powerBoost;
        };

        struct __attribute__ ((__packed__)) DataInfo {
            Data m_data;
            bool m_isAvailable;
        };


        struct __attribute__ ((__packed__)) DataControl {
            Data m_data;
        };
    };
};

struct PowerProfile {

    struct __attribute__ ((__packed__)) Control {

        enum class PowerProfiles : quint8 {
            POWER_PROFILE_UNKNOWN      = 0,
            POWER_PROFILE_QUIET        = 1,
            POWER_PROFILE_BALANCED     = 2,
            POWER_PROFILE_PERFORMANCE  = 3,
            POWER_PROFILE_CUSTOM       = 255
        };

        struct __attribute__ ((__packed__)) Data {
            PowerProfiles m_profile;
        };

        struct __attribute__ ((__packed__)) DataInfo {
            Data m_data;
            bool m_isAvailable;
        };

        struct __attribute__ ((__packed__)) DataControl {
            Data m_data;
        };


        static DataControl getDataControl(const DataInfo& dataInfo)
        {
            if(dataInfo.m_isAvailable == false)
            {
                THROW_EXCEPTION(ControlDataException,ControlDataException::NOT_AVAILABLE,"PowerProfile DataInfo not available");
            }

            return DataControl {.m_data {.m_profile = dataInfo.m_data.m_profile}};
        }

        static DataControl getDataControl(const PowerProfiles& profile)
        {
            return DataControl {.m_data {.m_profile = profile}};
        }
    };
};


using Text                  = std::array<char,128>;


struct Batery {
    enum class PowerChargeMode : quint8 {
        POWER_CHARGE_MODE_AC         = 1,
        POWER_CHARGE_MODE_BATTERY    = 2
    };

    struct __attribute__ ((__packed__)) Data {
        PowerChargeMode      m_powerChargeMode;
        Text             m_batteryStatus;
    };
    struct __attribute__ ((__packed__)) DataInfo {
        Data m_data;
        bool m_isAvailable;
    };

    struct __attribute__ ((__packed__)) DataControl {
    };
};

struct CPUSMTControl {

    struct __attribute__ ((__packed__)) DataInfo {

        struct __attribute__ ((__packed__)) Data {
            bool    m_active;
            char    m_control[16];
        } m_data;

        bool m_isAvailable;
    };

    struct __attribute__ ((__packed__)) DataControl {
        struct __attribute__ ((__packed__)) Data {
            char    m_control[16];
        } m_data;
    };


    static DataControl getDataControl(const DataInfo& dataInfo)
    {
        DataControl data;

        if(dataInfo.m_isAvailable == false)
        {
            THROW_EXCEPTION(ControlDataException,ControlDataException::NOT_AVAILABLE,"PowerProfile DataInfo not available");
        }

        std::memset(data.m_data.m_control,0,sizeof(data.m_data.m_control) * sizeof(data.m_data.m_control[0]));
        std::memcpy(data.m_data.m_control,dataInfo.m_data.m_control,sizeof(data.m_data.m_control) * sizeof(data.m_data.m_control[0]));

        return data;
    }

    static DataControl getDataControl(const QString& control)
    {
        DataControl data;

        std::memset(data.m_data.m_control,0,sizeof(data.m_data.m_control) * sizeof(data.m_data.m_control[0]));
        std::strncpy(data.m_data.m_control,control.toStdString().c_str(),sizeof(data.m_data.m_control) * sizeof(data.m_data.m_control[0]) - 1);

        return data;
    }
};


struct CPUTopology {

    struct __attribute__ ((__packed__)) ActiveCPUsRange {
        quint16 m_cpuMin;
        quint16 m_cpuMax;
    };

    static constexpr quint16 NO_RANGE_VALUE = std::numeric_limits<quint16>::max();;


    struct Heterogeneous {
        struct __attribute__ ((__packed__)) Data {
            std::array<ActiveCPUsRange,MAX_SUPPORTED_CPUS> m_ActiveCpusCore;
            std::array<ActiveCPUsRange,MAX_SUPPORTED_CPUS> m_ActiveCpusAtom;
        };

        struct __attribute__ ((__packed__)) DataInfo {
            Data m_data;
            bool m_isAvailable;
        };

        struct __attribute__ ((__packed__)) DataControl {
        };
    };

    struct Homogeneous {
        struct __attribute__ ((__packed__)) Data {
            std::array<ActiveCPUsRange,MAX_SUPPORTED_CPUS> m_ActiveCpus;
        };

        struct __attribute__ ((__packed__)) DataInfo {
            Data m_data;
            bool m_isAvailable;
        };

        struct __attribute__ ((__packed__)) DataControl {
        };
    };

    static void forAllCpuRange(const std::function<bool (const int)> &func, const std::array<ActiveCPUsRange,MAX_SUPPORTED_CPUS> &cpuRange)
    {
        for (const auto& activeCpuRange: cpuRange) {
            if(activeCpuRange.m_cpuMax != NO_RANGE_VALUE && activeCpuRange.m_cpuMin != NO_RANGE_VALUE)
            {
                for(quint16 i = activeCpuRange.m_cpuMin; i < activeCpuRange.m_cpuMax + 1; ++i)
                {
                    if(!func(i)) {
                        return;
                    }
                }
            }
        }
    }
};


struct CPUXFreqControl
{
    struct __attribute__ ((__packed__)) DataInfo {

        struct __attribute__ ((__packed__)) CPUX {
            quint32                 m_cpuBaseFreq;
            quint32                 m_cpuInfoMinFreq;
            quint32                 m_cpuInfoMaxFreq;
            quint32                 m_cpuScalingCurFreq;
            quint32                 m_cpuScalingMinFreq;
            quint32                 m_cpuScalingMaxFreq;
            bool                    m_cpuOnline;
            quint32                 m_cpuCoreId;
            quint32                 m_dieId;
            quint32                 m_physicalPackageId;
            quint32                 m_clusterId;
        };

        struct Data {
            std::array<CPUX,MAX_SUPPORTED_CPUS> m_cpus;
        } m_data;

        bool m_isAvailable;
    };

    struct __attribute__ ((__packed__)) DataControl {

        struct __attribute__ ((__packed__))  CPUX {
            quint32             m_cpuScalingMinFreq;
            quint32             m_cpuScalingMaxFreq;
        };

        struct __attribute__ ((__packed__))  Data {
            std::array<CPUX,MAX_SUPPORTED_CPUS> m_cpus;
        } m_data;

        static void copy(CPUX& dst,const CPUX& src)
        {
            dst.m_cpuScalingMinFreq = src.m_cpuScalingMinFreq;
            dst.m_cpuScalingMaxFreq = src.m_cpuScalingMaxFreq;
        }
    };

    static void forAllCpu(const std::function<bool (const DataInfo::CPUX&,const size_t)> &func, const std::array<DataInfo::CPUX,MAX_SUPPORTED_CPUS> &cpus)
    {
        for(size_t i = 0; i < cpus.size(); ++i)
        {
            if(!func(cpus.at(i),i)) {
                return;
            }
        }
    }

    static void forAllCpu(const std::function<bool (const DataControl::CPUX&,const size_t)> &func, const std::array<DataControl,MAX_SUPPORTED_CPUS> &cpus)
    {
        for(size_t i = 0; i < cpus.size(); ++i)
        {
            if(!func(cpus.at(i).m_data.m_cpus.at(i),i)) {
                return;
            }
        }
    }

    static void forAllCpuRange (const std::function<bool (DataControl::CPUX&,const size_t)> &func,std::array<DataControl::CPUX,MAX_SUPPORTED_CPUS> &cpus, const std::array<CPUTopology::ActiveCPUsRange,MAX_SUPPORTED_CPUS> &cpuRange)
    {
        for (const auto& activeCpuRange: cpuRange) {
            if(activeCpuRange.m_cpuMax != CPUTopology::NO_RANGE_VALUE && activeCpuRange.m_cpuMin != CPUTopology::NO_RANGE_VALUE)
            {
                for(quint16 i = activeCpuRange.m_cpuMin; i < activeCpuRange.m_cpuMax + 1; ++i)
                {
                    if(!func(cpus.at(i),i)) {
                        return;
                    }
                }
            }
        }
    }

    static void forAllCpuRangeSet(std::array<DataControl::CPUX,MAX_SUPPORTED_CPUS> &cpus, const std::array<CPUTopology::ActiveCPUsRange,MAX_SUPPORTED_CPUS> &cpuRange,const DataControl::CPUX& value)
    {
        CPUXFreqControl::forAllCpuRange([&value](DataControl::CPUX& cpu,const size_t){
            CPUXFreqControl::DataControl::copy(cpu,value);
            return true;
        },cpus,cpuRange);
    }


    static DataControl getDataControl(const DataInfo& dataInfo)
    {
        DataControl dataControl = {};

        if(dataInfo.m_isAvailable == false)
        {
            THROW_EXCEPTION(ControlDataException,ControlDataException::NOT_AVAILABLE,"CPUXFreqControl DataInfo not available");
        }


        forAllCpu([&dataControl](const DataInfo::CPUX& cpuInfo,const size_t i){

            dataControl.m_data.m_cpus.at(i).m_cpuScalingMinFreq = cpuInfo.m_cpuScalingMinFreq;
            dataControl.m_data.m_cpus.at(i).m_cpuScalingMaxFreq = cpuInfo.m_cpuScalingMaxFreq;

            return true;
        },dataInfo.m_data.m_cpus);

        return dataControl;
    }

    static DataControl getDataControl(const DataInfo& dataControl,const std::array<CPUTopology::ActiveCPUsRange,MAX_SUPPORTED_CPUS> &cpuRange,const DataControl::CPUX& value)
    {
        LenovoLegionDaemon::CPUXFreqControl::DataControl  data = LenovoLegionDaemon::CPUXFreqControl::getDataControl(dataControl);
        LenovoLegionDaemon::CPUXFreqControl::forAllCpuRangeSet(data.m_data.m_cpus,cpuRange,value);
        return data;
    }
};


struct CPUXControl {
    struct __attribute__ ((__packed__)) DataInfo {

        struct __attribute__ ((__packed__)) CPUX {
            char                    m_Governor[32];
            char                    m_availableGovernors[128];
            bool                    m_cpuOnline;
            quint32                 m_cpuCoreId;
            quint32                 m_dieId;
            quint32                 m_physicalPackageId;
            quint32                 m_clusterId;
        };

        struct Data {
            std::array<CPUX,MAX_SUPPORTED_CPUS> m_cpus;
        } m_data;

        bool m_isAvailable;
    };

    struct __attribute__ ((__packed__)) DataControl {
        struct __attribute__ ((__packed__)) CPUX {
            char                    m_Governor[32];
            bool                    m_cpuOnline;
        };

        struct __attribute__ ((__packed__)) Data {
            std::array<CPUX,MAX_SUPPORTED_CPUS> m_cpus;
        } m_data;

        static void copy(CPUX& dst,const CPUX& src)
        {
            dst.m_cpuOnline = src.m_cpuOnline;
            memcpy(dst.m_Governor,src.m_Governor,sizeof(dst.m_Governor) * sizeof(dst.m_Governor[0]));
        }

    };

    static void forAllCpu(const std::function<bool (const DataInfo::CPUX&,const size_t)> &func, const std::array<DataInfo::CPUX,MAX_SUPPORTED_CPUS> &cpus)
    {
        for(size_t i = 0; i < cpus.size(); ++i)
        {
            if(!func(cpus.at(i),i)) {
                return;
            }
        }
    }

    static void forAllCpuRange(const std::function<bool (DataInfo::CPUX&,const size_t)> &func,std::array<DataInfo::CPUX,MAX_SUPPORTED_CPUS> &cpus, const std::array<CPUTopology::ActiveCPUsRange,MAX_SUPPORTED_CPUS> &cpuRange)
    {
        for (const auto& activeCpuRange: cpuRange) {
            if(activeCpuRange.m_cpuMax != CPUTopology::NO_RANGE_VALUE && activeCpuRange.m_cpuMin != CPUTopology::NO_RANGE_VALUE)
            {
                for(quint16 i = activeCpuRange.m_cpuMin; i < activeCpuRange.m_cpuMax + 1; ++i)
                {
                    if(!func(cpus.at(i),i)) {
                        return;
                    }
                }
            }
        }
    }

    static void forAllCpuRange(const std::function<bool (DataControl::CPUX&,const size_t)> &func,std::array<DataControl::CPUX,MAX_SUPPORTED_CPUS> &cpus, const std::array<CPUTopology::ActiveCPUsRange,MAX_SUPPORTED_CPUS> &cpuRange)
    {
        for (const auto& activeCpuRange: cpuRange) {
            if(activeCpuRange.m_cpuMax != CPUTopology::NO_RANGE_VALUE && activeCpuRange.m_cpuMin != CPUTopology::NO_RANGE_VALUE)
            {
                for(quint16 i = activeCpuRange.m_cpuMin; i < activeCpuRange.m_cpuMax + 1; ++i)
                {
                    if(!func(cpus.at(i),i)) {
                        return;
                    }
                }
            }
        }
    }

    static void forAllCpuRangeSet(std::array<DataControl::CPUX,MAX_SUPPORTED_CPUS> &cpus, const std::array<CPUTopology::ActiveCPUsRange,MAX_SUPPORTED_CPUS> &cpuRange,const DataControl::CPUX& value)
    {
        LenovoLegionDaemon::CPUXControl::forAllCpuRange([&value](LenovoLegionDaemon::CPUXControl::DataControl::CPUX& cpu,const size_t){
            CPUXControl::DataControl::copy(cpu,value);
            return true;
        },cpus,cpuRange);
    }

    static DataControl getDataControl(const DataInfo& dataInfo)
    {
        DataControl dataControl = {};

        if(dataInfo.m_isAvailable == false)
        {
            THROW_EXCEPTION(ControlDataException,ControlDataException::NOT_AVAILABLE,"CPUX DataInfo not available");
        }

        forAllCpu([&dataControl](const DataInfo::CPUX& cpuInfo,const size_t i){

            dataControl.m_data.m_cpus.at(i).m_cpuOnline = cpuInfo.m_cpuOnline;

            if(dataControl.m_data.m_cpus.at(i).m_cpuOnline)
            {
                std::memset(dataControl.m_data.m_cpus.at(i).m_Governor,0,sizeof(dataControl.m_data.m_cpus.at(i).m_Governor) * sizeof(dataControl.m_data.m_cpus.at(i).m_Governor[0]));
                std::memcpy(dataControl.m_data.m_cpus.at(i).m_Governor,cpuInfo.m_Governor,sizeof(dataControl.m_data.m_cpus.at(i).m_Governor) * sizeof(dataControl.m_data.m_cpus.at(i).m_Governor[0]));
            }
            else
            {
                std::memset(dataControl.m_data.m_cpus.at(i).m_Governor,0,sizeof(dataControl.m_data.m_cpus.at(i).m_Governor) * sizeof(dataControl.m_data.m_cpus.at(i).m_Governor[0]));
            }
            return true;
        },dataInfo.m_data.m_cpus);

        return dataControl;
    }

    static DataControl getDataControl(const DataControl::CPUX& value)
    {
        DataControl dataControl;
        dataControl.m_data.m_cpus.fill(value);
        return dataControl;
    }

    static DataControl::CPUX getDataControl(const DataInfo::CPUX& value)
    {
        DataControl::CPUX data;

        data.m_cpuOnline = value.m_cpuOnline;
        memcpy(data.m_Governor,value.m_Governor,sizeof(data.m_Governor) * sizeof(data.m_Governor[0]));

        return data;
    }

    static DataControl getDataControl(const DataInfo& dataControl,const std::array<CPUTopology::ActiveCPUsRange,MAX_SUPPORTED_CPUS> &cpuRange,const DataControl::CPUX& value)
    {
        LenovoLegionDaemon::CPUXControl::DataControl  data = LenovoLegionDaemon::CPUXControl::getDataControl(dataControl);
        LenovoLegionDaemon::CPUXControl::forAllCpuRangeSet(data.m_data.m_cpus,cpuRange,value);
        return data;
    }
};

}
