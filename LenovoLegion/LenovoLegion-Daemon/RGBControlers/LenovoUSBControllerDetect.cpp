// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "LenovoDevices.h"

#include "RGBController_LenovoGen9.h"
#include "RGBControllerDetector.h"




namespace LenovoLegionDaemon {

/*-----------------------------------------------------*\
| vendor IDs                                            |
\*-----------------------------------------------------*/
#define ITE_VID                                 0x048D

/*-----------------------------------------------------*\
| Interface, Usage, and Usage Page                      |
\*-----------------------------------------------------*/
enum
{
    LENOVO_PAGE   = 0xFF89,
    LENOVO_USAGE  = 0x07
};


RGBControllerInterface * DetectLenovoLegionUSBControllersGen9(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        RGBController_LenovoGen9*     rgb_controller  = new RGBController_LenovoGen9(new LenovoGen9USBController(dev, info->path, info->product_id));
        rgb_controller->GetName()                     = name;

        return rgb_controller;
    }

    return nullptr;
}

REGISTER_HID_DETECTOR_PU("Lenovo Legion 7 Pro Gen 9",   DetectLenovoLegionUSBControllersGen9, ITE_VID, LEGION_7PROGEN9,   LENOVO_PAGE, LENOVO_USAGE);

}
