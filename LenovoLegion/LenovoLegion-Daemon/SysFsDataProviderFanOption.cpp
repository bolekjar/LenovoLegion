// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "SysFsDataProviderFanOption.h"
#include "SysFsDriverLegionEc.h"


#include "../LenovoLegion-PrepareBuild/FanControl.pb.h"


#include <Core/LoggerHolder.h>


namespace LenovoLegionDaemon {

SysFsDataProviderFanOption::SysFsDataProviderFanOption(SysFsDriverManager* sysFsDriverManager,QObject* parent) : SysFsDataProvider(sysFsDriverManager,parent,dataType) {}


QByteArray SysFsDataProviderFanOption::serializeAndGetData() const
{
    legion::messages::FanOption         fanOptionMsg;
    QByteArray                          byteArray;

    LOG_D(__PRETTY_FUNCTION__);


    try {
        SysFsDriverLegionEc::EC::FanControl fanControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLegionEc::DRIVER_NAME));

        fanOptionMsg.set_full_speed(getData(fanControl.m_full_speed).toUShort() == 1);
        fanOptionMsg.set_lock(getData(fanControl.m_lock).toUShort() == 1);
    } catch(SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::ERROR_CODES::DRIVER_NOT_AVAILABLE)
        {
            LOG_D(QString(__PRETTY_FUNCTION__) + "- Driver not available");
            fanOptionMsg.Clear();
        }
        else
        {
            throw;
        }
    }

    byteArray.resize(fanOptionMsg.ByteSizeLong());
    if(!fanOptionMsg.SerializeToArray(byteArray.data(),byteArray.size()))
    {
        THROW_EXCEPTION(exception_T,DataProvider::ERROR_CODES::SERIALIZE_ERROR,"Serialize of data message error !");
    }

    return byteArray;
}

QByteArray SysFsDataProviderFanOption::deserializeAndSetData(const QByteArray &data)
{
    SysFsDriverLegionEc::EC::FanControl fanControl(m_sysFsDriverManager->getDriverDesriptor(SysFsDriverLegionEc::DRIVER_NAME));
    legion::messages::FanOption         fanOptionMsg;

    LOG_D(__PRETTY_FUNCTION__);

    if(!fanOptionMsg.ParseFromArray(data.data(),data.size()))
    {
        THROW_EXCEPTION(exception_T,DataProvider::ERROR_CODES::INVALID_DATA,"Parse of data message error !");
    }

    if(fanOptionMsg.has_full_speed()) {
        setData(fanControl.m_full_speed,fanOptionMsg.full_speed());
    }
    if(fanOptionMsg.has_lock()) {
        setData(fanControl.m_lock,fanOptionMsg.lock());
    }

    return {};
}


}
