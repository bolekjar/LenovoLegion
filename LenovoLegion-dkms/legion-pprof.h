// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#ifndef LEGION_PPROF_H_
#define LEGION_PPROF_H_

#include "legion-wmi-gamezone.h"
#include "legion-common.h"

#include <linux/device.h>


int  legion_platform_profile_init(struct device *parent);
void legion_platform_profile_exit(struct device *parent);

#endif /* LEGION_PPROF_H_ */
