// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#pragma once

#include <string>

#include "RGBControllerInterface.h"

#include <stdint.h>


namespace  LenovoLegionDaemon {

#define NA 0xFFFFFFFF

/*-----------------------------------------------------*\
| Keyboard product IDs                                  |
\*-----------------------------------------------------*/
#define LEGION_7PROGEN9                         0xC998


enum LENOVO_KEYBOARD
{
    ISO,
    ANSI,
    JAPAN
};

enum LENOVO_SIZE
{
    SEVENTEEN,
    FIFTEEN,
    UNKNOWN
};

struct lenovo_led
{
    uint8_t led_num;
    std::string name;
};

struct lenovo_zone
{
    std::string name;
    zone_type type;
    unsigned char id;
    unsigned int height;
    unsigned int width;
    const unsigned int* matrix_map;
    const lenovo_led* leds;
    unsigned int start; //index to start reading the list of leds
    unsigned int end; //end index
};
}
