// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#pragma once

#include "DataProviderRGBController.h"

#include <string>

#include <stdint.h>

namespace LenovoLegionDaemon {


class HIDDeviceDetector
{

public:

    HIDDeviceDetector(std::string name,
                      DataProviderRGBController::HIDDeviceDetectorFunction detector,
                      uint16_t vid,
                      uint16_t pid,
                      int interface,
                      int usage_page,
                      int usage)
    {
        DataProviderRGBController::registerControler(name,detector,vid,pid,interface,usage_page,usage);
    }
};

}
