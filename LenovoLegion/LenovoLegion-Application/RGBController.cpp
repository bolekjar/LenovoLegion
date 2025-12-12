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
    m_rgbControllerData = m_dataProvider->getDataMessage<legion::messages::RGBController>(LenovoLegionDaemon::DataProviderRGBController::dataType);
}

void RGBController::sendRGBControllerData()
{

    legion::messages::RGBController rgbControllerData = m_rgbControllerData;

    if(!m_pendingChanges.test(CHANGE_ACTIVE_MODE))
    {
        rgbControllerData.clear_active_mode();
    }

    if(!m_pendingChanges.test(CHANGE_LEDS))
    {
        rgbControllerData.clear_leds();
    }

    if(!m_pendingChanges.test(CHANGE_MODES))
    {
        rgbControllerData.clear_modes();
    }

    if(!m_pendingChanges.test(CHANGE_PROFILES))
    {
        rgbControllerData.clear_active_profile();
    }

    if(!m_pendingChanges.test(CHANGE_COLORS))
    {
        rgbControllerData.clear_colors();
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
    if(zone >= static_cast<unsigned int>(m_rgbControllerData.zones().size()))
    {
        return 0;
    }
    return m_rgbControllerData.zones().at(zone).leds_count();
}

std::string RGBController::GetModeName(unsigned int mode) const
{
    if(mode >= static_cast<unsigned int>(m_rgbControllerData.modes().size()))
    {
        return "";
    }
    return m_rgbControllerData.modes().at(mode).name().data();
}

std::string RGBController::GetZoneName(unsigned int zone) const
{
    if(zone >= static_cast<unsigned int>(m_rgbControllerData.zones().size()))
    {
        return "";
    }
    return m_rgbControllerData.zones().at(zone).name().data();
}

std::string RGBController::GetLEDName(unsigned int led) const
{
    if(led >= static_cast<unsigned int>(m_rgbControllerData.leds().size()))
    {
        return "";
    }
    return m_rgbControllerData.leds().at(led).name().data();
}

LenovoLegionDaemon::RGBColor RGBController::GetLED(unsigned int led) const
{
    if(led >= static_cast<unsigned int>(m_rgbControllerData.colors().size()))
    {
        return 0;
    }
    return m_rgbControllerData.colors().at(led);
}

void RGBController::SetLED(unsigned int led, LenovoLegionDaemon::RGBColor color)
{
    m_rgbControllerData.mutable_colors()->at(led) = color;
    m_pendingChanges.set(CHANGE_COLORS);
}

void RGBController::SetAllLEDs(LenovoLegionDaemon::RGBColor color)
{
    for(int i = 0; i < m_rgbControllerData.colors_size(); i++)
    {
        m_rgbControllerData.mutable_colors()->at(i) = color;
    }
    m_pendingChanges.set(CHANGE_COLORS);
}

int RGBController::GetMode() const
{
    return m_rgbControllerData.active_mode();
}

void RGBController::SetMode(int mode)
{
    m_rgbControllerData.set_active_mode(mode);
    m_pendingChanges.set(CHANGE_ACTIVE_MODE);
}


void RGBController::SetProfile(size_t profileIdx)
{
    m_rgbControllerData.set_active_profile(profileIdx);
    m_pendingChanges.set(CHANGE_PROFILES);
}

std::vector<LenovoLegionDaemon::led> RGBController::GetLEDs() const
{
    std::vector<LenovoLegionDaemon::led> leds;

    // Convert LEDs
    for(int i = 0; i < m_rgbControllerData.leds_size(); i++)
    {
        const auto& pbLed = m_rgbControllerData.leds(i);
        LenovoLegionDaemon::led led;
        led.name = pbLed.name();
        led.value = pbLed.value();
        leds.push_back(led);
    }

    return leds;
}

void RGBController::SetLEDs(const std::vector<LenovoLegionDaemon::led>& new_leds)
{
    for (size_t i = 0; i < new_leds.size(); i++)
    {
        m_rgbControllerData.mutable_leds()->at(i).set_name(new_leds[i].name);
        m_rgbControllerData.mutable_leds()->at(i).set_value(new_leds[i].value);
    }
    m_pendingChanges.set(CHANGE_LEDS);
}

std::vector<LenovoLegionDaemon::zone> RGBController::GetZones() const
{
    std::vector<LenovoLegionDaemon::zone> zones;

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

        zones.push_back(zone);
    }

    return zones;
}

std::vector<LenovoLegionDaemon::mode> RGBController::GetModes() const
{
    std::vector<LenovoLegionDaemon::mode> modes;

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

        modes.push_back(mode);
    }

    return modes;
}

void RGBController::SetModes(const std::vector<LenovoLegionDaemon::mode>& new_modes)
{
    for (size_t i = 0; i < new_modes.size(); i++)
    {
        m_rgbControllerData.mutable_modes()->at(i).set_name(new_modes[i].name);
        m_rgbControllerData.mutable_modes()->at(i).set_value(new_modes[i].value);
        m_rgbControllerData.mutable_modes()->at(i).set_flags(new_modes[i].flags);
        m_rgbControllerData.mutable_modes()->at(i).set_speed_min(new_modes[i].speed_min);
        m_rgbControllerData.mutable_modes()->at(i).set_speed_max(new_modes[i].speed_max);
        m_rgbControllerData.mutable_modes()->at(i).set_brightness_min(new_modes[i].brightness_min);
        m_rgbControllerData.mutable_modes()->at(i).set_brightness_max(new_modes[i].brightness_max);
        m_rgbControllerData.mutable_modes()->at(i).set_colors_min(new_modes[i].colors_min);
        m_rgbControllerData.mutable_modes()->at(i).set_colors_max(new_modes[i].colors_max);
        m_rgbControllerData.mutable_modes()->at(i).set_speed(new_modes[i].speed);
        m_rgbControllerData.mutable_modes()->at(i).set_brightness(new_modes[i].brightness);
        m_rgbControllerData.mutable_modes()->at(i).set_direction(new_modes[i].direction);
        m_rgbControllerData.mutable_modes()->at(i).set_color_mode(new_modes[i].color_mode);

        // Clear existing colors
        m_rgbControllerData.mutable_modes()->at(i).clear_colors();

        for (const auto& color : new_modes[i].colors)
        {
            m_rgbControllerData.mutable_modes()->at(i).add_colors(color);
        }
    }


    m_pendingChanges.set(CHANGE_MODES);
}

std::vector<LenovoLegionDaemon::RGBColor> RGBController::GetColors() const
{
    std::vector<LenovoLegionDaemon::RGBColor> colors;

    // Convert Colors
    for(int i = 0; i < m_rgbControllerData.colors_size(); i++)
    {
        colors.push_back(m_rgbControllerData.colors(i));
    }

    return colors;
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

void RGBController::ApplyPendingChanges()
{
    sendRGBControllerData();
}

}
