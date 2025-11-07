// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#ifndef LEGION_EC_SYSFS_H_
#define LEGION_EC_SYSFS_H_


struct device;
int  legion_ec_sysfs_init(struct device *parent);
void legion_ec_sysfs_exit(struct device *parent);

#endif /* LEGION_EC_SYSFS_H_ */
