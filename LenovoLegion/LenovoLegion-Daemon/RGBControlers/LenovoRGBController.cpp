// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "LenovoRGBController.h"
#include "RGBControllerKeyNames.h"

#include <Core/LoggerHolder.h>

#include <QRandomGenerator>

#include <thread>

namespace LenovoLegionDaemon {


LenovoRGBController::LenovoRGBController(LenovoUSBController* controller_ptr) :
    RGBController(  {.min = 1,.max = 6,.active = 1},                            /* profiles */
                    {.min = 0,.max = 9,.active = 0},                            /* brightness levels */
                    "Lenovo RGB Controller",            /* name */
                    "Lenovo",                           /* vendor */
                    "Lenovo RGB Controller",            /* description */
                    controller_ptr->getSerialString(),  /* serial */
                    controller_ptr->getLocation(),      /* location */
                    DEVICE_TYPE_KEYBOARD,
                    {                                   /* modes */
                        {
                            "Screw Rainbow",
                            MODE_SCREW_RAINBOW,
                            MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_SPINLR | MODE_FLAG_HAS_AUTOMATIC_SAVE | MODE_FLAG_HAS_PER_LED_SELECTION,
                            SPEED_MIN,
                            SPEED_MAX,
                            0,
                            0,
                            SPEED_MAX / 2,
                            MODE_DIRECTION_RIGHT,
                            MODE_COLORS_NONE,
                            {},
                        },
                        {
                            "Rainbow Wave",
                            MODE_RAINBOW_WAVE,
                            MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR | MODE_FLAG_HAS_DIRECTION_UD |  MODE_FLAG_HAS_AUTOMATIC_SAVE | MODE_FLAG_HAS_PER_LED_SELECTION,
                            SPEED_MIN,
                            SPEED_MAX,
                            0,
                            0,
                            SPEED_MAX / 2,
                            MODE_DIRECTION_RIGHT,
                            MODE_COLORS_NONE,
                            {},
                        },
                        {
                            "Color Change",
                            MODE_COLOR_CHANGE,
                            MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR |  MODE_FLAG_HAS_AUTOMATIC_SAVE | MODE_FLAG_HAS_PER_LED_SELECTION,
                            SPEED_MIN,
                            SPEED_MAX,
                            1,
                            4,
                            SPEED_MAX / 2,
                            0,
                            MODE_COLORS_RANDOM,
                            {0xFFF500, 0xFFF500, 0xFFF500, 0xFFF500},
                        },
                        {
                            "Color Pulse",
                            MODE_COLOR_PULSE,
                            MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR |  MODE_FLAG_HAS_AUTOMATIC_SAVE | MODE_FLAG_HAS_PER_LED_SELECTION,
                            SPEED_MIN,
                            SPEED_MAX,
                            1,
                            4,
                            SPEED_MAX / 2,
                            0,
                            MODE_COLORS_RANDOM,
                            {0xFFF500, 0xFFF500, 0xFFF500, 0xFFF500},
                        },
                        {
                            "Color Wave",
                            MODE_COLOR_WAVE,
                            MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR | MODE_FLAG_HAS_DIRECTION_UD | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR |  MODE_FLAG_HAS_AUTOMATIC_SAVE | MODE_FLAG_HAS_PER_LED_SELECTION,
                            SPEED_MIN,
                            SPEED_MAX,
                            1,
                            4,
                            SPEED_MAX / 2,
                            MODE_DIRECTION_RIGHT,
                            MODE_COLORS_RANDOM,
                            {0xFFF500, 0xFFF500, 0xFFF500, 0xFFF500},
                        },
                        {
                            "Smooth",
                            MODE_SMOOTH,
                            MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR |  MODE_FLAG_HAS_AUTOMATIC_SAVE | MODE_FLAG_HAS_PER_LED_SELECTION,
                            SPEED_MIN,
                            SPEED_MAX,
                            1,
                            4,
                            SPEED_MAX / 2,
                            0,
                            MODE_COLORS_RANDOM,
                            {0xFFF500, 0xFFF500, 0xFFF500, 0xFFF500},
                        },
                        {
                            "Rain",
                            MODE_RAIN,
                            MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR |  MODE_FLAG_HAS_AUTOMATIC_SAVE | MODE_FLAG_HAS_PER_LED_SELECTION,
                            SPEED_MIN,
                            SPEED_MAX,
                            1,
                            4,
                            SPEED_MAX / 2,
                            0,
                            MODE_COLORS_RANDOM,
                            {0xFFF500, 0xFFF500, 0xFFF500, 0xFFF500},
                        },
                        {
                            "Ripple",
                            MODE_RIPPLE,
                            MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR |  MODE_FLAG_HAS_AUTOMATIC_SAVE | MODE_FLAG_HAS_ALL_KB_LED_SELECTION,
                            SPEED_MIN,
                            SPEED_MAX,
                            1,
                            4,
                            SPEED_MAX / 2,
                            0,
                            MODE_COLORS_RANDOM,
                            {0xFFF500, 0xFFF500, 0xFFF500, 0xFFF500},
                        },
                        {
                            "Audio Bounce Lighting",
                            MODE_AUDIO_BOUNCE_LIGHTING,
                            MODE_FLAG_HAS_ALL_LED_SELECTION,
                            0,
                            0,
                            0,
                            0,
                            0,
                            MODE_COLORS_NONE,
                            {},
                            {}
                        },
                        {
                            "Audio Ripple Lighting",
                            MODE_AUDIO_RIPPLE_LIGHTING,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            MODE_COLORS_NONE,
                            {},
                            {}
                        },
                        {
                            "Always",
                            MODE_ALWAYS,
                            MODE_FLAG_HAS_PER_LED_SELECTION | MODE_FLAG_HAS_PER_LED_COLOR |  MODE_FLAG_HAS_AUTOMATIC_SAVE,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            MODE_COLORS_PER_LED,
                            {},
                        },
                        {
                            "Type Lighting",
                            MODE_TYPE_LIGHTING,
                            MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_AUTOMATIC_SAVE | MODE_FLAG_HAS_ALL_KB_LED_SELECTION,
                            SPEED_MIN,
                            SPEED_MAX,
                            1,
                            4,
                            SPEED_MAX / 2,
                            0,
                            MODE_COLORS_RANDOM,
                            {0xFFF500, 0xFFF500, 0xFFF500, 0xFFF500},
                        },
                        {
                            "Aura",
                            MODE_LEGION_AURASYNC,
                            MODE_FLAG_HAS_DIRECT_CONTROL | MODE_FLAG_HAS_ALL_LED_SELECTION,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            MODE_COLORS_NONE,
                            {},

                        }
                    },
                    {       /* zones */
                        {
                            "Keyboard",
                            ZONE_TYPE_MATRIX,
                            0,
                                [controller_ptr](){
                                        unsigned int leds_count = 0;

                                        for (int y = 0; y < controller_ptr->getKeyMap().m_height; y++)
                                        {
                                            for (int x = 0; x < controller_ptr->getKeyMap().m_width; x++)
                                            {
                                                if(controller_ptr->getKeyMap().m_keyCodes[x][y] != 0x0000)
                                                {
                                                    leds_count++;
                                                }
                                            }
                                        }
                                        return leds_count;
                                    }()
                            ,
                                 [controller_ptr](){
                                     unsigned int leds_count = 0;

                                     for (int y = 0; y < controller_ptr->getKeyMap().m_height; y++)
                                     {
                                         for (int x = 0; x < controller_ptr->getKeyMap().m_width; x++)
                                         {
                                             if(controller_ptr->getKeyMap().m_keyCodes[x][y] != 0x0000)
                                             {
                                                 leds_count++;
                                             }
                                         }
                                     }
                                     return leds_count;
                                 }()
                            ,
                                 [controller_ptr](){
                                     unsigned int leds_count = 0;

                                     for (int y = 0; y < controller_ptr->getKeyMap().m_height; y++)
                                     {
                                         for (int x = 0; x < controller_ptr->getKeyMap().m_width; x++)
                                         {
                                             if(controller_ptr->getKeyMap().m_keyCodes[x][y] != 0x0000)
                                             {
                                                 leds_count++;
                                             }
                                         }
                                     }
                                     return leds_count;
                                 }()
                            ,
                            {
                                  [controller_ptr](){
                                      std::vector<led> leds;

                                      for (int y = 0; y < controller_ptr->getKeyMap().m_height; y++)
                                      {
                                          for (int x = 0; x < controller_ptr->getKeyMap().m_width; x++)
                                          {
                                              if(controller_ptr->getKeyMap().m_keyCodes[x][y] != 0x0000 )
                                              {
                                                  if(KeyCodesToName.count(controller_ptr->getKeyMap().m_keyCodes[x][y]) > 0)
                                                  {
                                                      leds.push_back(led {KeyCodesToName.at(controller_ptr->getKeyMap().m_keyCodes[x][y]), controller_ptr->getKeyMap().m_keyCodes[x][y]});
                                                  }
                                                  else
                                                  {
                                                      leds.push_back(led {QString::number(controller_ptr->getKeyMap().m_keyCodes[x][y]).toStdString() ,
                                                                         controller_ptr->getKeyMap().m_keyCodes[x][y]});
                                                  }
                                              }
                                          }
                                      }
                                      return leds;
                                  }()
                            },
                            {
                                  [controller_ptr](){
                                      matrix_map_type matrix_map;

                                      matrix_map.height = controller_ptr->getKeyMap().m_height;
                                      matrix_map.width  = controller_ptr->getKeyMap().m_width;

                                      int i = 0;
                                      for (int y = 0; y < controller_ptr->getKeyMap().m_height; y++)
                                      {
                                          for (int x = 0; x < controller_ptr->getKeyMap().m_width; x++)
                                          {
                                              if(controller_ptr->getKeyMap().m_keyCodes[x][y] == 0x0000)
                                              {
                                                  matrix_map.map.push_back(NA);
                                              }
                                              else
                                              {
                                                  matrix_map.map.push_back(i);
                                                  i++;
                                              }
                                          }
                                      }
                                      return matrix_map;
                                  }()
                            },
                            0 /* start_idx */,
                        }
                    }
                    ),
    controller(controller_ptr),
    m_timerId(-1)
{
    /*
     * Read active profile settings
     */
    DeviceRefresh();


    /*
     * Start timer for captureData rendering if needed
     */
    if(std::find_if(m_effects.begin(),m_effects.end(),[](const led_group_effect& effect){
           return effect.m_mode == MODE_LEGION_AURASYNC;
       }) != m_effects.end())
    {
        LOG_D("Starting timer for direct control mode");

        controller->setLedsDirectOn(toControlerProfile(m_profiles.active));
        m_timerId = startTimer(100);
    }
}

LenovoRGBController::~LenovoRGBController()
{}


void LenovoRGBController::DeviceUpdateProfile()
{
    auto waitForApplyProfileOnController = [this](int maxWaitTimeinMicros = 1000000) {
        while(fromControlerProfile(controller->getCurrentProfileId()) != m_profiles.active)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            maxWaitTimeinMicros -= 100;

            if(maxWaitTimeinMicros <= 0)
            {
                break;
            }
        }
    };

    controller->setProfile(toControlerProfile(m_profiles.active));

    waitForApplyProfileOnController();

    readActiveProfileSettings();
}

void LenovoRGBController::DeviceUpdateBrightness()
{
    controller->setBrightness(static_cast<uint8_t>(m_britnesses.active));
}

std::vector<RGBColor> LenovoRGBController::DeviceGetState() const
{
    std::vector<RGBColor> colors;

    auto state = controller->getState();

    for (const auto & led : m_leds)
    {
        if(state.find(led.value) != state.end())
        {
            colors.push_back(state.at(led.value));
        }
        else
        {
            colors.push_back(0x0);
        }
    }

    return colors;
}

void LenovoRGBController::DeviceRefresh()
{
    /*
     * Read active profile
     */
    m_profiles.active =  fromControlerProfile(controller->getCurrentProfileId());

    /*
     * Read brightness
     */
    m_britnesses.active = controller->getCurrentBrightness();

    /*
     * Read active profile settings
     */
    readActiveProfileSettings();
}

void LenovoRGBController::DeviceUpdateEfects()
{
    std::vector<LenovoUSBController::led_group> groups;
    bool auraModePresent = false;

    for (auto & effect : m_effects)
    {
        if(effect.m_mode == MODE_LEGION_AURASYNC)
        {
            auraModePresent = true;
        }

        groups.push_back({
            .m_mode         = static_cast<uint8_t>(effect.m_mode),
            .m_speed        = static_cast<uint8_t>(effect.m_speed),
            .m_spin         = toControlerSpin(effect.m_direction),
            .m_direction    = toControlerDirection(effect.m_direction),
            .m_color_mode   = toControlerColorMode(effect.m_color_mode),
            .m_colors       = effect.m_colors,
            .m_leds         = [&effect]() {
                                std::set<uint16_t> leds;
                                for(const auto& led: effect.m_leds)
                                {
                                    leds.insert(led.value);
                                }
                                return std::vector<uint16_t>(leds.begin(), leds.end());
                            }()
        });
    }

    if(auraModePresent)
    {
        if(m_timerId == -1)
        {
            LOG_D("Starting timer for direct control mode");

            controller->setProfileDescription(toControlerProfile(m_profiles.active), groups);
            controller->setLedsDirectOn(toControlerProfile(m_profiles.active));
            m_timerId = startTimer(100);
        }
    }
    else
    {
        if(m_timerId != -1)
        {
            LOG_D("Stopping timer for direct control mode");
            killTimer(m_timerId);
            m_timerId = -1;
            controller->setLedsDirectOff(toControlerProfile(m_profiles.active));
        }

        controller->setProfileDescription(toControlerProfile(m_profiles.active), groups);
    }
}

void LenovoRGBController::timerEvent(QTimerEvent *)
{
    LOG_D("LenovoRGBController::timerEvent: Updating direct LED colors");

    m_captureData = [this](){
        std::vector<RGBColor> colors;

        colors.resize(controller->getKeyMap().m_width * controller->getKeyMap().m_height,QRandomGenerator::global()->bounded(0xFFFFFF));

        return colors;
    }();


    controller->setLedsDirect(m_leds,m_captureData);
}

void LenovoRGBController::DeviceResetEffectsToDefault()
{
    controller->setProfileDefault(toControlerProfile(m_profiles.active));
}

void LenovoRGBController::DeviceUpdateDirect()
{
    LOG_W("Direct control is not supported for Lenovo RGB Controller"); // TODO
}

void LenovoRGBController::readActiveProfileSettings()
{
    m_effects.clear();

    /*---------------------------------------------------------*\
    | Retrieve current values by readingledIdToIndex the device |
    \*---------------------------------------------------------*/
    std::vector<LenovoUSBController::led_group> l_currentSettings = controller->getProfileDescription(toControlerProfile(m_profiles.active));


    for (const auto & grub: l_currentSettings)
    {
        m_effects.push_back(led_group_effect{
            .m_mode        = grub.m_mode,
            .m_speed       = grub.m_speed,
            .m_direction   = fromControlerDirection(grub.m_direction) == MODE_DIRECTION_NA ? fromControlerSpin(grub.m_spin) : fromControlerDirection(grub.m_direction),
            .m_color_mode  = fromControlerColorMode(m_modes [std::distance(m_modes.begin(),std::find_if(m_modes.begin(),m_modes.end(),[&grub](const mode& m){ return m.value == grub.m_mode;})) ].flags, grub.m_color_mode),
            .m_colors      = grub.m_colors,
            .m_leds        = [this,&grub](){
                                std::vector<led> leds;

                                std::multimap<unsigned int,size_t>     l_ledIdToIndex;

                                /*
                                 * Build the led id to index map
                                 */
                                for (size_t idx = 0; idx < this->m_leds.size(); ++idx) {
//                                    LOG_D(QString::asprintf("Mapping LED Index %d to ID %X", idx, this->m_leds.at(idx).value));
                                    l_ledIdToIndex.insert(std::pair(this->m_leds.at(idx).value,idx));
                                }

                                for (uint16_t ledId : grub.m_leds)
                                {
 //                                   LOG_D(QString::asprintf("Mapping LED ID %X to colors", ledId));
                                    auto range = l_ledIdToIndex.equal_range(ledId);

                                    for (auto i = range.first; i != range.second; ++i)
                                    {
 //                                       LOG_D(QString::asprintf(" - Setting color for LED Index %d", i->second));
                                        leds.push_back(this->m_leds.at(i->second));
                                    }
                                }

                                return leds;
                            }()
        });
    }
}

unsigned int LenovoRGBController::fromControlerColorMode(const unsigned int  modeFlags,const uint8_t color_mode) const
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

unsigned int LenovoRGBController::fromControlerDirection(const uint8_t direction) const
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

    return MODE_DIRECTION_NA;
}

unsigned int LenovoRGBController::fromControlerSpin(const uint8_t spin) const
{
    switch(spin)
    {
    case 0x01:
        return MODE_DIRECTION_SPINLEFT;
        break;
    case 0x02:
        return MODE_DIRECTION_SPINRIGHT;
        break;
    }

    return MODE_DIRECTION_NA;
}

unsigned int LenovoRGBController::fromControlerBrightness(uint8_t brightness) const
{
    if(brightness < m_britnesses.min || brightness > m_britnesses.max)
    {
        LOG_E("Received brightness level from controller is out of range");
        return m_britnesses.min;
    }

    return static_cast<uint8_t>(brightness);
}

unsigned int LenovoRGBController::fromControlerProfile(uint8_t profile) const
{
    if(profile < m_profiles.min || profile > m_profiles.max)
    {
        LOG_E("Received profile id from controller is out of range");
        return m_profiles.min;
    }

    return static_cast<uint8_t>(profile);
}

uint8_t LenovoRGBController::toControlerDirection(unsigned int  direction) const
{
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

uint8_t LenovoRGBController::toControlerSpin(unsigned int spin) const
{
    switch(spin)
    {
    case MODE_DIRECTION_SPINRIGHT:
        return 0x02;
        break;

    case MODE_DIRECTION_SPINLEFT:
        return 0x01;
        break;
    default:
        return 0x00;
        break;
    }
}

uint8_t LenovoRGBController::toControlerColorMode(unsigned int color_mode) const
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

uint8_t LenovoRGBController::toControlerBrightness(unsigned int brightness) const
{
    if(brightness < m_britnesses.min || brightness > m_britnesses.max)
    {
        LOG_E("Trying to set brightness level to controller which is out of range");
        return static_cast<uint8_t>(m_britnesses.min);
    }

    return static_cast<uint8_t>(brightness);
}

uint8_t LenovoRGBController::toControlerProfile(unsigned int profile) const
{
    if(profile < m_profiles.min || profile > m_profiles.max)
    {
        LOG_E("Trying to set profile id to controller which is out of range");
        return static_cast<uint8_t>(m_profiles.min);
    }

    return static_cast<uint8_t>(profile);
}

}
