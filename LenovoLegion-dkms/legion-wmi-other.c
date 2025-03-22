// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "legion-wmi-other.h"
#include "legion-wmi.h"

#include <linux/module.h>

struct bios_args {
    u32 feature_id;
    u32 value;
} __packed;


int wmi_lenovo_other_set_value(enum OtherMethodFeature feature_id,u32 value) {
    struct bios_args args = {
        .feature_id = feature_id,
        .value = value,
    };

    return wmi_set_value(&args,sizeof(args),LEGION_WMI_LENOVO_OTHER_METHOD_GUID,WMI_METHOD_ID_SET_FEATURE_VALUE);
}

int wmi_lenovo_other_get_value(enum OtherMethodFeature feature_id,u32 *value) {

    struct  bios_args args = {
        .feature_id = feature_id,
        .value = 0,
    };

    return wmi_get_value_u32(&args,sizeof(args),LEGION_WMI_LENOVO_OTHER_METHOD_GUID,WMI_METHOD_ID_GET_FEATURE_VALUE,value);
}
