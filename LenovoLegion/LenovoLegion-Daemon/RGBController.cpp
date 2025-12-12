// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include <cstring>

#include "RGBController.h"

namespace LenovoLegionDaemon {

RGBController::RGBController()
{}

RGBController::~RGBController()
{
    leds.clear();
    colors.clear();
    zones.clear();
    modes.clear();
}

std::string RGBController::GetName() const
{
    return(name);
}

std::string RGBController::GetVendor() const
{
    return(vendor);
}

std::string RGBController::GetDescription() const
{
    return(description);
}

std::string RGBController::GetVersion() const
{
    return(version);
}

std::string RGBController::GetSerial() const
{
    return(serial);
}

std::string RGBController::GetLocation() const
{
    return(location);
}

std::string RGBController::GetModeName(unsigned int mode) const
{
    return(modes[mode].name);
}

std::string RGBController::GetZoneName(unsigned int zone) const
{
    return(zones[zone].name);
}

std::string RGBController::GetLEDName(unsigned int led) const
{
    if(led < led_alt_names.size())
    {
        if(led_alt_names[led] != "")
        {
            return(led_alt_names[led]);
        }
    }

    return(leds[led].name);
}




void RGBController::SetupColors()
{
    unsigned int total_led_count;
    unsigned int zone_led_count;

    /*---------------------------------------------------------*\
    | Determine total number of LEDs on the device              |
    \*---------------------------------------------------------*/
    total_led_count = 0;

    for(std::size_t zone_idx = 0; zone_idx < zones.size(); zone_idx++)
    {
        total_led_count += GetLEDsInZone(zone_idx);
    }

    /*---------------------------------------------------------*\
    | Set the size of the color buffer to the number of LEDs    |
    \*---------------------------------------------------------*/
    colors.resize(total_led_count);

    /*---------------------------------------------------------*\
    | Set the color buffer pointers on each zone                |
    \*---------------------------------------------------------*/
    total_led_count = 0;

    for(std::size_t zone_idx = 0; zone_idx < zones.size(); zone_idx++)
    {
        zones[zone_idx].start_idx   = total_led_count;
        zone_led_count              = GetLEDsInZone(zone_idx);

        if((colors.size() > 0) && (zone_led_count > 0))
        {
            zones[zone_idx].colors = &colors[total_led_count];
        }
        else
        {
            zones[zone_idx].colors = NULL;
        }

        if((leds.size() > 0) && (zone_led_count > 0))
        {
            zones[zone_idx].leds   = &leds[total_led_count];
        }
        else
        {
            zones[zone_idx].leds    = NULL;
        }


        total_led_count += zone_led_count;
    }
}

unsigned int RGBController::GetLEDsInZone(unsigned int zone) const
{
    unsigned int leds_count = zones[zone].leds_count;

    if(zones[zone].flags & ZONE_FLAG_RESIZE_EFFECTS_ONLY)
    {
        if(leds_count > 1)
        {
            leds_count = 1;
        }
    }

    return(leds_count);
}

RGBColor RGBController::GetLED(unsigned int led) const
{
    if(led < colors.size())
    {
        return(colors[led]);
    }
    else
    {
        return(0x00000000);
    }
}

void RGBController::SetLED(unsigned int led, RGBColor color)
{
    if(led < colors.size())
    {
        colors[led] = color;
    }
}

void RGBController::SetAllLEDs(RGBColor color)
{
    for(std::size_t zone_idx = 0; zone_idx < zones.size(); zone_idx++)
    {
        SetAllZoneLEDs((int)zone_idx, color);
    }
}

void RGBController::SetAllZoneLEDs(int zone, RGBColor color)
{
    for (std::size_t color_idx = 0; color_idx < GetLEDsInZone(zone); color_idx++)
    {
        zones[zone].colors[color_idx] = color;
    }
}

int RGBController::GetMode() const
{
    return(active_mode);
}

void RGBController::SetMode(int mode)
{
    active_mode = mode;
}

void RGBController::SetProfile(size_t profileIdx)
{
    active_profile = profileIdx;
}

std::vector<led> RGBController::GetLEDs() const
{
    return leds;
}

void RGBController::SetLEDs(const std::vector<led> &new_leds)
{
    leds = new_leds;
}

std::vector<zone> RGBController::GetZones() const
{
    return zones;
}

 std::string RGBController::GetControllerName() const
{
    return name;
}

int RGBController::GetActiveMode() const
{
    return active_mode;
}

std::vector<mode> RGBController::GetModes() const
{
    return modes;
}

void RGBController::SetModes(const std::vector<mode> &new_modes)
{
    modes = new_modes;
}

std::vector<RGBColor> RGBController::GetColors() const
{
    return colors;
}

device_type RGBController::GetDeviceType() const
{
    return type;
}

unsigned int RGBController::GetProfiles() const
{
    return profiles;
}

 size_t RGBController::GetActiveProfile() const
{
    return active_profile;
}

void RGBController::DeviceUpdateMode()
{

}

std::string device_type_to_str(device_type type)
{
    switch(type)
    {
    case DEVICE_TYPE_MOTHERBOARD:
        return "Motherboard";
    case DEVICE_TYPE_DRAM:
        return "DRAM";
    case DEVICE_TYPE_GPU:
        return "GPU";
    case DEVICE_TYPE_COOLER:
        return "Cooler";
    case DEVICE_TYPE_LEDSTRIP:
        return "LED Strip";
    case DEVICE_TYPE_KEYBOARD:
        return "Keyboard";
    case DEVICE_TYPE_MOUSE:
        return "Mouse";
    case DEVICE_TYPE_MOUSEMAT:
        return "Mousemat";
    case DEVICE_TYPE_HEADSET:
        return "Headset";
    case DEVICE_TYPE_HEADSET_STAND:
        return "Headset Stand";
    case DEVICE_TYPE_GAMEPAD:
        return "Gamepad";
    case DEVICE_TYPE_LIGHT:
        return "Light";
    case DEVICE_TYPE_SPEAKER:
        return "Speaker";
    case DEVICE_TYPE_VIRTUAL:
        return "Virtual";
    case DEVICE_TYPE_STORAGE:
        return "Storage";
    case DEVICE_TYPE_CASE:
        return "Case";
    case DEVICE_TYPE_MICROPHONE:
        return "Microphone";
    case DEVICE_TYPE_ACCESSORY:
        return "Accessory";
    case DEVICE_TYPE_KEYPAD:
        return "Keypad";
    default:
        return "Unknown";
    }
}

}
