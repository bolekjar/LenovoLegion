// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */


#pragma once

#include "LenovoGen9USBController.h"

#include "RGBController.h"

#include <QtTypes>

#include <vector>
#include <map>


namespace LenovoLegionDaemon {

class RGBController_LenovoGen9 : public RGBController
{

public:

    enum SupportedModes : quint8
    {
        MODE_SCREW_RAINBOW                  = 0x01,
        MODE_RAINBOW_WAVE                   = 0x02,
        MODE_COLOR_CHANGE                   = 0x03,
        MODE_COLOR_PULSE                    = 0x04,
        MODE_COLOR_WAVE                     = 0x05,
        MODE_SMOOTH                         = 0x06,
        MODE_RAIN                           = 0x07,
        MODE_RIPPLE                         = 0x08,
        MODE_AUDIO_BOUNCE                   = 0x09,
        MODE_AUDIO_RIPPLE                   = 0x0A,
        MODE_STATIC                         = 0x0B,
        MODE_TYPE                           = 0x0C,
        MODE_DIRECT                         = 0x0D
    };

    enum
    {
        BRIGHTNESS_MIN                = 0x00,
        BRIGHTNESS_MAX                = 0x09,
        SPEED_MIN                     = 0x01,
        SPEED_MAX                     = 0x03
    };

public:

    struct Zone
    {
        const std::string     name;
        const zone_type       type;

        const unsigned int    height;
        const unsigned int    width;

        std::vector<unsigned int> matrix_map;
        std::vector<led>          leds;
    };

private:

    std::vector<Zone>                            m_lenovoZones;

public:

    RGBController_LenovoGen9(LenovoGen9USBController* controller_ptr);
    ~RGBController_LenovoGen9();

    void        SetupZones()                       override;
    void        ResizeZone(int zone, int new_size) override;

    void        DeviceUpdateLEDs()                 override;
    void        DeviceUpdateMode()                 override;


    void        UpdateZoneLEDs(int zone)           override;
    void        UpdateSingleLED(int led)           override;

    void        SetDeviceProfile(size_t profileIdx)override;

private:


    void readActiveProfileSettings();


    /*
     * Serialization from controler model
     */
    unsigned int             fromControlerColorMode(const unsigned int  modeFlags,
                                                    const uint8_t color_mode)                               const;
    void                     fromControlerColours  (std::vector<RGBColor>&       colors,
                                                    std::vector<RGBColor>&       modeSpecificColors,
                                                    const unsigned int           colorMode,
                                                    const std::vector<RGBColor>& controlerColors,
                                                    const std::vector<uint16_t>& controlerLeds)             const;
    unsigned int             fromControlerDirection(const uint8_t direction)                                const;
    unsigned int             fromControlerSpin(const uint8_t spin)                                          const;
    size_t                   fromControlerProfile(uint8_t profile)                                          const;


    /*
     * Serialization to controler model
     */
    std::vector<LenovoGen9USBController::led_group>     toControlerLedGroups()                              const;
    uint8_t                                             toControlerDirection(unsigned int direction)        const;
    uint8_t                                             toControlerSpin(unsigned int spin)                  const;
    uint8_t                                             toControlerColorMode(unsigned int color_mode)       const;
    std::map<RGBColor,std::vector<uint16_t>>            toControlerColorsLedsMap(unsigned int color_mode)   const;
    uint8_t                                             toControlerProfile(size_t profile)                  const;

private:

    std::unique_ptr<LenovoGen9USBController>  controller;
};

}
