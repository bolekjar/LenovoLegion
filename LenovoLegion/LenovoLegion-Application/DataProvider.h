// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <QObject>

#include <ControlData.h>

namespace LenovoLegionGui {


class PowerControlDataProvider;
class FanControlDataProvider;
class HWMonitoringDataProvider;
class ProtocolProcessor;
class CPUControlDataProvider;
class CPUFrequencyControlDataProvider;
class PowerProfileControlDataProvider;
class BateryStatusDataProvider;

struct DataProvider : QObject
{
    Q_OBJECT

public:

    struct Message {

        enum class Type : quint8 {
            NOTIFICATION_CLIENT_IS_CONNECTED    = 0,
            NOTIFICATION_CLIENT_IS_DISCONNECTED = 1,
            NOTIFICATION_POWER_PROFILE_CHANGED  = 2,
            NOTIFICATION_BATTERY_STATUS_CHANGED = 3,
            NOTIFICATION_LENOVO_DRIVER_ADDED    = 4,
            NOTIFICATION_LENOVO_DRIVER_REMOVED  = 5,
            NOTIFICATION_CPU_X_LIST_RELOADED    = 6,
        };

        Type m_type;

    };

    DataProvider(QObject *parent) : QObject(parent) {}

    virtual ~DataProvider() = default;

    virtual BateryStatusDataProvider*           getBateryStatusDataProvider()                            = 0;
    virtual PowerControlDataProvider*           getPowerControlDataProvider()                            = 0;
    virtual FanControlDataProvider*             getFanControlDataProvider()                              = 0;
    virtual HWMonitoringDataProvider*           getHWMonitoringDataProvider()                            = 0;
    virtual CPUControlDataProvider*             getCPUControlDataProvider()                              = 0;
    virtual CPUFrequencyControlDataProvider*    getCPUFrequencyControlDataProvider()                     = 0;
    virtual PowerProfileControlDataProvider*    getPowerProfileControlDataProvider()                     = 0;

signals:

    void dataProviderEvent(const LenovoLegionGui::DataProvider::Message& event);
};


}
