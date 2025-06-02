// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "SysFsDataProvider.h"
#include "SysFsDriverManager.h"

#include "SysFsDriverLenovo.h"
#include "SysFsDriverIntelPowercapRapl.h"
#include "SysFsDriverCPUXList.h"
#include "SysFsDriverCPUAtom.h"
#include "SysFsDriverCPUCore.h"
#include "SysFsDriverPowerSuplyBattery0.h"
#include "SysFsDriverCPU.h"


#include <Core/LoggerHolder.h>

#include <QFile>
#include <QTextStream>
#include <QCoreApplication>

#include <algorithm>

namespace  LenovoLegionDaemon {


SysFsDataProvider::SysFsDataProvider(SysFsDriverManager* sysFsDriverManager,QObject* parent)  :
    QObject(parent),
    m_sysFsDriverManager(sysFsDriverManager)
{
}


PowerProfile::Control::DataInfo SysFsDataProvider::getPowerProfileData() const {

    PowerProfile::Control::DataInfo data = {
        .m_data = {
            .m_profile = PowerProfile::Control::PowerProfiles::POWER_PROFILE_UNKNOWN
        },
        .m_isAvailable = true
    };

    LOG_D("SysFsDataProvider::getPowerProfileData()");

    try {
        SysFsDriverLenovo::Lenovo::FanControl fanControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLenovo::DRIVER_NAME));

        data.m_data.m_profile = static_cast<PowerProfile::Control::PowerProfiles>(getData(fanControl.m_fanMode).toUShort());
    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D("SysFsDataProvider::getPowerProfileData() - Driver not available");
            data.m_isAvailable = false;
        }
        else
        {
            throw;
        }
    }

    return data;
}

PowerControl::GPU::DataInfo SysFsDataProvider::getGpuControlData()  const {

    PowerControl::GPU::DataInfo  data = {
        .m_data = {
            .m_boostLimit  = 0,
            .m_targetLimit = 0,
            .m_tempLimit   = 0
        },
        .m_isAvailable = true
    };


    LOG_D("SysFsDataProvider::getGpuControlData()");



    try {
        SysFsDriverLenovo::Lenovo::GPUControl gpuControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLenovo::DRIVER_NAME));

        data.m_data.m_boostLimit = static_cast<quint8>(getData(gpuControl.m_gpuBoostLimit).toUShort());
        data.m_data.m_targetLimit = static_cast<quint8>(getData(gpuControl.m_gpuTargetLimit).toUShort());
        data.m_data.m_tempLimit = static_cast<quint8>(getData(gpuControl.m_gpuTempLimit).toUShort());

    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D("SysFsDataProvider::getGpuControlData() - Driver not available");
            data.m_isAvailable = false;
        }
        else
        {
            throw;
        }
    }

    return data;
}

PowerControl::CPU::DataInfo SysFsDataProvider::getCpuControlData()  const  {

    PowerControl::CPU::DataInfo data = {
        .m_data = {
            .m_shortTermPowerLimit = 0,
            .m_longTermPowerLimit  = 0,
            .m_crossLoadingPowerLimit = 0,
            .m_tempLimit = 0,
            .m_l1Tau = 0,
            .m_totalPowerTargetInAC = 0,
            .m_powerBoost = 0
        },
        .m_isAvailable = true
    };


    LOG_D("SysFsDataProvider::getCpuControlData()");

    try {
        SysFsDriverLenovo::Lenovo::CPUControl cpuControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLenovo::DRIVER_NAME));

        data.m_data.m_shortTermPowerLimit = static_cast<quint8>(getData(cpuControl.m_cpuShortTermPowerLimit).toUShort());
        data.m_data.m_longTermPowerLimit = static_cast<quint8>(getData(cpuControl.m_cpuLongTermPowerLimit).toUShort());
        data.m_data.m_crossLoadingPowerLimit = static_cast<quint8>(getData(cpuControl.m_cpuCrossLoadingPowerLimit).toUShort());
        data.m_data.m_tempLimit = static_cast<quint8>(getData(cpuControl.m_cpuTempLimit).toUShort());
        data.m_data.m_l1Tau = static_cast<quint8>(getData(cpuControl.m_cpuL1Tau).toUShort());
        data.m_data.m_totalPowerTargetInAC = static_cast<quint8>(getData(cpuControl.m_cpuTotalPowerTargetInAC).toUShort());
        data.m_data.m_powerBoost = static_cast<quint8>(getData(cpuControl.m_cpuPowerBoost).toUShort());

    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D("SysFsDataProvider::getCpuControlData() - Driver not available");
            data.m_isAvailable = false;
        }
        else
        {
            throw;
        }
    }

    return data;
}


HWMonitoring::DataInfo        SysFsDataProvider::getHWMonitoringData()     const {
    HWMonitoring::DataInfo data = {
        .m_lenovo = {
            .m_data = {
                .m_cpuTemp      = 0,
                .m_gpuTemp      = 0,
                .m_fan1Speed    = 0,
                .m_fan2Speed    = 0,
                .m_fan1MaxSpeed = 0,
                .m_fan2MaxSpeed = 0
            },
            .m_isAvailable = true,
        },
        .m_intelPowerapRapl = {
            .m_data = {
                .m_powercapCPUEnergy = 0,
            },
            .m_isAvailable = true,
        },
        .m_cpusFreq =  {}
    };

    LOG_D("SysFsDataProvider::getHWMonitoringData()");

    try {
        SysFsDriverLenovo::Lenovo::HWMon hwMon(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLenovo::DRIVER_NAME));

        data.m_lenovo.m_data.m_cpuTemp   = getData(hwMon.m_cpuTemp).toUInt();
        data.m_lenovo.m_data.m_gpuTemp   = getData(hwMon.m_gpuTemp).toUInt();
        data.m_lenovo.m_data.m_fan1Speed = getData(hwMon.m_fan1Speed).toUInt();
        data.m_lenovo.m_data.m_fan2Speed = getData(hwMon.m_fan2Speed).toUInt();
        data.m_lenovo.m_data.m_fan1MaxSpeed = getData(hwMon.m_fan1SpeedMax).toUInt();
        data.m_lenovo.m_data.m_fan2MaxSpeed = getData(hwMon.m_fan2SpeedMax).toUInt();

    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            data.m_lenovo.m_isAvailable = false;
        }
        else
        {
            throw;
        }
    }


    try {
        SysFsDriverIntelPowercapRapl::IntelPowercapRapl intelPowerapRapl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverIntelPowercapRapl::DRIVER_NAME));

        data.m_intelPowerapRapl.m_data.m_powercapCPUEnergy = getData(intelPowerapRapl.m_powercapCPUEnergy).toULongLong();

    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            data.m_intelPowerapRapl.m_isAvailable = false;
        }
        else
        {
            throw;
        }
    }

    try {

        SysFsDriverCPUXList::CPUXList cpus(m_sysFsDriverManager->getDriverDescriptorsInVector(SysFsDriverCPUXList::DRIVER_NAME));


        for(size_t i = 0; i < std::min(cpus.cpuList().size(), data.m_cpusFreq.size()) ; ++i)
        {
            bool cpuOnline                      = cpus.cpuList().at(i).isOnlineAvailable() ? getData(cpus.cpuList().at(i).m_cpuOnline.value()).toUShort() == 1 : true;

            if(cpuOnline)
            {
                data.m_cpusFreq[i].m_isAvailable                    = true;
                data.m_cpusFreq[i].m_data.m_cpuBaseFreq                    = cpus.cpuList().at(i).m_freq.m_cpuBaseFreq.has_value() ? getData(cpus.cpuList().at(i).m_freq.m_cpuBaseFreq.value()).toUInt() : 0;
                data.m_cpusFreq[i].m_data.m_cpuInfoMinFreq                 = getData(cpus.cpuList().at(i).m_freq.m_cpuInfoMinFreq).toUInt();
                data.m_cpusFreq[i].m_data.m_cpuInfoMaxFreq                 = getData(cpus.cpuList().at(i).m_freq.m_cpuInfoMaxFreq).toUInt();
                data.m_cpusFreq[i].m_data.m_cpuScalingCurFreq              = getData(cpus.cpuList().at(i).m_freq.m_cpuScalingCurFreq).toUInt();
                data.m_cpusFreq[i].m_data.m_cpuScalingMinFreq              = getData(cpus.cpuList().at(i).m_freq.m_cpuScalingMinFreq).toUInt();
                data.m_cpusFreq[i].m_data.m_cpuScalingMaxFreq              = getData(cpus.cpuList().at(i).m_freq.m_cpuScalingMaxFreq).toUInt();
            }
            else
            {
                data.m_cpusFreq[i].m_isAvailable                    = false;
                data.m_cpusFreq[i].m_data.m_cpuBaseFreq                    = 0;
                data.m_cpusFreq[i].m_data.m_cpuInfoMinFreq                 = 0;
                data.m_cpusFreq[i].m_data.m_cpuInfoMaxFreq                 = 0;
                data.m_cpusFreq[i].m_data.m_cpuScalingCurFreq              = 0;
                data.m_cpusFreq[i].m_data.m_cpuScalingMinFreq              = 0;
                data.m_cpusFreq[i].m_data.m_cpuScalingMaxFreq              = 0;
            }
        }
    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D("SysFsDataProvider::getHWMonitoringData() - Driver not available");
            data.m_cpusFreq.fill({.m_data = {0,0,0,0,0,0},.m_isAvailable = false});
        }
        else
        {
            throw;
        }
    }

    return data;
}

FanControl::Control::DataInfo SysFsDataProvider::getFanControlData() const
{
    FanControl::Control::DataInfo data = {
        .m_data = {
            .m_maxFanSpeed = false,
            .m_LockFan     = false,
        },
        .m_isAvailable = true,
    };

    LOG_D("SysFsDataProvider::getFanControlData()");

    try {
        SysFsDriverLenovo::Lenovo::FanControl fanControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLenovo::DRIVER_NAME));

        data.m_data.m_maxFanSpeed = (getData(fanControl.m_maxFanSpeed).toUShort() == 1);
        data.m_data.m_LockFan     = (getData(fanControl.m_lockFanControl).toUShort() == 1);

    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D("SysFsDataProvider::getFanControlData() - Driver not available");
            data.m_isAvailable = false;
        }
        else
        {
            throw;
        }
    }

    return data;
}

FanControl::CurveControl::DataInfo SysFsDataProvider::getFanCurveControlData() const
{
    FanControl::CurveControl::DataInfo data ={
        .m_data = {
            .m_points = {},
        },
        .m_isAvailable = true,
    };

    LOG_D("SysFsDataProvider::getFanCurveControlData()");

    try {
        SysFsDriverLenovo::Lenovo::HWMon hwMon(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLenovo::DRIVER_NAME));

        auto fanCurveData = getData(hwMon.m_fanCurvePoints).split(',');

        if(data.m_data.m_points.size() != static_cast<size_t>(fanCurveData.size()))
        {
            THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_DATA,std::string("Invalid fan curve data !").c_str());
        }

        for (auto idx = 0; idx < fanCurveData.size(); ++idx) {
            data.m_data.m_points.at(idx) = static_cast<quint8>(fanCurveData[idx].toUShort());
        }
    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D("SysFsDataProvider::getFanCurveControlData() - Driver not available");
            data.m_isAvailable = false;
        }
        else
        {
            throw;
        }
    }

    return data;

}

CPUTopology::Heterogeneous::DataInfo SysFsDataProvider::getCPUTopologyHeterogenousData() const
{
    CPUTopology::Heterogeneous::DataInfo data = {
        .m_data = {
            .m_ActiveCpusCore = {},
            .m_ActiveCpusAtom = {}
        },
        .m_isAvailable = true
    };

    LOG_D("SysFsDataProvider::getCPUTopologyHeterogenousData()");

    try {
        SysFsDriverCPUCore::CPUCore core(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverCPUCore::DRIVER_NAME));
        SysFsDriverCPUAtom::CPUAtom atom(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverCPUAtom::DRIVER_NAME));

        data.m_data.m_ActiveCpusAtom.fill({CPUTopology::NO_RANGE_VALUE,CPUTopology::NO_RANGE_VALUE});
        data.m_data.m_ActiveCpusCore.fill({CPUTopology::NO_RANGE_VALUE,CPUTopology::NO_RANGE_VALUE});

        if(!getData(core.m_cpus).trimmed().isEmpty())
        {
            auto cpusCoreTopology = getData(core.m_cpus).split(',');
            for (size_t i = 0; i < std::min(static_cast<size_t>(cpusCoreTopology.size()),data.m_data.m_ActiveCpusCore.size()); ++i) {

                auto range = cpusCoreTopology.at(i).split('-');

                if(range.size() == 1)
                {
                    data.m_data.m_ActiveCpusCore[i] = {
                        static_cast<quint8>(range.at(0).toUShort()),
                        static_cast<quint8>(range.at(0).toUShort())
                    };
                }
                else if(range.size() == 2)
                {
                    data.m_data.m_ActiveCpusCore[i] = {
                        static_cast<quint8>(range.at(0).toUShort()),
                        static_cast<quint8>(range.at(1).toUShort())
                    };
                }
                else
                {
                    THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_DATA,std::string("Invalid CPU topology data !").c_str());
                }
            }
        }

        if(!getData(atom.m_cpus).trimmed().isEmpty())
        {
            auto cpusAtomTopology = getData(atom.m_cpus).split(',');
            for (size_t i = 0; i < std::min(static_cast<size_t>(cpusAtomTopology.size()),data.m_data.m_ActiveCpusAtom.size()); ++i) {

                auto range = cpusAtomTopology.at(i).split('-');

                if(range.size() == 1)
                {
                    data.m_data.m_ActiveCpusAtom[i] = {
                        static_cast<quint8>(range.at(0).toUShort()),
                        static_cast<quint8>(range.at(0).toUShort())
                    };
                }
                else if(range.size() == 2)
                {
                    data.m_data.m_ActiveCpusAtom[i] = {
                        static_cast<quint8>(range.at(0).toUShort()),
                        static_cast<quint8>(range.at(1).toUShort())
                    };
                }
                else
                {
                    THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_DATA,std::string("Invalid CPU topology data !").c_str());
                }
            }
        }
    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D("SysFsDataProvider::getCPUTopologyHeterogenousData() - Driver not available");
            data.m_isAvailable = false;
        }
        else
        {
            throw;
        }
    }

    return data;
}

CPUTopology::Homogeneous::DataInfo SysFsDataProvider::getCPUTopologyHomogenousData() const
{
    CPUTopology::Homogeneous::DataInfo data = {
        .m_data = {
            .m_ActiveCpus = {}
        },
        .m_isAvailable = true
    };

    LOG_D("SysFsDataProvider::getCPUTopologyHomogenousData()");

    try {
        SysFsDriverCPU::CPU cpu(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverCPU::DRIVER_NAME));

        data.m_data.m_ActiveCpus.fill({CPUTopology::NO_RANGE_VALUE,CPUTopology::NO_RANGE_VALUE});

        if(!getData(cpu.m_topology.m_online).trimmed().isEmpty())
        {
            auto cpusTopology = getData(cpu.m_topology.m_online).split(',');
            for (size_t i = 0; i < std::min(static_cast<size_t>(cpusTopology.size()),data.m_data.m_ActiveCpus.size()); ++i) {

                auto range = cpusTopology.at(i).split('-');

                if(range.size() == 1)
                {
                    data.m_data.m_ActiveCpus[i] = {
                        static_cast<quint8>(range.at(0).toUShort()),
                        static_cast<quint8>(range.at(0).toUShort())
                    };
                }
                else if(range.size() == 2)
                {
                    data.m_data.m_ActiveCpus[i] = {
                        static_cast<quint8>(range.at(0).toUShort()),
                        static_cast<quint8>(range.at(1).toUShort())
                    };
                }
                else
                {
                    THROW_EXCEPTION(exception_T,ERROR_CODES::INVALID_DATA,std::string("Invalid CPU topology data !").c_str());
                }
            }
        }

    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D("SysFsDataProvider::getCPUTopologyHomogenousData() - Driver not available");
            data.m_isAvailable = false;
        }
        else
        {
            throw;
        }
    }

    return data;
}

CPUXFreqControl::DataInfo SysFsDataProvider::getCPUsInfoData() const
{
    CPUXFreqControl::DataInfo data ={
        .m_data = {
            .m_cpus = {}
        },
        .m_isAvailable = true
    };

    LOG_D("SysFsDataProvider::getCPUsInfoData()");

    try {
        SysFsDriverCPUXList::CPUXList cpuXlist(m_sysFsDriverManager->getDriverDescriptorsInVector(SysFsDriverCPUXList::DRIVER_NAME));

        for(size_t i = 0; i < std::min(cpuXlist.cpuList().size(),data.m_data.m_cpus.size()) ; ++i)
        {
            data.m_data.m_cpus.at(i).m_cpuOnline                      = cpuXlist.cpuList().at(i).isOnlineAvailable() ? getData(cpuXlist.cpuList().at(i).m_cpuOnline.value()).toUShort() == 1 : true;

            if(data.m_data.m_cpus.at(i).m_cpuOnline)
            {
                data.m_data.m_cpus.at(i).m_cpuBaseFreq                    = cpuXlist.cpuList().at(i).m_freq.m_cpuBaseFreq.has_value() ? getData(cpuXlist.cpuList().at(i).m_freq.m_cpuBaseFreq.value()).toUInt() : 0;
                data.m_data.m_cpus.at(i).m_cpuInfoMinFreq                 = getData(cpuXlist.cpuList().at(i).m_freq.m_cpuInfoMinFreq).toUInt();
                data.m_data.m_cpus.at(i).m_cpuInfoMaxFreq                 = getData(cpuXlist.cpuList().at(i).m_freq.m_cpuInfoMaxFreq).toUInt();
                data.m_data.m_cpus.at(i).m_cpuScalingCurFreq              = getData(cpuXlist.cpuList().at(i).m_freq.m_cpuScalingCurFreq).toUInt();
                data.m_data.m_cpus.at(i).m_cpuScalingMinFreq              = getData(cpuXlist.cpuList().at(i).m_freq.m_cpuScalingMinFreq).toUInt();
                data.m_data.m_cpus.at(i).m_cpuScalingMaxFreq              = getData(cpuXlist.cpuList().at(i).m_freq.m_cpuScalingMaxFreq).toUInt();
                data.m_data.m_cpus.at(i).m_cpuCoreId                      = getData(cpuXlist.cpuList().at(i).m_topology.m_coreId).toUInt();
                data.m_data.m_cpus.at(i).m_dieId                          = getData(cpuXlist.cpuList().at(i).m_topology.m_dieId).toUInt();
                data.m_data.m_cpus.at(i).m_physicalPackageId              = getData(cpuXlist.cpuList().at(i).m_topology.m_physicalPackageId).toUInt();
                data.m_data.m_cpus.at(i).m_clusterId                      = getData(cpuXlist.cpuList().at(i).m_topology.m_clusterId).toUInt();
            }
            else
            {
                data.m_data.m_cpus.at(i).m_cpuBaseFreq                    = 0;
                data.m_data.m_cpus.at(i).m_cpuInfoMinFreq                 = 0;
                data.m_data.m_cpus.at(i).m_cpuInfoMaxFreq                 = 0;
                data.m_data.m_cpus.at(i).m_cpuScalingCurFreq              = 0;
                data.m_data.m_cpus.at(i).m_cpuScalingMinFreq              = 0;
                data.m_data.m_cpus.at(i).m_cpuScalingMaxFreq              = 0;
                data.m_data.m_cpus.at(i).m_cpuCoreId                      = 0;
                data.m_data.m_cpus.at(i).m_dieId                          = 0;
                data.m_data.m_cpus.at(i).m_physicalPackageId              = 0;
                data.m_data.m_cpus.at(i).m_clusterId                      = 0;
            }
        }
    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D("SysFsDataProvider::getCPUsInfoData() - Driver not available");
            data.m_isAvailable = false;
        }
        else
        {
            throw;
        }
    }

    return data;
}

CPUXControl::DataInfo SysFsDataProvider::getCPUsInfoControlData() const
{
    CPUXControl::DataInfo data = {
        .m_data = {
            .m_cpus = {}
        },
        .m_isAvailable = true
    };

    LOG_D("SysFsDataProvider::getCPUsInfoGavernorData()");
    try {
        SysFsDriverCPUXList::CPUXList cpuXlist(m_sysFsDriverManager->getDriverDescriptorsInVector(SysFsDriverCPUXList::DRIVER_NAME));

        for(size_t i = 0; i < std::min(cpuXlist.cpuList().size(),data.m_data.m_cpus.size()) ; ++i)
        {
            data.m_data.m_cpus.at(i).m_cpuOnline = cpuXlist.cpuList().at(i).isOnlineAvailable() ? getData(cpuXlist.cpuList().at(i).m_cpuOnline.value()).toUShort() == 1 : true;

            if(data.m_data.m_cpus.at(i).m_cpuOnline)
            {
                auto cpuScalingAvailableGovernors = getData(cpuXlist.cpuList().at(i).m_freq.m_cpuScalingAvailableGovernors).toStdString();
                std::memset(data.m_data.m_cpus.at(i).m_availableGovernors,0,sizeof(data.m_data.m_cpus.at(i).m_availableGovernors) * sizeof(data.m_data.m_cpus.at(i).m_availableGovernors[0]));
                std::strncpy(data.m_data.m_cpus.at(i).m_availableGovernors,cpuScalingAvailableGovernors.data(),sizeof(data.m_data.m_cpus.at(i).m_availableGovernors) * sizeof(data.m_data.m_cpus.at(i).m_availableGovernors[0]) - 1);

                auto cpuScalingGovernor = getData(cpuXlist.cpuList().at(i).m_freq.m_cpuScalingGovernor).toStdString();
                std::memset(data.m_data.m_cpus.at(i).m_Governor,0,sizeof(data.m_data.m_cpus.at(i).m_Governor) * sizeof(data.m_data.m_cpus.at(i).m_Governor[0]));
                std::strncpy(data.m_data.m_cpus.at(i).m_Governor,cpuScalingGovernor.data(),sizeof(data.m_data.m_cpus.at(i).m_Governor) * sizeof(data.m_data.m_cpus.at(i).m_Governor[0]) - 1);
                data.m_data.m_cpus.at(i).m_cpuCoreId = getData(cpuXlist.cpuList().at(i).m_topology.m_coreId).toUInt();
                data.m_data.m_cpus.at(i).m_dieId = getData(cpuXlist.cpuList().at(i).m_topology.m_dieId).toUInt();
                data.m_data.m_cpus.at(i).m_physicalPackageId = getData(cpuXlist.cpuList().at(i).m_topology.m_physicalPackageId).toUInt();
                data.m_data.m_cpus.at(i).m_clusterId = getData(cpuXlist.cpuList().at(i).m_topology.m_clusterId).toUInt();
            }
            else
            {
                std::memset(data.m_data.m_cpus.at(i).m_availableGovernors,0,sizeof(data.m_data.m_cpus.at(i).m_availableGovernors) * sizeof(data.m_data.m_cpus.at(i).m_availableGovernors[0]));
                std::memset(data.m_data.m_cpus.at(i).m_Governor,0,sizeof(data.m_data.m_cpus.at(i).m_Governor) * sizeof(data.m_data.m_cpus.at(i).m_Governor[0]));
                data.m_data.m_cpus.at(i).m_cpuCoreId         = 0;
                data.m_data.m_cpus.at(i).m_dieId             = 0;
                data.m_data.m_cpus.at(i).m_physicalPackageId = 0;
                data.m_data.m_cpus.at(i).m_clusterId         = 0;
            }
        }
    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D("SysFsDataProvider::getCPUsInfoGavernorData() - Driver not available");
            data.m_isAvailable = false;
        }
        else
        {
            throw;
        }
    }

    return data;
}

Batery::DataInfo SysFsDataProvider::getBateryControlData() const
{
    Batery::DataInfo data={
        .m_data = {
            .m_powerChargeMode = Batery::PowerChargeMode::POWER_CHARGE_MODE_AC,
            .m_batteryStatus = {0}
        },
        .m_isAvailable = true,
    };


    LOG_D("SysFsDataProvider::getBateryControlData()");

    try {
        SysFsDriverPowerSuplyBattery0::PowerSuplyBattery0 batery0(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverPowerSuplyBattery0::DRIVER_NAME));
        SysFsDriverLenovo::Lenovo::PowerCharge powerCharge(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLenovo::DRIVER_NAME));

        data.m_data.m_powerChargeMode = static_cast<Batery::PowerChargeMode>(getData(powerCharge.m_powerChargeMode).toUShort());

        auto batteryStatus = getData(batery0.m_powerSuplyBattery0).toStdString();
        std::copy_n(batteryStatus.begin(),std::min(batteryStatus.size(),data.m_data.m_batteryStatus.size() - 1),data.m_data.m_batteryStatus.begin());

    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D("SysFsDataProvider::getBateryControlData() - Driver not available");
            data.m_isAvailable = false;
        }
        else
        {
            throw;
        }
    }


    return data;
}

CPUSMTControl::DataInfo SysFsDataProvider::getCPUSMTControlData() const
{
    CPUSMTControl::DataInfo data = {
        .m_data ={
            .m_active = 0,
            .m_control = {0}
        },
        .m_isAvailable = true
    };

    LOG_D("SysFsDataProvider::getCPUSMTControlData()");


    try {
        SysFsDriverCPU::CPU::Smt smtControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverCPU::DRIVER_NAME));

        std::strncpy(data.m_data.m_control, getData(smtControl.m_control.value()).toStdString().c_str(), sizeof(data.m_data.m_control) * sizeof(data.m_data.m_control[0]) - 1);
        data.m_data.m_active = (getData(smtControl.m_active.value()).toUShort() == 1);

    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D("SysFsDataProvider::getCPUSMTControlData() - Driver not available");
            data.m_isAvailable = false;
        }
        else
        {
            throw;
        }
    }

    return data;
}

void SysFsDataProvider::setPowerProfileData(const PowerProfile::Control::DataControl &data)
{
    SysFsDriverLenovo::Lenovo::FanControl fanControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLenovo::DRIVER_NAME));

    LOG_D("SysFsDataProvider::setPowerProfileData()");

    setData(fanControl.m_fanMode,static_cast<quint8>(data.m_data.m_profile));
}

void SysFsDataProvider::setGpuControlData(const PowerControl::GPU::DataControl &data)
{
    SysFsDriverLenovo::Lenovo::GPUControl gpuControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLenovo::DRIVER_NAME));

    setData(gpuControl.m_gpuBoostLimit,data.m_data.m_boostLimit);
    setData(gpuControl.m_gpuTargetLimit,data.m_data.m_targetLimit);
    setData(gpuControl.m_gpuTempLimit,data.m_data.m_tempLimit);
}

void SysFsDataProvider::setCpuControlData(const PowerControl::CPU::DataControl &data)
{
    SysFsDriverLenovo::Lenovo::CPUControl cpuControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLenovo::DRIVER_NAME));

    LOG_D("SysFsDataProvider::setCpuControlData()");

    setData(cpuControl.m_cpuTempLimit,data.m_data.m_tempLimit);
    setData(cpuControl.m_cpuCrossLoadingPowerLimit,data.m_data.m_crossLoadingPowerLimit);
    setData(cpuControl.m_cpuLongTermPowerLimit,data.m_data.m_longTermPowerLimit);
    setData(cpuControl.m_cpuShortTermPowerLimit,data.m_data.m_shortTermPowerLimit);
    setData(cpuControl.m_cpuL1Tau,data.m_data.m_l1Tau);
    setData(cpuControl.m_cpuTotalPowerTargetInAC,data.m_data.m_totalPowerTargetInAC);
    setData(cpuControl.m_cpuPowerBoost,data.m_data.m_powerBoost);
}

void SysFsDataProvider::setCpuFreqControlData(const CPUXFreqControl::DataControl &data)
{
    SysFsDriverCPUXList::CPUXList cpuXlist(m_sysFsDriverManager->getDriverDescriptorsInVector(SysFsDriverCPUXList::DRIVER_NAME));

    LOG_D("SysFsDataProvider::setCpuFreqControlData()");

    for(size_t i = 0; i < std::min(cpuXlist.cpuList().size(),data.m_data.m_cpus.size()) ; ++i)
    {
        setData(cpuXlist.cpuList().at(i).m_freq.m_cpuScalingMinFreq,data.m_data.m_cpus.at(i).m_cpuScalingMinFreq);
        setData(cpuXlist.cpuList().at(i).m_freq.m_cpuScalingMaxFreq,data.m_data.m_cpus.at(i).m_cpuScalingMaxFreq);
    }
}

void SysFsDataProvider::setFanControlData(const FanControl::Control::DataControl &data)
{
    SysFsDriverLenovo::Lenovo::FanControl fanControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLenovo::DRIVER_NAME));

    LOG_D("SysFsDataProvider::setFanControlData()");

    setData(fanControl.m_maxFanSpeed,data.m_data.m_maxFanSpeed);
    setData(fanControl.m_lockFanControl,data.m_data.m_LockFan);
}

void SysFsDataProvider::setFanCurveControlData(const FanControl::CurveControl::DataControl &data)
{
    SysFsDriverLenovo::Lenovo::HWMon hwMon(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLenovo::DRIVER_NAME));

    setData(hwMon.m_fanCurvePoints,std::vector<quint8>(data.m_data.m_points.begin(),data.m_data.m_points.end()));
}

void SysFsDataProvider::setCpuControlData(const CPUXControl::DataControl &data)
{
    SysFsDriverCPUXList::CPUXList cpuXlist(m_sysFsDriverManager->getDriverDescriptorsInVector(SysFsDriverCPUXList::DRIVER_NAME));

    LOG_D("SysFsDataProvider::setCpuControlData()");

    m_sysFsDriverManager->blockKernelEvent(SysFsDriverCPUXList::DRIVER_NAME,true);


    auto cleanup =  qScopeGuard([this] { m_sysFsDriverManager->blockKernelEvent(SysFsDriverCPUXList::DRIVER_NAME,false); });

    for(size_t i = 0; i < std::min(cpuXlist.cpuList().size(),data.m_data.m_cpus.size()) ; ++i)
    {
        if(i == (std::min(cpuXlist.cpuList().size(),data.m_data.m_cpus.size()) - 1))
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents,100);
        }

        if(cpuXlist.cpuList().at(i).isOnlineAvailable())
        {
            setData(cpuXlist.cpuList().at(i).m_cpuOnline.value(),data.m_data.m_cpus.at(i).m_cpuOnline);
        }
        setData(cpuXlist.cpuList().at(i).m_freq.m_cpuScalingGovernor,std::string(data.m_data.m_cpus.at(i).m_Governor));
    }

}

void SysFsDataProvider::setCPUSMTControlData(const CPUSMTControl::DataControl &data)
{
    SysFsDriverCPU::CPU::Smt smtControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverCPU::DRIVER_NAME));

    LOG_D("SysFsDataProvider::setCPUSMTControlData()");

    setData(smtControl.m_control.value(),std::string(data.m_data.m_control));
}

QString SysFsDataProvider::getData(const std::filesystem::path &path) const
{
    QFile file(path);

    if(!file.open(QIODeviceBase::ReadOnly))
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::OPEN_FOR_READING_ERROR,std::string("I can not open file (").append(path.string()).append(") with permision=ReadOnly !").c_str());
    }

    return QTextStream(&file).readAll();
}
void SysFsDataProvider::setData(const std::filesystem::path &path, quint8 value)
{
    QFile file(path);

    if(!file.open(QIODeviceBase::WriteOnly))
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::OPEN_FOR_WRITING_ERROR,std::string("I can not open file (").append(path.string()).append(") with permision=WriteOnly !").c_str());
    }

    QTextStream(&file) << value;
}

void SysFsDataProvider::setData(const std::filesystem::path &path, quint32 value)
{
    QFile file(path);

    if(!file.open(QIODeviceBase::WriteOnly))
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::OPEN_FOR_WRITING_ERROR,std::string("I can not open file (").append(path.string()).append(") with permision=WriteOnly !").c_str());
    }

    QTextStream(&file) << value;
}

void SysFsDataProvider::setData(const std::filesystem::path &path, bool value)
{
    QFile file(path);

    if(!file.open(QIODeviceBase::WriteOnly))
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::OPEN_FOR_WRITING_ERROR,std::string("I can not open file (").append(path.string()).append(") with permision=WriteOnly !").c_str());
    }

    QTextStream(&file) << (quint8)(value ? 1 : 0);
}

void SysFsDataProvider::setData(const std::filesystem::path &path, const std::vector<quint8>& values)
{
    QFile file(path);
    QTextStream   out(&file);

    if(!file.open(QIODeviceBase::WriteOnly))
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::OPEN_FOR_WRITING_ERROR,std::string("I can not open file (").append(path.string()).append(") with permision=WriteOnly !").c_str());
    }

    for (size_t index = 0; index < values.size(); ++index) {

        if (index == values.size() - 1)
        {
            out << values[index];
        }
        else
        {
            out << values[index] << ",";
        }
    }
}

void SysFsDataProvider::setData(const std::filesystem::path &path, const std::string &value)
{
    QFile file(path);

    if(!file.open(QIODeviceBase::WriteOnly))
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::OPEN_FOR_WRITING_ERROR,std::string("I can not open file (").append(path.c_str()).append(") with permision=WriteOnly !").c_str());
    }

    QTextStream(&file) << value.c_str();
}


}

