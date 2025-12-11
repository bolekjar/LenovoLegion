// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "RGBController.h"
#include "DataProvider.h"

#include "../LenovoLegion-Daemon/DataProviderRGBController.h"

#include <Core/LoggerHolder.h>

namespace LenovoLegionGui {

RGBController::RGBController(DataProvider* dataProvider)
    : m_dataProvider(dataProvider)
    , m_callback(nullptr)
    , m_callback_arg(nullptr)
    , m_hasPendingChanges(false)
{
    refresh();
}

RGBController::~RGBController()
{
}

void RGBController::refresh()
{
    readRGBControllerData();
    convertProtobufToNative();
}

void RGBController::readRGBControllerData()
{
    m_rgbControllerData = m_dataProvider->getDataMessage<legion::messages::RGBController>(
        LenovoLegionDaemon::DataProviderRGBController::dataType);
}

void RGBController::sendRGBControllerData()
{
    convertNativeToProtobuf();
    m_dataProvider->setDataMessage(
        LenovoLegionDaemon::DataProviderRGBController::dataType, 
        m_rgbControllerData);
    m_hasPendingChanges = false;
}

void RGBController::convertProtobufToNative()
{
    // Clear existing data
    m_leds.clear();
    m_zones.clear();
    m_modes.clear();
    m_colors.clear();

    // Convert LEDs
    for(int i = 0; i < m_rgbControllerData.leds_size(); i++)
    {
        const auto& pbLed = m_rgbControllerData.leds(i);
        LenovoLegionDaemon::led led;
        led.name = pbLed.name();
        led.value = pbLed.value();
        m_leds.push_back(led);
    }

    // Convert Zones
    for(int i = 0; i < m_rgbControllerData.zones_size(); i++)
    {
        const auto& pbZone = m_rgbControllerData.zones(i);
        LenovoLegionDaemon::zone zone;
        
        zone.name = pbZone.name();
        zone.type = static_cast<LenovoLegionDaemon::zone_type>(pbZone.type());
        zone.start_idx = pbZone.start_idx();
        zone.leds_count = pbZone.leds_count();
        zone.leds_min = pbZone.leds_min();
        zone.leds_max = pbZone.leds_max();
        zone.flags = pbZone.flags();

        // Allocate and copy LEDs
        if(pbZone.leds_size() > 0)
        {
            zone.leds = new LenovoLegionDaemon::led[pbZone.leds_size()];
            for(int j = 0; j < pbZone.leds_size(); j++)
            {
                zone.leds[j].name = pbZone.leds(j).name();
                zone.leds[j].value = pbZone.leds(j).value();
            }
        }
        else
        {
            zone.leds = nullptr;
        }

        // Allocate and copy colors
        if(pbZone.colors_size() > 0)
        {
            zone.colors = new LenovoLegionDaemon::RGBColor[pbZone.colors_size()];
            for(int j = 0; j < pbZone.colors_size(); j++)
            {
                zone.colors[j] = pbZone.colors(j);
            }
        }
        else
        {
            zone.colors = nullptr;
        }

        // Copy matrix map if present
        if(pbZone.has_matrix_map())
        {
            zone.matrix_map = new LenovoLegionDaemon::matrix_map_type;
            zone.matrix_map->height = pbZone.matrix_map().height();
            zone.matrix_map->width = pbZone.matrix_map().width();
            
            unsigned int mapSize = zone.matrix_map->height * zone.matrix_map->width;
            if(mapSize > 0)
            {
                zone.matrix_map->map = new unsigned int[mapSize];
                for(unsigned int j = 0; j < mapSize && j < static_cast<unsigned int>(pbZone.matrix_map().map_size()); j++)
                {
                    zone.matrix_map->map[j] = pbZone.matrix_map().map(j);
                }
            }
            else
            {
                zone.matrix_map->map = nullptr;
            }
        }
        else
        {
            zone.matrix_map = nullptr;
        }

        // Copy segments
        for(int j = 0; j < pbZone.segments_size(); j++)
        {
            const auto& pbSegment = pbZone.segments(j);
            LenovoLegionDaemon::segment seg;
            seg.name = pbSegment.name();
            seg.type = static_cast<LenovoLegionDaemon::zone_type>(pbSegment.type());
            seg.start_idx = pbSegment.start_idx();
            seg.leds_count = pbSegment.leds_count();
            zone.segments.push_back(seg);
        }

        m_zones.push_back(zone);
    }

    // Convert Modes
    for(int i = 0; i < m_rgbControllerData.modes_size(); i++)
    {
        const auto& pbMode = m_rgbControllerData.modes(i);
        LenovoLegionDaemon::mode mode;
        
        mode.name = pbMode.name();
        mode.value = pbMode.value();
        mode.flags = pbMode.flags();
        mode.speed_min = pbMode.speed_min();
        mode.speed_max = pbMode.speed_max();
        mode.brightness_min = pbMode.brightness_min();
        mode.brightness_max = pbMode.brightness_max();
        mode.colors_min = pbMode.colors_min();
        mode.colors_max = pbMode.colors_max();
        mode.speed = pbMode.speed();
        mode.brightness = pbMode.brightness();
        mode.direction = pbMode.direction();
        mode.color_mode = pbMode.color_mode();

        // Copy colors
        for(int j = 0; j < pbMode.colors_size(); j++)
        {
            mode.colors.push_back(pbMode.colors(j));
        }

        m_modes.push_back(mode);
    }

    // Convert Colors
    for(int i = 0; i < m_rgbControllerData.colors_size(); i++)
    {
        m_colors.push_back(m_rgbControllerData.colors(i));
    }
}

void RGBController::convertNativeToProtobuf()
{
    m_rgbControllerData.clear_leds();
    m_rgbControllerData.clear_zones();
    m_rgbControllerData.clear_modes();
    m_rgbControllerData.clear_colors();

    // Convert LEDs
    for(const auto& led : m_leds)
    {
        auto* pbLed = m_rgbControllerData.add_leds();
        pbLed->set_name(led.name);
        pbLed->set_value(led.value);
    }

    // Convert Modes
    for(const auto& mode : m_modes)
    {
        auto* pbMode = m_rgbControllerData.add_modes();
        pbMode->set_name(mode.name);
        pbMode->set_value(mode.value);
        pbMode->set_flags(mode.flags);
        pbMode->set_speed_min(mode.speed_min);
        pbMode->set_speed_max(mode.speed_max);
        pbMode->set_brightness_min(mode.brightness_min);
        pbMode->set_brightness_max(mode.brightness_max);
        pbMode->set_colors_min(mode.colors_min);
        pbMode->set_colors_max(mode.colors_max);
        pbMode->set_speed(mode.speed);
        pbMode->set_brightness(mode.brightness);
        pbMode->set_direction(mode.direction);
        pbMode->set_color_mode(mode.color_mode);

        for(const auto& color : mode.colors)
        {
            pbMode->add_colors(color);
        }
    }

    // Convert Colors
    for(const auto& color : m_colors)
    {
        m_rgbControllerData.add_colors(color);
    }
}

// Interface implementation

unsigned int RGBController::GetLEDsInZone(unsigned int zone)
{
    if(zone >= m_zones.size())
    {
        return 0;
    }
    return m_zones[zone].leds_count;
}

std::string RGBController::GetModeName(unsigned int mode)
{
    if(mode >= m_modes.size())
    {
        return "";
    }
    return m_modes[mode].name;
}

std::string RGBController::GetZoneName(unsigned int zone)
{
    if(zone >= m_zones.size())
    {
        return "";
    }
    return m_zones[zone].name;
}

std::string RGBController::GetLEDName(unsigned int led)
{
    if(led >= m_leds.size())
    {
        return "";
    }
    return m_leds[led].name;
}

LenovoLegionDaemon::RGBColor RGBController::GetLED(unsigned int led)
{
    if(led >= m_colors.size())
    {
        return 0;
    }
    return m_colors[led];
}

void RGBController::SetLED(unsigned int led, LenovoLegionDaemon::RGBColor color)
{
    if(led >= m_colors.size())
    {
        m_colors.resize(led + 1, 0);
    }
    m_colors[led] = color;
    m_hasPendingChanges = true;
}

void RGBController::SetAllLEDs(LenovoLegionDaemon::RGBColor color)
{
    for(auto& c : m_colors)
    {
        c = color;
    }
    m_hasPendingChanges = true;
}

int RGBController::GetMode()
{
    return m_rgbControllerData.active_mode();
}

void RGBController::SetMode(int mode)
{
    m_rgbControllerData.set_active_mode(mode);
    sendRGBControllerData();
}

void RGBController::RegisterUpdateCallback(LenovoLegionDaemon::RGBControllerCallback new_callback, void* new_callback_arg)
{
    m_callback = new_callback;
    m_callback_arg = new_callback_arg;
}

void RGBController::UpdateLEDs()
{
    sendRGBControllerData();
    
    if(m_callback)
    {
        m_callback(m_callback_arg);
    }
}

void RGBController::UpdateMode()
{
    sendRGBControllerData();
    
    if(m_callback)
    {
        m_callback(m_callback_arg);
    }
}

void RGBController::SaveMode()
{
    sendRGBControllerData();
}

void RGBController::ClearSegments(int zone)
{
    if(zone < 0 || zone >= static_cast<int>(m_zones.size()))
    {
        return;
    }
    m_zones[zone].segments.clear();
    m_hasPendingChanges = true;
}

void RGBController::AddSegment(int zone, LenovoLegionDaemon::segment new_segment)
{
    if(zone < 0 || zone >= static_cast<int>(m_zones.size()))
    {
        return;
    }
    m_zones[zone].segments.push_back(new_segment);
    m_hasPendingChanges = true;
}

void RGBController::SetDeviceProfile(size_t profileIdx)
{
    m_rgbControllerData.set_active_profile(profileIdx);
    sendRGBControllerData();
}

void RGBController::SetupZones()
{
    // Not implemented for client-side
}

void RGBController::ResizeZone(int zone, int new_size)
{
    // Not implemented for client-side
    (void)zone;
    (void)new_size;
}

void RGBController::SetCustomMode()
{
    // Not implemented for client-side
}

bool RGBController::IsApplayingSettingsInProgress() const
{
    return m_rgbControllerData.applying_settings();
}

const std::vector<LenovoLegionDaemon::led>& RGBController::GetLEDs() const
{
    return m_leds;
}

void RGBController::SetLEDs(const std::vector<LenovoLegionDaemon::led>& new_leds)
{
    m_leds = new_leds;
    m_hasPendingChanges = true;
}

const std::vector<LenovoLegionDaemon::zone>& RGBController::GetZones() const
{
    return m_zones;
}

const std::vector<LenovoLegionDaemon::mode>& RGBController::GetModes() const
{
    return m_modes;
}

void RGBController::SetModes(const std::vector<LenovoLegionDaemon::mode>& new_modes)
{
    m_modes = new_modes;
    m_hasPendingChanges = true;
}

const std::vector<LenovoLegionDaemon::RGBColor>& RGBController::GetColors() const
{
    return m_colors;
}

LenovoLegionDaemon::device_type RGBController::GetDeviceType() const
{
    return static_cast<LenovoLegionDaemon::device_type>(m_rgbControllerData.device_type());
}

unsigned int RGBController::GetProfiles() const
{
    return m_rgbControllerData.profiles();
}

size_t RGBController::GetActiveProfile() const
{
    return m_rgbControllerData.active_profile();
}

}
