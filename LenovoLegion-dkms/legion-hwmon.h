// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#ifndef LEGION_HWMON_H
#define LEGION_HWMON_H
#include "legion-common.h"

#include <linux/types.h>

ssize_t legion_hwmon_init(struct device *parent);

void legion_hwmon_exit(struct device *parent);


#endif // LEGION_HWMON_H
