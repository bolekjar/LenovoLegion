// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#pragma once

#include "LenovoDevices.h"
#include "LenovoUSBController.h"

#include "RGBController.h"


namespace LenovoLegionDaemon {

#define NA 0xFFFFFFFF

class RGBController_LenovoUSB : public RGBController
{
public:
    RGBController_LenovoUSB(LenovoUSBController* controller_ptr);
    ~RGBController_LenovoUSB();

    void        SetupZones();
    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        DeviceUpdateMode();
    void        DeviceSaveMode();

private:
    std::string ConvertBytesToHex(const std::vector<uint8_t> &input);
    std::string keyboardToString(LENOVO_KEYBOARD kb);
    std::string sizeToString(LENOVO_SIZE size);
    void        DumpControllerInformation();

    LENOVO_KEYBOARD keyboard_type;
    LENOVO_SIZE chasis_size;

    LenovoUSBController *controller;
};

}
