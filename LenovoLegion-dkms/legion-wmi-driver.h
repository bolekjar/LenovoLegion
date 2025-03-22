// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#ifndef LEGION_WMI_DRIVER_H_
#define LEGION_WMI_DRIVER_H_

#include "legion-common.h"

/*
 * Initialize WMI  driver
 */
int  legion_wmi_driver_init(struct device *parent);
void legion_wmi_driver_exit(void);


#endif /* LEGION_WMI_DRIVER_H_ */
