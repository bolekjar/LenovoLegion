// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#ifndef LEGION_WMI_H
#define LEGION_WMI_H

#include "legion-common.h"

#include <linux/types.h>

int wmi_set_value(void *args, size_t size_args,const char *guid,u32 method_id);

int wmi_get_value_u32(void *args, size_t size_args,const char *guid,u32 method_id,u32 *value);
int wmi_get_value_buffer(void *args, size_t args_size,const char *guid,u32 method_id,u8 *res,size_t ressize);

#endif // LEGION_WMI_H
