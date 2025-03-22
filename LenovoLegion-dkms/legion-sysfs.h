// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#ifndef LEGION_SYSFS_H
#define LEGION_SYSFS_H

#include "legion-common.h"


int  legion_sysfs_init(struct device *parent);
void legion_sysfs_exit(struct device *parent);

#endif // LEGION_SYSFS_H
