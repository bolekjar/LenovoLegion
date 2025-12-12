// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <Core/ExceptionBuilder.h>

#include "../LenovoLegion-Daemon/RGBControllerInterface.h"
#include "../LenovoLegion-PrepareBuild/RGBController.pb.h"


namespace LenovoLegionGui {

class DataProvider;

class RGBController : public LenovoLegionDaemon::RGBControllerInterface
{
    enum PENDING_CHANGES : int {
        CHANGE_LEDS            = 0,
        CHANGE_MODES           = 1,
        CHANGE_PROFILES        = 2,
        CHANGE_ACTIVE_MODE     = 3,
        CHANGE_COLORS          = 4,
        MAXIMUM_CHANGES        = 5
    };

public:
    DEFINE_EXCEPTION(RGBController)

    enum ERROR_CODES : int {
        DATA_NOT_AVAILABLE = -1,
        INVALID_INDEX = -2
    };

public:
    explicit RGBController(DataProvider* dataProvider);
    virtual ~RGBController() override;

    // Refresh data from daemon
    void refresh();

    // RGBControllerInterface implementation
    virtual unsigned int    GetLEDsInZone(unsigned int zone)  const                                             override;

    virtual std::string     GetModeName(unsigned int mode)    const                                             override;
    virtual std::string     GetZoneName(unsigned int zone)    const                                             override;
    virtual std::string     GetLEDName(unsigned int led)      const                                             override;

    virtual LenovoLegionDaemon::RGBColor        GetLED(unsigned int led)    const                               override;
    virtual void            SetLED(unsigned int led, LenovoLegionDaemon::RGBColor color)                        override;
    virtual void            SetAllLEDs(LenovoLegionDaemon::RGBColor color)                                      override;

    virtual int             GetMode()              const                                                        override;
    virtual void            SetMode(int mode)                                                                   override;

    virtual void            SetProfile(size_t profileIdx)                                                       override;

    virtual std::vector<LenovoLegionDaemon::led>       GetLEDs()  const                                         override;
    virtual void                          SetLEDs(const std::vector<LenovoLegionDaemon::led>& new_leds)         override;

    virtual std::vector<LenovoLegionDaemon::zone>      GetZones() const                                         override;

    virtual std::vector<LenovoLegionDaemon::mode>      GetModes()           const                               override;
    virtual void                          SetModes(const std::vector<LenovoLegionDaemon::mode>& new_modes)      override;

    virtual std::vector<LenovoLegionDaemon::RGBColor>   GetColors()          const                              override;
    virtual LenovoLegionDaemon::device_type                   GetDeviceType()      const                        override;
    virtual unsigned int                  GetProfiles()        const                                            override;
    virtual size_t                        GetActiveProfile()   const                                            override;

    virtual void ApplyPendingChanges()                                                                          override;

private:
    void readRGBControllerData();
    void sendRGBControllerData();

private:
    DataProvider* m_dataProvider;

    legion::messages::RGBController         m_rgbControllerData;
    std::bitset<MAXIMUM_CHANGES>            m_pendingChanges;
};

}
