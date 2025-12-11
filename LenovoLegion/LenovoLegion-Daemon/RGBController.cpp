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
{
    flags       = 0;
    DeviceThreadRunning = true;
    DeviceCallThread = new std::thread(&RGBController::DeviceCallThreadFunction, this);
}

RGBController::~RGBController()
{
    DeviceThreadRunning = false;
    DeviceCallThread->join();
    delete DeviceCallThread;

    leds.clear();
    colors.clear();
    zones.clear();
    modes.clear();
}

std::string RGBController::GetName()
{
    return(name);
}

std::string RGBController::GetVendor()
{
    return(vendor);
}

std::string RGBController::GetDescription()
{
    return(description);
}

std::string RGBController::GetVersion()
{
    return(version);
}

std::string RGBController::GetSerial()
{
    return(serial);
}

std::string RGBController::GetLocation()
{
    return(location);
}

std::string RGBController::GetModeName(unsigned int mode)
{
    return(modes[mode].name);
}

std::string RGBController::GetZoneName(unsigned int zone)
{
    return(zones[zone].name);
}

std::string RGBController::GetLEDName(unsigned int led)
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

unsigned int RGBController::GetLEDsInZone(unsigned int zone)
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

RGBColor RGBController::GetLED(unsigned int led)
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

int RGBController::GetMode()
{
    return(active_mode);
}

void RGBController::SetMode(int mode)
{
    active_mode = mode;

    UpdateMode();
}

void RGBController::RegisterUpdateCallback(RGBControllerCallback new_callback, void * new_callback_arg)
{
    UpdateCallbacks.push_back(new_callback);
    UpdateCallbackArgs.push_back(new_callback_arg);
}

void RGBController::UnregisterUpdateCallback(void * callback_arg)
{
    for(unsigned int callback_idx = 0; callback_idx < UpdateCallbackArgs.size(); callback_idx++ )
    {
        if(UpdateCallbackArgs[callback_idx] == callback_arg)
        {
            UpdateCallbackArgs.erase(UpdateCallbackArgs.begin() + callback_idx);
            UpdateCallbacks.erase(UpdateCallbacks.begin() + callback_idx);

            break;
        }
    }
}

void RGBController::ClearCallbacks()
{
    UpdateCallbacks.clear();
    UpdateCallbackArgs.clear();
}

void RGBController::SignalUpdate()
{
    UpdateMutex.lock();

    /*-------------------------------------------------*\
    | Client info has changed, call the callbacks       |
    \*-------------------------------------------------*/
    for(unsigned int callback_idx = 0; callback_idx < UpdateCallbacks.size(); callback_idx++)
    {
        UpdateCallbacks[callback_idx](UpdateCallbackArgs[callback_idx]);
    }

    UpdateMutex.unlock();
}
void RGBController::UpdateLEDs()
{
    CallFlag_UpdateLEDs = true;

    SignalUpdate();
}

void RGBController::UpdateMode()
{
    CallFlag_UpdateMode = true;
}

void RGBController::SaveMode()
{
    DeviceSaveMode();
}

void RGBController::DeviceUpdateLEDs()
{

}

void RGBController::SetCustomMode()
{
    /*-------------------------------------------------*\
    | Search the Controller's mode list for a suitable  |
    | per-LED custom mode in the following order:       |
    | 1.    Direct                                      |
    | 2.    Custom                                      |
    | 3.    Static                                      |
    \*-------------------------------------------------*/
    #define NUM_CUSTOM_MODE_NAMES 3

    const std::string custom_mode_names[] =
    {
        "Direct",
        "Custom",
        "Static"
    };

    for(unsigned int custom_mode_idx = 0; custom_mode_idx < NUM_CUSTOM_MODE_NAMES; custom_mode_idx++)
    {
        for(unsigned int mode_idx = 0; mode_idx < modes.size(); mode_idx++)
        {
            if((modes[mode_idx].name == custom_mode_names[custom_mode_idx])
            && ((modes[mode_idx].color_mode == MODE_COLORS_PER_LED)
             || (modes[mode_idx].color_mode == MODE_COLORS_MODE_SPECIFIC)))
            {
                active_mode = mode_idx;
                return;
            }
        }
    }
}

unsigned int RGBController::GetFlags() const
{
    return flags;
}

void RGBController::SetFlags(unsigned int new_flags)
{
{ flags = new_flags; }
}

const std::vector<led> &RGBController::GetLEDs() const
{
    return leds;
}

void RGBController::SetLEDs(const std::vector<led> &new_leds)
{
    leds = new_leds;
}

const std::vector<zone> &RGBController::GetZones() const
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

const std::vector<mode> &RGBController::GetModes() const
{
    return modes;
}

void RGBController::SetModes(const std::vector<mode> &new_modes)
{
    modes = new_modes;
}

const std::vector<RGBColor> &RGBController::GetColors() const
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

void RGBController::DeviceCallThreadFunction()
{
    CallFlag_UpdateLEDs = false;
    CallFlag_UpdateMode = false;

    while(DeviceThreadRunning.load() == true)
    {
        if(CallFlag_UpdateMode.load() == true)
        {
            if(flags & CONTROLLER_FLAG_RESET_BEFORE_UPDATE)
            {
                CallFlag_UpdateMode = false;
                DeviceUpdateMode();
            }
            else
            {
                DeviceUpdateMode();
                CallFlag_UpdateMode = false;
            }
        }
        if(CallFlag_UpdateLEDs.load() == true)
        {
            if(flags & CONTROLLER_FLAG_RESET_BEFORE_UPDATE)
            {
                CallFlag_UpdateLEDs = false;
                DeviceUpdateLEDs();
            }
            else
            {
                DeviceUpdateLEDs();
                CallFlag_UpdateLEDs = false;
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

void RGBController::DeviceSaveMode()
{
    /*-------------------------------------------------*\
    | If not implemented by controller, does nothing    |
    \*-------------------------------------------------*/
}

void RGBController::ClearSegments(int zone)
{
    zones[zone].segments.clear();
}

void RGBController::AddSegment(int zone, segment new_segment)
{
    zones[zone].segments.push_back(new_segment);
}

void RGBController::SetDeviceProfile(size_t)
{
    /*-------------------------------------------------*\
    | If not implemented by controller, does nothing    |
    \*-------------------------------------------------*/
}

bool RGBController::IsApplayingSettingsInProgress() const
{
    return CallFlag_UpdateMode == true || CallFlag_UpdateLEDs == true;
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
