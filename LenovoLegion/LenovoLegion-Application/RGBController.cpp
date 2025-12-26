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
    readRGBControllerData(legion::messages::RGBControllerRequest::RequestFlags::RGBControllerRequest_RequestFlags_REQUEST_ALL);
}

RGBController::~RGBController()
{}

void RGBController::readRGBControllerData(const legion::messages::RGBControllerRequest::RequestFlags& requestFlags)
{
    legion::messages::RGBControllerResponse rgbControllerData = m_dataProvider->getDataMessage<legion::messages::RGBControllerResponse,legion::messages::RGBControllerRequest>(LenovoLegionDaemon::DataProviderRGBController::dataType,[&requestFlags]{
        legion::messages::RGBControllerRequest request;
        request.set_request_flags(requestFlags);

        return request;
    }());

    // Device Type
    if(requestFlags & legion::messages::RGBControllerRequest::RequestFlags::RGBControllerRequest_RequestFlags_REQUEST_DEVICE_TYPE)
    {
        m_deviceType = LenovoLegionDaemon::DEVICE_TYPE_UNKNOWN;

        if(rgbControllerData.has_device_type())
        {
            m_deviceType = static_cast<LenovoLegionDaemon::device_type>(rgbControllerData.device_type());
        }
    }

    // Convert Profiles
    if(requestFlags & legion::messages::RGBControllerRequest::RequestFlags::RGBControllerRequest_RequestFlags_REQUEST_PROFILE)
    {
        m_profiles = {0,0,0};

        if(rgbControllerData.has_profile())
        {
            m_profiles = {rgbControllerData.profile().min(),
                          rgbControllerData.profile().max(),
                          rgbControllerData.profile().current()};
        }
    }

    // Convert Brightness
    if(requestFlags & legion::messages::RGBControllerRequest::RequestFlags::RGBControllerRequest_RequestFlags_REQUEST_BRITNESS)
    {
        m_brightness    = {0,0,0};

        if(rgbControllerData.has_britness())
        {
            m_brightness    = {0,0,0};

            m_brightness = {rgbControllerData.britness().min(),
                            rgbControllerData.britness().max(),
                            rgbControllerData.britness().current()};
        }
    }


    // Convert Effects
    if(requestFlags & legion::messages::RGBControllerRequest::RequestFlags::RGBControllerRequest_RequestFlags_REQUEST_LED_GROUP_EFFECTS)
    {
        m_effects.clear();

        for(int i = 0; i < rgbControllerData.led_group_effects_size(); i++)
        {
            LenovoLegionDaemon::led_group_effect effect;

            const auto& pbEffect = rgbControllerData.led_group_effects(i);

            effect.m_mode       = pbEffect.mode();
            effect.m_direction  = pbEffect.direction();
            effect.m_speed      = pbEffect.speed();
            effect.m_color_mode = pbEffect.color_mode();

            for (int j = 0; j < pbEffect.colors_size(); j++)
            {
                effect.m_colors.push_back(pbEffect.colors(j));
            }

            for (int j = 0; j < pbEffect.leds_size(); j++)
            {
                effect.m_leds.push_back({pbEffect.leds(j).name().data(),pbEffect.leds(j).value()});
            }

            m_effects.push_back(effect);
        }
    }

    // Convert LEDs
    if(requestFlags & legion::messages::RGBControllerRequest::RequestFlags::RGBControllerRequest_RequestFlags_REQUEST_LEDS)
    {
        m_leds.clear();

        for(int i = 0; i < rgbControllerData.leds_size(); i++)
        {
            m_leds.emplace_back(rgbControllerData.leds(i).name().data(),rgbControllerData.leds(i).value());
        }
    }

    // Convert Zones
    if(requestFlags & legion::messages::RGBControllerRequest::RequestFlags::RGBControllerRequest_RequestFlags_REQUEST_ZONES)
    {
        m_zones.clear();

        for(int i = 0; i < rgbControllerData.zones_size(); i++)
        {
            const auto& pbZone = rgbControllerData.zones(i);

            m_zones.push_back( {
                .name               = pbZone.name().data(),
                .type               = pbZone.type(),
                .flags              = pbZone.flags(),
                .leds_count         = pbZone.leds_count(),
                .leds_min           = pbZone.leds_min(),
                .leds_max           = pbZone.leds_max(),
                .leds               = m_leds,
                .matrix_map         = [&pbZone](){
                    LenovoLegionDaemon::matrix_map_type matrix;

                    matrix.height = pbZone.matrix_map().height();
                    matrix.width  = pbZone.matrix_map().width();

                    for(int r = 0; r < pbZone.matrix_map().map_size(); r++)
                    {
                        matrix.map.push_back(pbZone.matrix_map().map().at(r));
                    }

                    return matrix;
                }(),
                .start_idx         = 0,
            });
        }
    }
    // Convert Modes
    if(requestFlags & legion::messages::RGBControllerRequest::RequestFlags::RGBControllerRequest_RequestFlags_REQUEST_MODES)
    {
        m_modes.clear();

        for(int i = 0; i < rgbControllerData.modes_size(); i++)
        {
            const auto& pbMode = rgbControllerData.modes(i);
            m_modes.push_back({
                .name           = pbMode.name().data(),
                .value          = pbMode.value(),
                .flags          = pbMode.flags(),
                .speed_min      = pbMode.speed_min(),
                .speed_max      = pbMode.speed_max(),
                .colors_min     = pbMode.colors_min(),
                .colors_max     = pbMode.colors_max(),
                .speed          = pbMode.speed(),
                .direction      = pbMode.direction(),
                .color_mode     = pbMode.color_mode(),
                .colors         = [&pbMode]{
                    std::vector<LenovoLegionDaemon::RGBColor> colors;

                    for (int j = 0; j < pbMode.colors_size(); j++)
                    {
                        colors.push_back(pbMode.colors(j));
                    }

                    return colors;
                }(),

            });
        }
    }
}

void RGBController::sendRGBControllerData()
{

    legion::messages::RGBControllerSetRequest rgbControllerData;

    if(m_pendingChanges.test(CHANGE_PROFILES))
    {
        rgbControllerData.mutable_profile()->set_current(m_profiles.active);
    }

    if(m_pendingChanges.test(CHANGE_BRIGHTNESS))
    {
        rgbControllerData.mutable_britness()->set_current(m_brightness.active);
    }

    if(m_pendingChanges.test(CHANGE_EFFECTS))
    {
        for(const auto& effect : m_effects)
        {
            auto* pbEffect = rgbControllerData.add_led_group_effects();

            pbEffect->set_mode(effect.m_mode);
            pbEffect->set_direction(effect.m_direction);
            pbEffect->set_speed(effect.m_speed);
            pbEffect->set_color_mode(effect.m_color_mode);

            for (const auto& color : effect.m_colors)
            {
                pbEffect->add_colors(color);
            }

            for (const auto& led : effect.m_leds)
            {
                auto* pbLed = pbEffect->add_leds();
                pbLed->set_name(led.name);
                pbLed->set_value(led.value);
            }
        }
    }

    if(m_pendingChanges.test(CHANGE_RESET_EFFECTS))
    {
        rgbControllerData.set_reset_effects_to_def(true);
    }

    if(m_pendingChanges.any())
    {
        m_dataProvider->setDataMessage(LenovoLegionDaemon::DataProviderRGBController::dataType,rgbControllerData);
    }

    if(m_pendingChanges.test(CHANGE_PROFILES) || m_pendingChanges.test(CHANGE_RESET_EFFECTS))
    {
        readRGBControllerData(legion::messages::RGBControllerRequest::RequestFlags::RGBControllerRequest_RequestFlags_REQUEST_LED_GROUP_EFFECTS);
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

std::string RGBController::GetModeNameByIdx(unsigned int mode) const
{
    if(mode >= static_cast<unsigned int>(m_modes.size()))
    {
        return "";
    }
    return m_modes[mode].name;
}

LenovoLegionDaemon::mode RGBController::GetModeByIdx(unsigned int mode) const
{
    if(mode >= static_cast<unsigned int>(m_modes.size()))
    {
        return {};
    }
    return m_modes[mode];
}

LenovoLegionDaemon::mode RGBController::GetModeByModeValue(int mode) const
{
    for (const auto & m : m_modes)
    {
        if(m.value == mode)
        {
            return m;
        }
    }

    return {};
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

void RGBController::SetProfile(unsigned int profileIdx)
{
    m_profiles.active = profileIdx;
    m_pendingChanges.set(CHANGE_PROFILES);
}

const std::vector<LenovoLegionDaemon::led>& RGBController::GetLEDs() const
{
    return m_leds;
}


const std::vector<LenovoLegionDaemon::zone>& RGBController::GetZones() const
{
    return m_zones;
}

const std::vector<LenovoLegionDaemon::mode>& RGBController::GetModes() const
{
    return m_modes;
}

std::set<int> RGBController::GetLedsIndexesByDeviceSpecificValue(unsigned int value) const
{
    return (std::set<int>)[&]() {

        std::set<int> indices;

        for (size_t i = 0; i < m_leds.size(); i++)
        {
            if (m_leds[i].value == value)
            {
                indices.insert(i);
            }
        }
        return indices;
    }();
}

std::vector<LenovoLegionDaemon::RGBColor> RGBController::GetStateForAllLeds() const
{
    legion::messages::RGBControllerResponse rgbControllerData = m_dataProvider->getDataMessage<legion::messages::RGBControllerResponse,legion::messages::RGBControllerRequest>(LenovoLegionDaemon::DataProviderRGBController::dataType,[]{
        legion::messages::RGBControllerRequest request;
        request.set_request_flags(legion::messages::RGBControllerRequest::RequestFlags::RGBControllerRequest_RequestFlags_REQUEST_STATE_FOR_ALL_LEDS);

        return request;
    }());


    std::vector<LenovoLegionDaemon::RGBColor> colors;

    for(int i = 0; i < rgbControllerData.colors_size(); i++)
    {
        colors.push_back(rgbControllerData.colors(i));
    }

    return colors;
}

LenovoLegionDaemon::device_type RGBController::GetDeviceType() const
{
    return m_deviceType;
}

const LenovoLegionDaemon::Profiles& RGBController::GetProfiles() const
{
    return m_profiles;;
}

const LenovoLegionDaemon::Brightnesses& RGBController::GetBrightness() const
{
    return m_brightness;
}

void RGBController::SetBrightness(unsigned int brightness)
{
    m_brightness.active = brightness;
    m_pendingChanges.set(CHANGE_BRIGHTNESS);
}

const std::vector<LenovoLegionDaemon::led_group_effect> &RGBController::GetEffects() const
{
    return m_effects;
}

const LenovoLegionDaemon::led_group_effect &RGBController::GetEffect(unsigned int effectIdx) const
{
    return m_effects.at(effectIdx);
}

void RGBController::SetEfects(const std::vector<LenovoLegionDaemon::led_group_effect> &effects)
{
    m_effects = effects;
    m_pendingChanges.set(CHANGE_EFFECTS);
}

void RGBController::AddEffect(const LenovoLegionDaemon::led_group_effect &effect)
{
    m_effects.push_back(effect);
    m_pendingChanges.set(CHANGE_EFFECTS);
}

void RGBController::RemoveEffect(unsigned int effectIdx)
{
    m_effects.erase(m_effects.begin() + effectIdx);
    m_pendingChanges.set(CHANGE_EFFECTS);
}

void RGBController::ClearEffects()
{
    m_effects.clear();
    m_pendingChanges.set(CHANGE_EFFECTS);
}

void RGBController::ResetEffectsToDefault()
{
    m_pendingChanges.set(CHANGE_RESET_EFFECTS);
}

void RGBController::ApplyPendingChanges()
{
    sendRGBControllerData();
}

}
