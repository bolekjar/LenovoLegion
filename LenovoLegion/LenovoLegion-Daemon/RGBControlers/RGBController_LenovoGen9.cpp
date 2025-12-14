// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "RGBController_LenovoGen9.h"
#include "RGBControllerKeyNames.h"
#include "LenovoDevices.h"

#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <thread>

namespace LenovoLegionDaemon {

RGBController_LenovoGen9::RGBController_LenovoGen9(LenovoGen9USBController* controller_ptr) :
    RGBController(),
    m_lenovoZones {

    {
        {
            "Keyboard",
            ZONE_TYPE_MATRIX,
            7,
            20,
            {   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
                20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  NA,  30,  31,  32,  33,  NA,  34,  35,  36,  37,
                38,  39,  40,  41,  NA,  42,  43,  44,  45,  46,  47,  48,  49,  50,  NA,  NA,  51,  52,  53,  NA,
                54,  55,  56,  57,  NA,  58,  59,  60,  61,  62,  63,  64,  65,  66,  NA,  67,  68,  69,  70,  71,
                72,  73,  74,  75,  76,  NA,  77,  78,  79,  80,  81,  82,  83,  NA,  84,  NA,  85,  86,  87,  NA,
                88,  89,  90,  91,  92,  NA,  NA,  NA,  NA,  NA,  93,  94,  NA,  NA,  95,  NA,  96,  NA,  97,  98,
                NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  99,  100, 101,  NA,  NA,  NA,  NA
            },
            {
                //row 1
                {KEY_EN_ESCAPE,0x01},//0
                {KEY_EN_F1,0x02},//1
                {KEY_EN_F2,0x03},//2
                {KEY_EN_F3,0x04},//3
                {KEY_EN_F4,0x05},//4
                {KEY_EN_F5,0x06},//5
                {KEY_EN_F6,0x07},//6
                {KEY_EN_F7,0x08},//7
                {KEY_EN_F8,0x09},//8
                {KEY_EN_F9,0x0A},//9
                {KEY_EN_F10,0x0B},//10
                {KEY_EN_F11,0x0C},//11
                {KEY_EN_F12,0x0D},//12
                {KEY_EN_INSERT,0x0E},//13
                {KEY_EN_PRINT_SCREEN,0x0F},//14
                {KEY_EN_DELETE,0x10},//15
                {KEY_EN_HOME,0x11},//16
                {KEY_EN_END,0x12},//17
                {KEY_EN_PAGE_UP,0x13},//18
                {KEY_EN_PAGE_DOWN,0x14},//19

                //row 2
                {KEY_EN_BACK_TICK,0x16},//20
                {KEY_EN_1,0x17},//21
                {KEY_EN_2,0x18},//22
                {KEY_EN_3,0x19},//23
                {KEY_EN_4,0x1A},//24
                {KEY_EN_5,0x1B},//25
                {KEY_EN_6,0x1C},//26
                {KEY_EN_7,0x1D},//27
                {KEY_EN_8,0x1E},//28
                {KEY_EN_9,0x1F},//29
                {KEY_EN_0,0x20},//30
                {KEY_EN_MINUS,0x21},//31
                {KEY_EN_EQUALS,0x22},//32
                {KEY_EN_BACKSPACE,0x38},//33
                {KEY_EN_NUMPAD_LOCK,0x26},//34
                {KEY_EN_NUMPAD_DIVIDE,0x27},//35
                {KEY_EN_NUMPAD_TIMES,0x28},//36
                {KEY_EN_NUMPAD_MINUS,0x29},//37

                //row 3
                {KEY_EN_TAB,0x40},//38
                {KEY_EN_Q,0x42},//39
                {KEY_EN_W,0x43},//40
                {KEY_EN_E,0x44},//41
                {KEY_EN_R,0x45},//42
                {KEY_EN_T,0x46},//43
                {KEY_EN_Y,0x47},//44
                {KEY_EN_U,0x48},//45
                {KEY_EN_I,0x49},//46
                {KEY_EN_O,0x4A},//47
                {KEY_EN_P,0x4B},//48
                {KEY_EN_LEFT_BRACKET,0x4C},//49
                {KEY_EN_RIGHT_BRACKET,0x4D},//50
                {KEY_EN_NUMPAD_7,0x4F},//51
                {KEY_EN_NUMPAD_8,0x50},//52
                {KEY_EN_NUMPAD_9,0x51},//53

                //row 4
                {KEY_EN_CAPS_LOCK,0x55},//54
                {KEY_EN_A,0x6D},//55
                {KEY_EN_S,0x6E},//56
                {KEY_EN_D,0x58},//57
                {KEY_EN_F,0x59},//58
                {KEY_EN_G,0x5A},//69
                {KEY_EN_H,0x71},//60
                {KEY_EN_J,0x72},//61
                {KEY_EN_K,0x5B},//62
                {KEY_EN_L,0x5C},//63
                {KEY_EN_SEMICOLON,0x5D},//64
                {KEY_EN_QUOTE,0x5F},//65
                {KEY_EN_ISO_BACK_SLASH,0xA8},//66
                {KEY_EN_ISO_ENTER,0x77},//67
                {KEY_EN_NUMPAD_4,0x79},//68
                {KEY_EN_NUMPAD_5,0x7B},//69
                {KEY_EN_NUMPAD_6,0x7C},//70
                {KEY_EN_NUMPAD_PLUS,0x68},//71

                //row 5
                {KEY_EN_LEFT_SHIFT,0x6A},//72
                {KEY_EN_ISO_BACK_SLASH,0x4E},//73
                {KEY_EN_Z,0x82},//74
                {KEY_EN_X,0x83},//75
                {KEY_EN_C,0x6F},//76
                {KEY_EN_V,0x70},//77
                {KEY_EN_B,0x87},//78
                {KEY_EN_N,0x88},//79
                {KEY_EN_M,0x73},//80
                {KEY_EN_COMMA,0x74},//81
                {KEY_EN_PERIOD,0x75},//821
                {KEY_EN_FORWARD_SLASH,0x76},//83
                {KEY_EN_RIGHT_SHIFT,0x8D},//84
                {KEY_EN_NUMPAD_1,0x8E},//85
                {KEY_EN_NUMPAD_2,0x90},//86
                {KEY_EN_NUMPAD_3,0x92},//87

                //row 6
                {KEY_EN_LEFT_CONTROL,0x7F},//88
                {KEY_EN_LEFT_FUNCTION,0x80},//89
                {KEY_EN_LEFT_WINDOWS,0x96},//90
                {KEY_EN_LEFT_ALT,0x97},//91
                {KEY_EN_SPACE,0x98},//92
                {KEY_EN_RIGHT_ALT,0x9A},//93
                {KEY_EN_RIGHT_CONTROL,0x9B},//94
                {KEY_EN_UP_ARROW,0x9D},//95
                {KEY_EN_NUMPAD_0,0xA3},//96
                {KEY_EN_NUMPAD_PERIOD,0xA5},//97
                {KEY_EN_NUMPAD_ENTER,0xA7},//98

                //row 7
                {KEY_EN_LEFT_ARROW,0x9C},//99
                {KEY_EN_DOWN_ARROW,0x9F},//100
                {KEY_EN_RIGHT_ARROW,0xA1},//101
            }
        },
        {
            "Neon",
                ZONE_TYPE_LINEAR,
                1,
                6,
                {},
            {
                {"Neon group 1", 0xF5 | 0x0100},//0
                    {"Neon group 2", 0xF6 | 0x0100},//1
                    {"Neon group 3", 0xF7 | 0x0100},//2
                    {"Neon group 4", 0xF8 | 0x0100},//3
                    {"Neon group 5", 0xF9 | 0x0100},//4
                    {"Neon group 6", 0xFA | 0x0100} //5
            }
        }
    }

    },
    controller(controller_ptr)
{
    /*
     * Set the controller
     */
    vendor      = "Lenovo";
    description = "Lenovo Legion 7 Pro Gen 9 laptop driver";
    type        = DEVICE_TYPE_KEYBOARD;

    name        = controller->getName();
    location    = controller->getLocation();
    serial      = controller->getSerialString();
    version     = controller->getFirmwareVersion();

    /*
     * Set supported profiles
     */
    profiles       =  6;

    /*
     * Set supported modes
     */
    mode Screw;
    Screw.name              = "Screw Rainbow";
    Screw.value             = MODE_SCREW_RAINBOW;
    Screw.flags             = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Screw.speed_min         = SPEED_MIN;
    Screw.speed_max         = SPEED_MAX;
    Screw.speed             = SPEED_MAX / 2;
    Screw.color_mode        = MODE_COLORS_NONE;
    Screw.brightness_min    = BRIGHTNESS_MIN;
    Screw.brightness_max    = BRIGHTNESS_MAX;
    Screw.brightness        = BRIGHTNESS_MAX / 2;
    Screw.direction         = MODE_DIRECTION_RIGHT;
    modes.push_back(Screw);

    mode Rainbow;
    Rainbow.name            = "Rainbow Wave";
    Rainbow.value           = MODE_RAINBOW_WAVE;
    Rainbow.flags           = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR | MODE_FLAG_HAS_DIRECTION_UD | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Rainbow.speed_min       = SPEED_MIN;
    Rainbow.speed_max       = SPEED_MAX;
    Rainbow.speed           = SPEED_MAX / 2;
    Rainbow.color_mode      = MODE_COLORS_NONE;
    Rainbow.brightness_min  = BRIGHTNESS_MIN;
    Rainbow.brightness_max  = BRIGHTNESS_MAX;
    Rainbow.brightness      = BRIGHTNESS_MAX / 2;
    Rainbow.direction       = MODE_DIRECTION_RIGHT;
    modes.push_back(Rainbow);

    mode ColorChange;
    ColorChange.name        = "Color Change";
    ColorChange.value       = MODE_COLOR_CHANGE;
    ColorChange.flags       = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    ColorChange.speed_min   = SPEED_MIN;
    ColorChange.speed_max   = SPEED_MAX;
    ColorChange.speed       = SPEED_MAX / 2;
    ColorChange.colors_min  = 1;
    ColorChange.colors_max  = 4;
    ColorChange.colors.resize(4);
    ColorChange.colors[0]   = 0xFFF500;
    ColorChange.colors[1]   = 0xFFF500;
    ColorChange.colors[2]   = 0xFFF500;
    ColorChange.colors[3]   = 0xFFF500;
    ColorChange.color_mode  = MODE_COLORS_RANDOM;
    ColorChange.brightness_min = BRIGHTNESS_MIN;
    ColorChange.brightness_max = BRIGHTNESS_MAX;
    ColorChange.brightness     = BRIGHTNESS_MAX / 2;
    modes.push_back(ColorChange);

    mode ColorPulse;
    ColorPulse.name         = "Color Pulse";
    ColorPulse.value        = MODE_COLOR_PULSE;
    ColorPulse.flags        = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    ColorPulse.speed_min    = SPEED_MIN;
    ColorPulse.speed_max    = SPEED_MAX;
    ColorPulse.speed        = SPEED_MAX / 2;
    ColorPulse.colors_min   = 1;
    ColorPulse.colors_max   = 4;
    ColorPulse.colors.resize(4);
    ColorPulse.colors[0]    = 0xFFF500;
    ColorPulse.colors[1]    = 0xFFF500;
    ColorPulse.colors[2]    = 0xFFF500;
    ColorPulse.colors[3]    = 0xFFF500;
    ColorPulse.color_mode   = MODE_COLORS_RANDOM;
    ColorPulse.brightness_min = BRIGHTNESS_MIN;
    ColorPulse.brightness_max = BRIGHTNESS_MAX;
    ColorPulse.brightness     = BRIGHTNESS_MAX / 2;
    modes.push_back(ColorPulse);

    mode ColorWave;
    ColorWave.name          = "Color Wave";
    ColorWave.value         = MODE_COLOR_WAVE;
    ColorWave.flags         = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR | MODE_FLAG_HAS_DIRECTION_UD | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    ColorWave.speed_min     = SPEED_MIN;
    ColorWave.speed_max     = SPEED_MAX;
    ColorWave.speed         = SPEED_MAX / 2;
    ColorWave.colors_min    = 1;
    ColorWave.colors_max    = 4;
    ColorWave.colors.resize(4);
    ColorWave.colors[0]     = 0xFFF500;
    ColorWave.colors[1]     = 0xFFF500;
    ColorWave.colors[2]     = 0xFFF500;
    ColorWave.colors[3]     = 0xFFF500;
    ColorWave.color_mode    = MODE_COLORS_RANDOM;
    ColorWave.brightness_min = BRIGHTNESS_MIN;
    ColorWave.brightness_max = BRIGHTNESS_MAX;
    ColorWave.brightness     = BRIGHTNESS_MAX / 2;
    ColorWave.direction      = MODE_DIRECTION_RIGHT;
    modes.push_back(ColorWave);

    mode Smooth;
    Smooth.name         = "Smooth";
    Smooth.value        = MODE_SMOOTH;
    Smooth.flags        = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Smooth.speed_min    = SPEED_MIN;
    Smooth.speed_max    = SPEED_MAX;
    Smooth.speed        = SPEED_MAX / 2;
    Smooth.colors_min   = 1;
    Smooth.colors_max   = 4;
    Smooth.colors.resize(4);
    Smooth.colors[0]    = 0xFFF500;
    Smooth.colors[1]    = 0xFFF500;
    Smooth.colors[2]    = 0xFFF500;
    Smooth.colors[3]    = 0xFFF500;
    Smooth.color_mode   = MODE_COLORS_RANDOM;
    Smooth.brightness_min = BRIGHTNESS_MIN;
    Smooth.brightness_max = BRIGHTNESS_MAX;
    Smooth.brightness     = BRIGHTNESS_MAX / 2;
    modes.push_back(Smooth);

    mode Rain;
    Rain.name           = "Rain";
    Rain.value          = MODE_RAIN;
    Rain.flags          = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Rain.speed_min      = SPEED_MIN;
    Rain.speed_max      = SPEED_MAX;
    Rain.speed          = SPEED_MAX / 2;
    Rain.colors_min     = 1;
    Rain.colors_max     = 4;
    Rain.colors.resize(4);
    Rain.colors[0]      = 0xFFF500;
    Rain.colors[1]      = 0xFFF500;
    Rain.colors[2]      = 0xFFF500;
    Rain.colors[3]      = 0xFFF500;
    Rain.color_mode     = MODE_COLORS_RANDOM;
    Rain.brightness_min = BRIGHTNESS_MIN;
    Rain.brightness_max = BRIGHTNESS_MAX;
    Rain.brightness     = BRIGHTNESS_MAX / 2;
    modes.push_back(Rain);

    mode Ripple;
    Ripple.name         = "Ripple";
    Ripple.value        = MODE_RIPPLE;
    Ripple.flags        = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Ripple.speed_min    = SPEED_MIN;
    Ripple.speed_max    = SPEED_MAX;
    Ripple.speed        = SPEED_MAX / 2;
    Ripple.colors_min   = 1;
    Ripple.colors_max   = 4;
    Ripple.colors.resize(4);
    Ripple.colors[0]    = 0xFFF500;
    Ripple.colors[1]    = 0xFFF500;
    Ripple.colors[2]    = 0xFFF500;
    Ripple.colors[3]    = 0xFFF500;
    Ripple.color_mode   = MODE_COLORS_RANDOM;
    Ripple.brightness_min = BRIGHTNESS_MIN;
    Ripple.brightness_max = BRIGHTNESS_MAX;
    Ripple.brightness     = BRIGHTNESS_MAX / 2;
    modes.push_back(Ripple);

    mode AudioBounce;
    AudioBounce.name        = "Audio Bounce Lighting";
    AudioBounce.value       = MODE_AUDIO_BOUNCE;
    AudioBounce.flags       = MODE_FLAG_HAS_BRIGHTNESS;
    AudioBounce.color_mode  = MODE_COLORS_NONE;
    AudioBounce.brightness_min = BRIGHTNESS_MIN;
    AudioBounce.brightness_max = BRIGHTNESS_MAX;
    AudioBounce.brightness     = BRIGHTNESS_MAX / 2;
    modes.push_back(AudioBounce);

    mode AudioRipple;
    AudioRipple.name        = "Audio Ripple Lighting";
    AudioRipple.value       = MODE_AUDIO_RIPPLE;
    AudioRipple.flags       = MODE_FLAG_HAS_BRIGHTNESS;
    AudioRipple.color_mode  = MODE_COLORS_NONE;
    AudioRipple.brightness_min = BRIGHTNESS_MIN;
    AudioRipple.brightness_max = BRIGHTNESS_MAX;
    AudioRipple.brightness     = BRIGHTNESS_MAX / 2;
    modes.push_back(AudioRipple);

    mode Static;
    Static.name             = "Static";
    Static.value            = MODE_STATIC;
    Static.flags            = MODE_FLAG_HAS_PER_LED_COLOR |  MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Static.color_mode       = MODE_COLORS_PER_LED;
    Static.brightness_min   = BRIGHTNESS_MIN;
    Static.brightness_max   = BRIGHTNESS_MAX;
    Static.brightness       = BRIGHTNESS_MAX / 2;
    modes.push_back(Static);

    mode Type;
    Type.name               = "Type Lighting";
    Type.value              = MODE_TYPE;
    Type.flags              = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Type.speed_min          = SPEED_MIN;
    Type.speed_max          = SPEED_MAX;
    Type.speed              = SPEED_MAX / 2;
    Type.colors_min         = 1;
    Type.colors_max         = 4;
    Type.colors.resize(4);
    Type.colors[0]          = 0xFFF500;
    Type.colors[1]          = 0xFFF500;
    Type.colors[2]          = 0xFFF500;
    Type.colors[3]          = 0xFFF500;
    Type.color_mode         = MODE_COLORS_RANDOM;
    Type.brightness_min     = BRIGHTNESS_MIN;
    Type.brightness_max     = BRIGHTNESS_MAX;
    Type.brightness         = BRIGHTNESS_MAX / 2;
    modes.push_back(Type);

    mode Direct;
    Direct.name             = "Direct";
    Direct.value            = MODE_DIRECT;
    Direct.flags            = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAGS_DIRECT;
    Direct.color_mode       = MODE_COLORS_PER_LED;
    Direct.brightness_min   = BRIGHTNESS_MIN;
    Direct.brightness_max   = BRIGHTNESS_MAX;
    Direct.brightness       = BRIGHTNESS_MAX / 2;
    modes.push_back(Direct);


    /*
     * Setups the zones and colors
     */
    SetupZones();
    SetupColors();


    /*
     * Read active profile
     */
    active_profile =  fromControlerProfile(controller->getCurrentProfileId());

    /*
     * Read active profile settings
     */
    readActiveProfileSettings();
}

RGBController_LenovoGen9::~RGBController_LenovoGen9()
{}


void RGBController_LenovoGen9::SetupZones()
{
    /*-------------------------------------------------*\
    | Clear any existing Zones/LED configuration        |
    \*-------------------------------------------------*/
    leds.clear();
    zones.clear();

    /*
     * Set zones
     */
    for(unsigned int i = 0; i < m_lenovoZones.size(); i++)
    {
        zones.emplace_back();

        zones.back().name       = m_lenovoZones[i].name;
        zones.back().type       = m_lenovoZones[i].type;
        zones.back().leds_count = m_lenovoZones[i].leds.size();
        zones.back().leds_max   = m_lenovoZones[i].leds.size();
        zones.back().leds_min   = m_lenovoZones[i].leds.size();

        if(m_lenovoZones[i].matrix_map.size() > 0)
        {
            zones.back().matrix_map = std::make_unique<matrix_map_type>(m_lenovoZones[i].height,m_lenovoZones[i].width,m_lenovoZones[i].matrix_map.data());
        }

        std::copy(m_lenovoZones[i].leds.begin(), m_lenovoZones[i].leds.end(), std::back_inserter(leds));
    }

}

void RGBController_LenovoGen9::DeviceUpdateProfile()
{
    auto waitForApplyProfileOnController = [this](int maxWaitTimeinMicros = 1000000) {
        while(fromControlerProfile(controller->getCurrentProfileId()) != active_profile)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            maxWaitTimeinMicros -= 100;

            if(maxWaitTimeinMicros <= 0)
            {
                break;
            }
        }
    };

    controller->switchProfileTo(toControlerProfile(active_profile));

    waitForApplyProfileOnController();

    readActiveProfileSettings();
}

void RGBController_LenovoGen9::Refresh()
{
    /*
     * Read active profile
     */
    active_profile =  fromControlerProfile(controller->getCurrentProfileId());

    /*
     * Read active profile settings
     */
    readActiveProfileSettings();
}

void RGBController_LenovoGen9::DeviceUpdateMode()
{
    controller->setBrightness(modes[active_mode].brightness);

    DeviceUpdateLEDs();
 }

void RGBController_LenovoGen9::DeviceUpdateLEDs()
{
    if(modes[active_mode].value == MODE_DIRECT)
    {
        controller->setLedsDirectOn(toControlerProfile(active_profile));
        controller->setLedsDirect(leds, colors);
    }
    else
    {
        controller->setLedsDirectOff(toControlerProfile(active_profile));
        controller->setLedsByGroup(toControlerProfile(active_profile), toControlerLedGroups());
    }
}

void RGBController_LenovoGen9::readActiveProfileSettings()
{
    /*---------------------------------------------------------*\
    | Retrieve current values by readingledIdToIndex the device             |
    \*---------------------------------------------------------*/
    std::vector<LenovoGen9USBController::led_group> l_currentSettings = controller->getProfileSettings(toControlerProfile(active_profile));

    for (auto & l_mode: modes) {

        /*
         * Common on all modes. Read brgthness
         */
        l_mode.brightness = controller->getCurrentBrightness();

        for (const auto & grub: l_currentSettings) {

            /*
             * Read speed
             */
            l_mode.speed = grub.speed;


            if(l_mode.value == grub.mode)
            {

                /*
                 * Read color mode
                 */
                l_mode.color_mode = fromControlerColorMode(l_mode.flags, grub.color_mode);


                l_mode.colors.resize(grub.colors.size());

                /*
                 * Set colors
                 */

                fromControlerColours(colors,l_mode.colors,l_mode.color_mode,grub.colors, grub.leds);

                try {

                    /*
                    * Read direction
                    */
                    l_mode.direction = fromControlerDirection(grub.direction);

                } catch (const std::out_of_range&) {

                    try {

                        /*
                         * Read spin
                         */
                        l_mode.direction = fromControlerSpin(grub.spin);

                    }
                    catch (const std::out_of_range&) {}
                }

                /*
                 * Set active mode
                 */
                active_mode = std::distance(modes.begin(),std::find_if(modes.begin(),modes.end(),[&grub](const mode& m){return m.value == grub.mode;}));
            }
        }
    }
}

unsigned int RGBController_LenovoGen9::fromControlerColorMode(const unsigned int  modeFlags,const uint8_t color_mode) const
{
    switch(color_mode)
    {
    case 0x02:
    {
        if(modeFlags & MODE_FLAG_HAS_PER_LED_COLOR)
        {
            return  MODE_COLORS_PER_LED;
        }
        else if(modeFlags & MODE_FLAG_HAS_MODE_SPECIFIC_COLOR)
        {
            return MODE_COLORS_MODE_SPECIFIC;
        }
    }
    break;
    case 0x01:
    {
        if(modeFlags & MODE_FLAG_HAS_RANDOM_COLOR)
        {
            return MODE_COLORS_RANDOM;
        }
    }
    break;
    default:
        return MODE_COLORS_NONE;
    }

    return MODE_COLORS_NONE;
}


void RGBController_LenovoGen9::fromControlerColours  (   std::vector<RGBColor>&       colors,
                                                         std::vector<RGBColor>&       modeSpecificColors,
                                                         const unsigned int           colorMode,
                                                         const std::vector<RGBColor>& controlerColors,
                                                         const std::vector<uint16_t>& controlerLeds)             const
{
    switch(colorMode)
    {
    case MODE_COLORS_PER_LED:
    {
        std::unordered_map<uint16_t,size_t>     l_ledIdToIndex;

        /*
         * Build the led id to index map
         */
        for (size_t idx = 0; idx < this->leds.size(); ++idx) {
            l_ledIdToIndex[this->leds.at(idx).value] = idx;
        }

        for (uint16_t ledId : controlerLeds)
        {
            try {
                colors.at(l_ledIdToIndex.at(ledId)) = controlerColors.at(0);
            } catch (const std::out_of_range&) {}
        }

    }
    break;

    case MODE_COLORS_MODE_SPECIFIC:
    {
        std::copy_n(controlerColors.begin(),std::min(modeSpecificColors.size(),controlerColors.size()),modeSpecificColors.begin());
    }
    break;
    }
}

unsigned int RGBController_LenovoGen9::fromControlerDirection(const uint8_t direction) const
{
    switch(direction)
    {
    case 0x01:
        return MODE_DIRECTION_UP;
        break;
    case 0x02:
        return MODE_DIRECTION_DOWN;
        break;
    case 0x03:
        return MODE_DIRECTION_LEFT;
        break;
    case 0x04:
        return MODE_DIRECTION_RIGHT;
        break;
    }

    throw std::out_of_range("Invalid direction");
}

unsigned int RGBController_LenovoGen9::fromControlerSpin(const uint8_t spin) const
{
    switch(spin)
    {
    case 0x01:
        return MODE_DIRECTION_RIGHT;
        break;
    case 0x02:
        return MODE_DIRECTION_LEFT;
        break;
    }

    throw std::out_of_range("Invalid spin");
}

size_t RGBController_LenovoGen9::fromControlerProfile(uint8_t profile) const
{
    return static_cast<uint8_t>(profile - 1);
}

std::vector<LenovoGen9USBController::led_group> RGBController_LenovoGen9::toControlerLedGroups() const
{
    std::vector<LenovoGen9USBController::led_group> groups;

    auto leds = toControlerColorsLedsMap(modes[active_mode].color_mode);


    for (const auto& cur :  leds)
    {
        groups.push_back(LenovoGen9USBController::led_group{
            .mode       = static_cast<uint8_t>(modes[active_mode].value),
            .speed      = static_cast<uint8_t>(modes[active_mode].speed),
            .spin       = toControlerSpin(modes[active_mode].direction),
            .direction  = toControlerDirection(modes[active_mode].direction),
            .color_mode = toControlerColorMode(modes[active_mode].color_mode),
            .colors     = modes[active_mode].color_mode == MODE_COLORS_PER_LED ? std::vector<RGBColor> { cur.first } : std::vector<RGBColor>(modes[active_mode].colors),
            .leds       = cur.second
        });
    }


    return groups;
}

uint8_t RGBController_LenovoGen9::toControlerDirection(unsigned int  direction) const
{
    if(modes[active_mode].value == MODE_SCREW_RAINBOW)
    {
        return 0x0;
    }


    switch(direction)
    {
    case MODE_DIRECTION_UP:
        return 0x01;
        break;

    case MODE_DIRECTION_DOWN:
        return 0x02;
        break;

    case MODE_DIRECTION_LEFT:
            return 0x03;
        break;

    case MODE_DIRECTION_RIGHT:
            return 0x04;
        break;
    default:
        return 0x00;
        break;
    }
}

uint8_t RGBController_LenovoGen9::toControlerSpin(unsigned int spin) const
{
    if(modes[active_mode].value != MODE_SCREW_RAINBOW)
    {
        return 0x00;
    }

    switch(spin)
    {
    case MODE_DIRECTION_UP:
        return 0x00;
        break;

    case MODE_DIRECTION_DOWN:
        return 0x00;
        break;

    case MODE_DIRECTION_LEFT:
        return 0x02;
        break;

    case MODE_DIRECTION_RIGHT:
        return 0x01;
        break;
    default:
        return 0x00;
        break;
    }
}

uint8_t RGBController_LenovoGen9::toControlerColorMode(unsigned int color_mode) const
{
    switch(color_mode)
    {
    default:
    case MODE_COLORS_NONE:
        return 0x00;
        break;

    case MODE_COLORS_RANDOM:
        return 0x01;
        break;

    case MODE_COLORS_MODE_SPECIFIC:
    case MODE_COLORS_PER_LED:
        return 0x02;
        break;
    }
}

uint8_t RGBController_LenovoGen9::toControlerProfile(size_t profile) const
{
    return static_cast<uint8_t>(profile + 1);
}

std::map<RGBColor,std::vector<uint16_t>> RGBController_LenovoGen9::toControlerColorsLedsMap(unsigned int color_mode) const
{
    std::map<RGBColor,std::vector<uint16_t>> colorToLedsMap;;

    if(color_mode == MODE_COLORS_PER_LED)
    {
        for(size_t i = 0; i < leds.size(); i++)
        {
            colorToLedsMap[colors[i]].push_back(leds[i].value);
        }
    }
    else
    {
        /*---------------------------------------------------------*\
        | Riplle and Type only apply to keyboard                    |
        \*---------------------------------------------------------*/
        if(modes[active_mode].value == MODE_RIPPLE || modes[active_mode].value == MODE_TYPE)
        {
            for(const zone &z : zones)
            {
                if(z.name == "Keyboard")
                {
                    std::for_each(leds.begin() + z.start_idx,leds.end(),[&colorToLedsMap](const led &l){
                        colorToLedsMap[0x00].push_back(l.value);
                    });
                }
            }
        }
        else
        {
            for (const auto &led : leds)
            {
                colorToLedsMap[0x00].push_back(led.value);
            }
        }
    }

    return colorToLedsMap;
}

}
