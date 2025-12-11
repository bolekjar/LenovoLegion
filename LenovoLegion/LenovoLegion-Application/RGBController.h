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
    virtual unsigned int    GetLEDsInZone(unsigned int zone)                                                    override;

    virtual std::string     GetModeName(unsigned int mode)                                                      override;
    virtual std::string     GetZoneName(unsigned int zone)                                                      override;
    virtual std::string     GetLEDName(unsigned int led)                                                        override;

    virtual LenovoLegionDaemon::RGBColor        GetLED(unsigned int led)                                        override;
    virtual void            SetLED(unsigned int led, LenovoLegionDaemon::RGBColor color)                        override;
    virtual void            SetAllLEDs(LenovoLegionDaemon::RGBColor color)                                      override;

    virtual int             GetMode()                                                                           override;
    virtual void            SetMode(int mode)                                                                   override;

    virtual void            RegisterUpdateCallback(LenovoLegionDaemon::RGBControllerCallback new_callback, 
                                                   void * new_callback_arg)                                     override;

    virtual void            UpdateLEDs()                                                                        override;
    virtual void            UpdateMode()                                                                        override;
    virtual void            SaveMode()                                                                          override;

    virtual void            ClearSegments(int zone)                                                             override;
    virtual void            AddSegment(int zone, LenovoLegionDaemon::segment new_segment)                       override;

    virtual void            SetDeviceProfile(size_t profileIdx)                                                 override;

    virtual void            SetupZones()                                                                        override;
    virtual void            ResizeZone(int zone, int new_size)                                                  override;
    virtual void            SetCustomMode()                                                                     override;
    virtual bool            IsApplayingSettingsInProgress() const                                               override;

    virtual const std::vector<LenovoLegionDaemon::led>&       GetLEDs()  const                                  override;
    virtual void                          SetLEDs(const std::vector<LenovoLegionDaemon::led>& new_leds)         override;

    virtual const std::vector<LenovoLegionDaemon::zone>&      GetZones() const                                  override;

    virtual const std::vector<LenovoLegionDaemon::mode>&      GetModes()           const                        override;
    virtual void                          SetModes(const std::vector<LenovoLegionDaemon::mode>& new_modes)      override;

    virtual const std::vector<LenovoLegionDaemon::RGBColor>&  GetColors()          const                        override;
    virtual LenovoLegionDaemon::device_type                   GetDeviceType()      const                        override;
    virtual unsigned int                  GetProfiles()        const                                            override;
    virtual size_t                        GetActiveProfile()   const                                            override;

private:
    void readRGBControllerData();
    void sendRGBControllerData();
    void convertProtobufToNative();
    void convertNativeToProtobuf();

private:
    DataProvider* m_dataProvider;

    legion::messages::RGBController m_rgbControllerData;

    // Native data structures converted from protobuf
    std::vector<LenovoLegionDaemon::led> m_leds;
    std::vector<LenovoLegionDaemon::zone> m_zones;
    std::vector<LenovoLegionDaemon::mode> m_modes;
    std::vector<LenovoLegionDaemon::RGBColor> m_colors;

    // Callback support
    LenovoLegionDaemon::RGBControllerCallback m_callback;
    void* m_callback_arg;

    // Pending changes flag
    bool m_hasPendingChanges;
};

}
