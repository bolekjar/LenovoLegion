// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#ifndef LEGION_COMMON_H
#define LEGION_COMMON_H

#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/platform_profile.h>

struct legion_data {
    struct device						  *hwmon_dev;
    struct mutex   						   mutex;
    struct device 						  *pprof_device;
};

#define MAXIMUM_FAN_SPEED 5500

#endif // LEGION_COMMON_H
