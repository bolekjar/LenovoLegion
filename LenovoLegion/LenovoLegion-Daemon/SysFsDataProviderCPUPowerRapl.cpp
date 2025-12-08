// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "SysFsDataProviderCPUPowerRapl.h"

#include "SysFsDriverIntelPowercapRapl.h"


#include "Core/LoggerHolder.h"

#include "../LenovoLegion-PrepareBuild/CpuPowerRapl.pb.h"


namespace LenovoLegionDaemon {


SysFsDataProviderCPUPowerRapl::SysFsDataProviderCPUPowerRapl(SysFsDriverManager* sysFsDriverManager,QObject* parent) : SysFsDataProvider(sysFsDriverManager,parent,dataType) {}


QByteArray SysFsDataProviderCPUPowerRapl::serializeAndGetData() const
{
    legion::messages::CPUPowerRapl         cpuPowerRapl;
    QByteArray                             byteArray;

    LOG_D(__PRETTY_FUNCTION__);

    try {
        SysFsDriverIntelPowercapRapl::IntelPowercapRapl cpuRaplControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverIntelPowercapRapl::DRIVER_NAME));

        auto ltp_power_limit = cpuPowerRapl.mutable_ltp_limit();


        ltp_power_limit->set_max_power_uw(getData(cpuRaplControl.m_ltp_max_power_uw).toULongLong());
        ltp_power_limit->set_power_limit_uw(getData(cpuRaplControl.m_ltp_power_limit_uw).toULongLong());
        ltp_power_limit->set_name(getData(cpuRaplControl.m_ltp_name).toStdString());
        ltp_power_limit->set_time_window_us(getData(cpuRaplControl.m_ltp_time_window_us).toULongLong());



        auto stp_power_limit = cpuPowerRapl.mutable_stp_limit();

        stp_power_limit->set_max_power_uw(getData(cpuRaplControl.m_stp_max_power_uw).toULongLong() );
        stp_power_limit->set_power_limit_uw(getData(cpuRaplControl.m_stp_power_limit_uw).toULongLong());
        stp_power_limit->set_name(getData(cpuRaplControl.m_stp_name).toStdString());
        stp_power_limit->set_time_window_us(getData(cpuRaplControl.m_stp_time_window_us).toULongLong());


        auto pp_power_limit = cpuPowerRapl.mutable_pp_limit();

        pp_power_limit->set_max_power_uw(getData(cpuRaplControl.m_pp_max_power_uw).toULongLong());
        pp_power_limit->set_power_limit_uw(getData(cpuRaplControl.m_pp_power_limit_uw).toULongLong());
        pp_power_limit->set_name(getData(cpuRaplControl.m_pp_name).toStdString());
        pp_power_limit->set_time_window_us(getData(cpuRaplControl.m_pp_time_window_us).toULongLong());

    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D(QString(__PRETTY_FUNCTION__) + "- Driver not available");
            cpuPowerRapl.Clear();
        }
        else
        {
            throw;
        }
    }

    byteArray.resize(cpuPowerRapl.ByteSizeLong());
    if(!cpuPowerRapl.SerializeToArray(byteArray.data(),byteArray.size()))
    {
        THROW_EXCEPTION(exception_T,DataProvider::ERROR_CODES::SERIALIZE_ERROR,"Serialize of data message error !");
    }


    return byteArray;
}


QByteArray SysFsDataProviderCPUPowerRapl::deserializeAndSetData(const QByteArray &data)
{
    SysFsDriverIntelPowercapRapl::IntelPowercapRapl cpuRaplControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverIntelPowercapRapl::DRIVER_NAME));
    legion::messages::CPUPowerRapl                  cpuPowerRapl;

    LOG_D(__PRETTY_FUNCTION__);


    if(!cpuPowerRapl.ParseFromArray(data.data(),data.size()))
    {
        THROW_EXCEPTION(exception_T,DataProvider::ERROR_CODES::INVALID_DATA,"Parse of data message error !");
    }


    if(cpuPowerRapl.has_ltp_limit())
    {
        if(cpuPowerRapl.ltp_limit().has_power_limit_uw())
        {
            setData(cpuRaplControl.m_ltp_power_limit_uw,static_cast<quint64>(cpuPowerRapl.ltp_limit().power_limit_uw()));
        };
        if(cpuPowerRapl.ltp_limit().has_time_window_us())
        {
            setData(cpuRaplControl.m_ltp_time_window_us,static_cast<quint64>(cpuPowerRapl.ltp_limit().time_window_us()));
        };
    }

    if(cpuPowerRapl.has_stp_limit())
    {
        if(cpuPowerRapl.stp_limit().has_power_limit_uw())
        {
            setData(cpuRaplControl.m_stp_power_limit_uw,static_cast<quint64>(cpuPowerRapl.stp_limit().power_limit_uw()));
        };
        if(cpuPowerRapl.stp_limit().has_time_window_us())
        {
            setData(cpuRaplControl.m_stp_time_window_us,static_cast<quint64>(cpuPowerRapl.stp_limit().time_window_us()));
        };
    }

    if(cpuPowerRapl.has_pp_limit())
    {
        if(cpuPowerRapl.pp_limit().has_power_limit_uw())
        {
            setData(cpuRaplControl.m_pp_power_limit_uw,static_cast<quint64>(cpuPowerRapl.pp_limit().power_limit_uw()));
        };
        if(cpuPowerRapl.pp_limit().has_time_window_us())
        {
            setData(cpuRaplControl.m_pp_time_window_us,static_cast<quint64>(cpuPowerRapl.pp_limit().time_window_us()));
        };
    }

    return {};

}


}


