// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <QObject>

#include "ControlDataProvider.h"


#include <Core/ExceptionBuilder.h>

#include <QFile>

#include <vector>

namespace  LenovoLegionDaemon {


class SysFsDriverManager;

class SysFsDataProvider : public QObject,
                         public ControlDataProvider
{

    Q_OBJECT

public:

    DEFINE_EXCEPTION(SysFsData);


    enum ERROR_CODES : int {
        OPEN_FOR_READING_ERROR    = -1,
        OPEN_FOR_WRITING_ERROR    = -2,
        INVALID_DATA              = -3,
    };

public:

    SysFsDataProvider(SysFsDriverManager* sysFsDriverManager, QObject* parent = nullptr);

    PowerProfile::Control::DataInfo        getPowerProfileData()     const override;
    PowerControl::GPU::DataInfo            getGpuControlData()       const override;
    PowerControl::CPU::DataInfo            getCpuControlData()       const override;
    HWMonitoring::DataInfo                 getHWMonitoringData()     const override;

    CPUTopology::Heterogeneous::DataInfo   getCPUTopologyHeterogenousData()      const override;
    CPUTopology::Homogeneous::DataInfo     getCPUTopologyHomogenousData()        const override;


    CPUXFreqControl::DataInfo              getCPUsInfoData()         const override;


    CPUXControl::DataInfo                  getCPUsInfoControlData()  const override;

    FanControl::Control::DataInfo          getFanControlData()       const override;
    FanControl::CurveControl::DataInfo     getFanCurveControlData()  const override;

    Batery::DataInfo                       getBateryControlData()    const override;

    CPUSMTControl::DataInfo                getCPUSMTControlData()    const override;


    void setPowerProfileData(const PowerProfile::Control::DataControl& data)         override;
    void setGpuControlData(const PowerControl::GPU::DataControl& data)               override;
    void setCpuControlData(const PowerControl::CPU::DataControl& data)               override;
    void setCpuFreqControlData(const CPUXFreqControl::DataControl& data)             override;

    void setFanControlData(const FanControl::Control::DataControl& data)             override;
    void setFanCurveControlData(const FanControl::CurveControl::DataControl& data)   override;
    void setCpuControlData(const CPUXControl::DataControl& data)                     override;

    void setCPUSMTControlData(const CPUSMTControl::DataControl& data)                override;

private:

    QString getData(const std::filesystem::path& path) const;

    void    setData(const std::filesystem::path& path,quint8 value);
    void    setData(const std::filesystem::path& path,quint32 value);
    void    setData(const std::filesystem::path& path,bool value);
    void    setData(const std::filesystem::path& path,const std::vector<quint8>& values);
    void    setData(const std::filesystem::path& path,const std::string& values);

private:

    SysFsDriverManager* m_sysFsDriverManager;
};

};
