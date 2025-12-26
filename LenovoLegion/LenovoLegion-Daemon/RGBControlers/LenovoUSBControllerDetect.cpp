// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "LenovoRGBController.h"
#include "RGBControllerDetector.h"

namespace LenovoLegionDaemon {

/*-----------------------------------------------------*\
| vendor ID                                            |
\*-----------------------------------------------------*/
#define ITE_VID                                 0x048D


/*-----------------------------------------------------*\
| Product ID                                            |
\*-----------------------------------------------------*/
#define LENOVO_PID                              0xC900
#define LENOVO_PID_MASK                         0xFF00


RGBController * DetectLenovoLegionUSBControllersGen9(const hid_device_info& info, const std::string& name)
{
    hid_device* dev = hid_open_path(info.path);

    if(dev)
    {
        LenovoRGBController*     rgb_controller  = new LenovoRGBController(new LenovoUSBController(dev, info.path, info.product_id));
        rgb_controller->GetName()                     = name;

        return rgb_controller;
    }

    return nullptr;
}

REGISTER_HID_DETECTOR("Lenovo Laptop",   DetectLenovoLegionUSBControllersGen9, ITE_VID, LENOVO_PID, LENOVO_PID_MASK);

}
