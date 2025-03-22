// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <ControlData.h>
#include <DataProvider.h>

#include <QObject>



namespace  LenovoLegionGui {


class ProtocolProcessor;



class FanControlDataProvider : public QObject
{
    Q_OBJECT
public:

    explicit FanControlDataProvider(ProtocolProcessor * protocolProcessor,QObject *parent = nullptr);

    ~FanControlDataProvider() = default;

    LenovoLegionDaemon::FanControl::Control::DataInfo          getFanControlData()       const;
    LenovoLegionDaemon::FanControl::CurveControl::DataInfo     getFanCurveControlData()  const;


    void setFanControlData(const LenovoLegionDaemon::FanControl::Control::DataControl& data);
    void setFanCurveControlData(const LenovoLegionDaemon::FanControl::CurveControl::DataControl& data);

signals:

private:

    ProtocolProcessor*  m_protocolProcessor;
};

}
