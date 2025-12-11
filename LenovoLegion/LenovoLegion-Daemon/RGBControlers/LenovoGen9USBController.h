// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#pragma once

#include "RGBControllerInterface.h"

#include <string>
#include <vector>

#include <hidapi.h>
#include <stdint.h>

namespace LenovoLegionDaemon {

#ifndef HID_MAX_STR
#define HID_MAX_STR                255
#endif

#define PACKET_SIZE                960
#define REPORT_ID                 0x07
#define DIRECT_MODE               0xA1
#define SWITCH_PROFILE            0xC8
#define GET_ACTIVE_PROFILE        0xCA
#define SAVE_PROFILE              0xCB
#define GET_PROFILE               0xCC
#define GET_BRIGHTNESS            0xCD
#define SET_BRIGHTNESS            0xCE
#define SET_DIRECT_MODE           0xD0
#define GET_DIRECT_MODE_PROFILE   0xD1

class LenovoGen9USBController
{

public:

    struct led_group
    {
        uint8_t               mode;
        uint8_t               speed;
        uint8_t               spin;
        uint8_t               direction;
        uint8_t               color_mode;
        std::vector<RGBColor> colors;
        std::vector<uint16_t> leds;
    };

public:
    /*--------------*\
    |ctor(s) and dtor|
    \*--------------*/
    LenovoGen9USBController(hid_device* dev_handle, const char* path, uint16_t in_pid);
    ~LenovoGen9USBController();

    /*--------------*\
    |device functions|
    \*--------------*/
    void setLedsByGroup(uint8_t profile_id, std::vector<led_group> led_groups);
    void setLedsDirect(std::vector<led> &leds, std::vector<RGBColor> &colors);
    void setLedsAllOff(uint8_t profile_id);
    void setLedsDirectOn(uint8_t profile_id);
    void setLedsDirectOff(uint8_t profile_id);
    void setBrightness(uint8_t brightness);

    void switchProfileTo(uint8_t profile_id);

    uint16_t                getPid();
    std::string             getName();
    std::string             getLocation();
    uint8_t                 getCurrentProfileId();
    uint8_t                 getCurrentBrightness();
    uint8_t                 getKeyboardId();
    std::vector<led_group>  getProfileSettings(uint8_t profile_id);
    std::string             getSerialString();
    std::string             getFirmwareVersion();

    private:
    /*--------------*\
    |data members    |
    \*--------------*/
    std::string name;
    hid_device *dev;
    std::string location;
    uint16_t    pid;
    std::string version;

    /*--------------*\
    |device functions|
    \*--------------*/
    void sendFeatureReport(uint8_t packet[], size_t packet_size);
    std::vector<uint8_t> getFeatureReport(uint8_t packet[], size_t packet_size);
    std::string ConvertBytesToHex(uint8_t packet[], size_t packet_size);
    std::string ConvertBytesToHex(const std::vector<uint8_t> &input);
};

}
