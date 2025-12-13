// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "DataProviderRGBController.h"
#include "RGBController.h"

#include "Core/LoggerHolder.h"
#include "../LenovoLegion-PrepareBuild/RGBController.pb.h"


namespace LenovoLegionDaemon {

    DataProviderRGBController::DataProviderRGBController(QObject* parent)  :DataProvider(parent,dataType) {}

    DataProviderRGBController::~DataProviderRGBController()
    {
    }

    QByteArray DataProviderRGBController::serializeAndGetData() const
    {
        legion::messages::RGBController rgbController;
        QByteArray byteArray;

        LOG_D(__PRETTY_FUNCTION__);

        if(m_rgbController == nullptr)
        {
            LOG_D(QString(__PRETTY_FUNCTION__) + " - RGB Controller not available");
            rgbController.Clear();
        }
        else
        {
            // Set device type
            rgbController.set_device_type(static_cast<legion::messages::RGBController::DeviceType>(m_rgbController->GetDeviceType()));

            // Set active mode
            rgbController.set_active_mode(m_rgbController->GetMode());

            // Set profiles
            rgbController.set_profiles(m_rgbController->GetProfiles());
            rgbController.set_active_profile(m_rgbController->GetActiveProfile());


            // Serialize LEDs
            const auto& leds = m_rgbController->GetLEDs();
            for(const auto& led : leds)
            {
                auto* pbLed = rgbController.add_leds();
                pbLed->set_name(led.name);
                pbLed->set_value(led.value);
            }

            // Serialize Zones
            const auto& zones = m_rgbController->GetZones();
            for(const auto& zone : zones)
            {
                auto* pbZone = rgbController.add_zones();
                pbZone->set_name(zone.name);
                pbZone->set_type(static_cast<legion::messages::RGBController::ZoneType>(zone.type));
                pbZone->set_start_idx(zone.start_idx);
                pbZone->set_leds_count(zone.leds_count);
                pbZone->set_leds_min(zone.leds_min);
                pbZone->set_leds_max(zone.leds_max);
                pbZone->set_flags(zone.flags);

                // Serialize matrix map if present
                if(zone.matrix_map != nullptr)
                {
                    auto* pbMatrixMap = pbZone->mutable_matrix_map();
                    pbMatrixMap->set_height(zone.matrix_map->height);
                    pbMatrixMap->set_width(zone.matrix_map->width);
                    for(unsigned int i = 0; i < zone.matrix_map->height * zone.matrix_map->width; i++)
                    {
                        pbMatrixMap->add_map(zone.matrix_map->map[i]);
                    }
                }

                // Serialize segments
                for(const auto& segment : zone.segments)
                {
                    auto* pbSegment = pbZone->add_segments();
                    pbSegment->set_name(segment.name);
                    pbSegment->set_type(static_cast<legion::messages::RGBController::ZoneType>(segment.type));
                    pbSegment->set_start_idx(segment.start_idx);
                    pbSegment->set_leds_count(segment.leds_count);
                }
            }

            // Serialize Modes
            const auto& modes = m_rgbController->GetModes();
            for(const auto& mode : modes)
            {
                auto* pbMode = rgbController.add_modes();
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

                // Serialize mode colors
                for(const auto& color : mode.colors)
                {
                    pbMode->add_colors(color);
                }
            }

            // Serialize Colors
            const auto& colors = m_rgbController->GetColors();
            for(const auto& color : colors)
            {
                rgbController.add_colors(color);
            }
        }

        byteArray.resize(rgbController.ByteSizeLong());
        if(!rgbController.SerializeToArray(byteArray.data(), byteArray.size()))
        {
            THROW_EXCEPTION(exception_T, ERROR_CODES::SERIALIZE_ERROR, "Serialize of data message error !");
        }

        return byteArray;
    }

    QByteArray DataProviderRGBController::deserializeAndSetData(const QByteArray& data)
    {
        legion::messages::RGBController rgbController;

        LOG_D(__PRETTY_FUNCTION__);

        if(!rgbController.ParseFromArray(data.data(), data.size()))
        {
            THROW_EXCEPTION(exception_T, DataProvider::ERROR_CODES::INVALID_DATA, "Parse of data message error !");
        }

        if(m_rgbController == nullptr)
        {
            LOG_D(QString(__PRETTY_FUNCTION__) + " - RGB Controller not available");
            return {};
        }

        // Apply mode if changed
        if(rgbController.has_active_mode())
        {
            int currentMode = m_rgbController->GetMode();
            if(currentMode != rgbController.active_mode())
            {
                LOG_D(QString("Changing mode from %1 to %2").arg(currentMode).arg(rgbController.active_mode()));

                m_rgbController->SetMode(rgbController.active_mode());
                m_rgbController->DeviceUpdateMode();
            }
        }

        // Apply profile if changed
        if(rgbController.has_active_profile())
        {
            size_t currentProfile = m_rgbController->GetActiveProfile();
            if(currentProfile != rgbController.active_profile())
            {
                LOG_D(QString("Changing profile from %1 to %2").arg(currentProfile).arg(rgbController.active_profile()));

                m_rgbController->SetProfile(rgbController.active_profile());
                m_rgbController->DeviceUpdateProfile();
            }
        }

        // Apply modes settings if provided
        if(rgbController.modes_size() > 0)
        {
            LOG_D(QString("Applying %1 modes").arg(rgbController.modes_size()));

            std::vector<mode> modes;
            for(int i = 0; i < rgbController.modes_size(); i++)
            {
                const auto& pbMode = rgbController.modes(i);
                mode m;
                m.name = pbMode.name();
                m.value = pbMode.value();
                m.flags = pbMode.flags();
                m.speed_min = pbMode.speed_min();
                m.speed_max = pbMode.speed_max();
                m.brightness_min = pbMode.brightness_min();
                m.brightness_max = pbMode.brightness_max();
                m.colors_min = pbMode.colors_min();
                m.colors_max = pbMode.colors_max();
                m.speed = pbMode.speed();
                m.brightness = pbMode.brightness();
                m.direction = pbMode.direction();
                m.color_mode = pbMode.color_mode();

                // Copy mode colors
                for(int j = 0; j < pbMode.colors_size(); j++)
                {
                    m.colors.push_back(pbMode.colors(j));
                }

                modes.push_back(m);
            }
            m_rgbController->SetModes(modes);

            m_rgbController->DeviceUpdateMode();
        }

        // Apply LEDs settings if provided
        if(rgbController.leds_size() > 0)
        {
            LOG_D(QString("Applying %1 LEDs").arg(rgbController.leds_size()));

            std::vector<led> leds;
            for(int i = 0; i < rgbController.leds_size(); i++)
            {
                const auto& pbLed = rgbController.leds(i);
                led l;
                l.name = pbLed.name();
                l.value = pbLed.value();
                leds.push_back(l);
            }
            m_rgbController->SetLEDs(leds);

            m_rgbController->DeviceUpdateLEDs();
        }

        // Apply individual LED colors if provided
        if(rgbController.colors_size() > 0)
        {
            LOG_D(QString("Applying %1 individual LED colors").arg(rgbController.colors_size()));

            const auto& colors = m_rgbController->GetColors();
            for(int i = 0; i < rgbController.colors_size() && i < static_cast<int>(colors.size()); i++)
            {
                m_rgbController->SetLED(i, rgbController.colors(i));
            }

            m_rgbController->DeviceUpdateLEDs();
        }

        return {};
    }

    void DataProviderRGBController::init()
    {
        hid_device_info *current_hid_device = hid_enumerate(0, 0);;

        LOG_D(__PRETTY_FUNCTION__);

        clean();

        while (current_hid_device) {

            /*-----------------------------------------------------------------------------*\
            | Loop through all available detectors.  If all required information matches,   |
            | run the detector                                                              |
            \*-----------------------------------------------------------------------------*/
            for (auto &detector : hidDeviceDetectorsBlocks)
            {

                if (detector.compare(current_hid_device)) {
                    m_rgbController.reset(detector.function(current_hid_device,detector.name));

                    if(m_rgbController != nullptr)
                    {
                        LOG_D(QString("RGB Controller Detected: ").append(detector.name.c_str()));
                        break;
                    }
                }
            }

            if(m_rgbController != nullptr)
            {
                break;
            }

            current_hid_device = current_hid_device->next;
        }
    }

    void DataProviderRGBController::clean()
    {
        m_rgbController.reset();
    }

    void DataProviderRGBController::registerControler(std::string name, HIDDeviceDetectorFunction det, uint16_t vid, uint16_t pid, int interface, int usage_page, int usage)
    {
        HIDDeviceDetectorBlock block;

        block.name         = name;
        block.vid          = vid;
        block.pid          = pid;
        block.interface    = interface;
        block.usage_page   = usage_page;
        block.usage        = usage;
        block.function     = det;

        hidDeviceDetectorsBlocks.push_back(block);
    }

    std::vector<DataProviderRGBController::HIDDeviceDetectorBlock> DataProviderRGBController::hidDeviceDetectorsBlocks{};


    bool DataProviderRGBController::BasicHIDBlock::compare(hid_device_info *info)
    {
        return ((vid == info->vendor_id) && (pid == info->product_id)
#ifdef USE_HID_USAGE
                && ((usage_page == HID_USAGE_PAGE_ANY) || (usage_page == info->usage_page))
                && ((usage == HID_USAGE_ANY) || (usage == info->usage))
                && ((interface == HID_INTERFACE_ANY) || (interface == info->interface_number))
#else
                && ((interface == HID_INTERFACE_ANY) || (interface == info->interface_number))
#endif
                );
    }

}

