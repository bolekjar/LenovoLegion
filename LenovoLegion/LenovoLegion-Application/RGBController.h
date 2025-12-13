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

#include <bitset>

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

    virtual const std::vector<LenovoLegionDaemon::led>&       GetLEDs()  const                                  override;
    virtual void                          SetLEDs(const std::vector<LenovoLegionDaemon::led>& new_leds)         override;

    virtual const std::vector<LenovoLegionDaemon::zone>&      GetZones() const                                  override;

    virtual const std::vector<LenovoLegionDaemon::mode>&      GetModes()           const                        override;
    virtual void                          SetModes(const std::vector<LenovoLegionDaemon::mode>& new_modes)      override;

    virtual const std::vector<LenovoLegionDaemon::RGBColor>&   GetColors()          const                       override;
    virtual LenovoLegionDaemon::device_type                   GetDeviceType()      const                        override;
    virtual unsigned int                  GetProfiles()        const                                            override;
    virtual size_t                        GetActiveProfile()   const                                            override;

    virtual void ApplyPendingChanges()                                                                          override;

private:
    void readRGBControllerData();
    void sendRGBControllerData();

private:
    DataProvider* m_dataProvider;

    std::vector<LenovoLegionDaemon::led>        m_leds;
    std::vector<LenovoLegionDaemon::zone>       m_zones;
    std::vector<LenovoLegionDaemon::mode>       m_modes;
    std::vector<LenovoLegionDaemon::RGBColor>   m_colors;
    std::vector<unsigned int>                   m_matrixMap;

    LenovoLegionDaemon::device_type   m_deviceType    = LenovoLegionDaemon::DEVICE_TYPE_UNKNOWN;
    int                               m_activeMode    = -1;
    size_t                            m_activeProfile = -1;
    int                               m_profiles      = -1;



    std::bitset<MAXIMUM_CHANGES>            m_pendingChanges;
};

}
