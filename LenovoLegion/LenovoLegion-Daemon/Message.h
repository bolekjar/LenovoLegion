// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include "ControlData.h"

#include <QtTypes>
#include <QString>



namespace LenovoLegionDaemon {



struct Message {

    enum Type : quint8 {

        REQUEST_GET_DATA   = 0,
        REQUEST_SET_DATA   = 1,

        RESPONSE_GET_DATA  = 2,
        RESPONSE_SET_DATA  = 3,

        NOTIFICATION       = 4
    };

    enum  DataType : quint8 {
        HW_MONITORING_DATA                       = 0,
        FAN_CURVE_CONTROL_DATA                   = 1,
        FANCONTROL_DATA                          = 2,
        GPU_CONTROL_DATA                         = 3,
        CPU_CONTROL_DATA                         = 4,
        POWER_PROFILE_CONTROL_DATA               = 5,
        BATTERY_CONTROL_DATA                     = 6,
        CPU_HETEROGENOUS_TOPOLOGY_DATA           = 7,
        CPU_HOMOGENOUS_TOPOLOGY_DATA             = 8,
        CPUS_FREQ_CONTROL_DATA                   = 9,
        CPUS_CONTROL_DATA                        = 10,
        CPUS_SMT_CONTROL_DATA                    = 11,

        TEXT                                     = 12,
        ENUMERATION                              = 13
    };

    enum EnumerationType : quint8 {

        GET_HW_MONITORING_DATA                     = 0,
        GET_FAN_CURVE_CONTROL_DATA                 = 1,
        GET_FANCONTROL_DATA                        = 2,
        GET_GPU_CONTROL_DATA                       = 3,
        GET_CPU_CONTROL_DATA                       = 4,
        GET_POWER_PROFILE_CONTROL_DATA             = 5,
        GET_BATTERY_CONTROL_DATA                   = 6,
        GET_CPU_HETEROGENOUS_TOPOLOGY_DATA         = 7,
        GET_CPU_HOMOGENOUS_TOPOLOGY_DATA           = 8,
        GET_CPUS_FREQ_CONTROL_DATA                 = 9,
        GET_CPUS_CONTROL_DATA                      = 10,
        GET_CPU_SMT_CONTROL_DATA                   = 11,

        INF_LENOVO_DRIVER_REMOVED           = 100,
        INF_LENOVO_DRIVER_ADDED             = 101,
        INF_ACPI_PLATFORM_PROFILE_CHANGE    = 102,
        INF_POWER_SUPPLY_BATTERY0_CHANGE    = 103,
        INF_CPU_X_LIST_RELOADED             = 104,

        SET_DATA_SUCESS                 = 200,
        SET_DATA_FAIL                   = 201
    };

    union Data {
        HWMonitoring::DataInfo                   m_hwMonitoringData;

        FanControl::CurveControl::DataInfo       m_fanCurveControlData;
        FanControl::CurveControl::DataControl    m_fanCurveControlDataControl;

        FanControl::Control::DataInfo            m_fanControlData;
        FanControl::Control::DataControl         m_fanControlDataControl;


        PowerControl::GPU::DataInfo              m_gpuControlData;
        PowerControl::GPU::DataControl           m_gpuControlDataControl;

        PowerControl::CPU::DataInfo              m_cpuControlData;
        PowerControl::CPU::DataControl           m_cpuControlDataControl;


        PowerProfile::Control::DataInfo          m_powerProfileControlData;
        PowerProfile::Control::DataControl       m_powerProfileControlDataControl;

        Text                                     m_text;
        EnumerationType                          m_enumType;
        Batery::DataInfo                         m_bateryControlData;


        CPUTopology::Heterogeneous::DataInfo     m_cpuHeteregenousTopologyData;
        CPUTopology::Homogeneous::DataInfo       m_cpuHomogenousTopologyData;

        CPUXFreqControl::DataInfo                m_cpusFreqControlData;
        CPUXFreqControl::DataControl             m_cpusFreqControlDataControl;

        CPUXControl::DataInfo                    m_cpusControlData;
        CPUXControl::DataControl                 m_cpusControlDataControl;

        CPUSMTControl::DataInfo                  m_cpusSmtControlData;
        CPUSMTControl::DataControl               m_cpusSmtControlDataControl;
    };


    /*
     * Header
     */
    Type        m_type;
    DataType    m_dataType;

    /*
     * Data
     */
    Data        m_data;
};


}
