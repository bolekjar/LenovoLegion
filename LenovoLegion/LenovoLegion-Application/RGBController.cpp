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
#include "../LenovoLegion-PrepareBuild/RGBController.pb.h"

#include <Core/LoggerHolder.h>

namespace LenovoLegionGui {

RGBController::RGBController(DataProvider* dataProvider)
    : m_dataProvider(dataProvider)
{
    refresh();
}

RGBController::~RGBController()
{}

void RGBController::refresh()
{
    readRGBControllerData();
}

void RGBController::readRGBControllerData()
{
    legion::messages::RGBController rgbControllerData = m_dataProvider->getDataMessage<legion::messages::RGBController>(LenovoLegionDaemon::DataProviderRGBController::dataType);

    m_modes.clear();
    m_zones.clear();
    m_leds.clear();
    m_colors.clear();

    m_profiles      = rgbControllerData.profiles();
    m_activeProfile = rgbControllerData.active_profile();
    m_activeMode    = rgbControllerData.active_mode();
    m_deviceType    = static_cast<LenovoLegionDaemon::device_type>(rgbControllerData.device_type());


    // Convert LEDs
    for(int i = 0; i < rgbControllerData.leds_size(); i++)
    {
        const auto& pbLed = rgbControllerData.leds(i);
        LenovoLegionDaemon::led led;
        led.name = pbLed.name();
        led.value = pbLed.value();
        m_leds.push_back(led);
    }

    // Convert Colors
    for(int i = 0; i < rgbControllerData.colors_size(); i++)
    {
        m_colors.push_back(rgbControllerData.colors(i));
    }

    // Convert Zones
    for(int i = 0; i < rgbControllerData.zones_size(); i++)
    {
        const auto& pbZone = rgbControllerData.zones(i);
        LenovoLegionDaemon::zone& zone = m_zones.emplace_back();

        zone.name = pbZone.name();
        zone.type = static_cast<LenovoLegionDaemon::zone_type>(pbZone.type());
        zone.start_idx = pbZone.start_idx();
        zone.leds_count = pbZone.leds_count();
        zone.leds_min = pbZone.leds_min();
        zone.leds_max = pbZone.leds_max();
        zone.flags = pbZone.flags();


        zone.leds   = &m_leds[zone.start_idx];
        zone.colors = &m_colors[zone.start_idx];


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
    }

    // Convert Modes
    for(int i = 0; i < rgbControllerData.modes_size(); i++)
    {
        const auto& pbMode = rgbControllerData.modes(i);
        LenovoLegionDaemon::mode& mode = m_modes.emplace_back();

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
    }

}

void RGBController::sendRGBControllerData()
{

    legion::messages::RGBController rgbControllerData;

    if(m_pendingChanges.test(CHANGE_ACTIVE_MODE))
    {
        rgbControllerData.set_active_mode(m_activeMode);
    }

    if(m_pendingChanges.test(CHANGE_LEDS))
    {
        for(const auto& led : m_leds)
        {
            auto* pbLed = rgbControllerData.add_leds();
            pbLed->set_name(led.name);
            pbLed->set_value(led.value);
        }
    }

    if(m_pendingChanges.test(CHANGE_MODES))
    {
        for(const auto& mode : m_modes)
        {
            auto* pbMode = rgbControllerData.add_modes();
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
    }

    if(m_pendingChanges.test(CHANGE_PROFILES))
    {
        rgbControllerData.set_active_profile(m_activeProfile);
    }

    if(m_pendingChanges.test(CHANGE_COLORS))
    {
        for (const auto& color : m_colors)
        {
            rgbControllerData.add_colors(color);
        }
    }

    if(m_pendingChanges.any())
    {
        m_dataProvider->setDataMessage(LenovoLegionDaemon::DataProviderRGBController::dataType,rgbControllerData);
    }

    if(m_pendingChanges.test(CHANGE_PROFILES))
    {
        readRGBControllerData();
    }

    m_pendingChanges.reset();
}

unsigned int RGBController::GetLEDsInZone(unsigned int zone) const
{
    if(zone >= static_cast<unsigned int>(m_zones.size()))
    {
        return 0;
    }
    return m_zones[zone].leds_count;
}

std::string RGBController::GetModeName(unsigned int mode) const
{
    if(mode >= static_cast<unsigned int>(m_modes.size()))
    {
        return "";
    }
    return m_modes[mode].name;
}

std::string RGBController::GetZoneName(unsigned int zone) const
{
    if(zone >= static_cast<unsigned int>(m_zones.size()))
    {
        return "";
    }
    return m_zones[zone].name;
}

std::string RGBController::GetLEDName(unsigned int led) const
{
    if(led >= static_cast<unsigned int>(m_leds.size()))
    {
        return "";
    }
    return m_leds[led].name;
}

LenovoLegionDaemon::RGBColor RGBController::GetLED(unsigned int led) const
{
    if(led >= static_cast<unsigned int>(m_colors.size()))
    {
        return 0;
    }
    return m_colors[led];
}

void RGBController::SetLED(unsigned int led, LenovoLegionDaemon::RGBColor color)
{
    m_colors[led] = color;
    m_pendingChanges.set(CHANGE_COLORS);
}

void RGBController::SetAllLEDs(LenovoLegionDaemon::RGBColor color)
{
    for(size_t i = 0; i < m_colors.size(); i++)
    {
        m_colors[i] = color;
    }
    m_pendingChanges.set(CHANGE_COLORS);
}

int RGBController::GetMode() const
{
    return m_activeMode;
}

void RGBController::SetMode(int mode)
{
    m_activeMode = mode;
    m_pendingChanges.set(CHANGE_ACTIVE_MODE);
}


void RGBController::SetProfile(size_t profileIdx)
{
    m_activeProfile = profileIdx;
    m_pendingChanges.set(CHANGE_PROFILES);
}

const std::vector<LenovoLegionDaemon::led>& RGBController::GetLEDs() const
{
    return m_leds;
}

void RGBController::SetLEDs(const std::vector<LenovoLegionDaemon::led>& new_leds)
{
    m_leds = new_leds;
    m_pendingChanges.set(CHANGE_LEDS);
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
    m_pendingChanges.set(CHANGE_MODES);
}

const std::vector<LenovoLegionDaemon::RGBColor>& RGBController::GetColors() const
{
    return m_colors;
}

LenovoLegionDaemon::device_type RGBController::GetDeviceType() const
{
    return m_deviceType;
}

unsigned int RGBController::GetProfiles() const
{
    return m_profiles;;
}

size_t RGBController::GetActiveProfile() const
{
    return m_activeProfile;
}

void RGBController::ApplyPendingChanges()
{
    sendRGBControllerData();
}

}
