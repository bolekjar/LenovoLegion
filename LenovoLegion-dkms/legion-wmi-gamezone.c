// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "legion-wmi-gamezone.h"
#include "legion-wmi.h"

#include <linux/acpi.h>
#include <linux/delay.h>

struct bios_args {
    u32 value;
} __packed;

static int wait_for_gamezone_powermode_changed(const int target_value,int timeoutInMs)
{
	for (int i = 0; i < timeoutInMs / 50; i++) {
		int power_mode = 0;
		int err = 0;

		err = wmi_lenovo_gamezone_get_value(WMI_METHOD_ID_GETSMARTFANMODE,&power_mode);
		if(err) {
			return err;
		}

		if (target_value == power_mode)
			return 0;

		msleep(50);
	}

    return -ETIMEDOUT;
}


int wmi_lenovo_gamezone_set_value(u32 method_id,u32 value) {

	struct  bios_args args = {
        .value = value
    };

    return wmi_set_value(&args,sizeof(args),LEGION_WMI_GAMEZONE_GUID,method_id);
}

int wmi_lenovo_gamezone_get_value(u32 method_id,u32 *value) {
	return wmi_get_value_u32(NULL,0,LEGION_WMI_GAMEZONE_GUID,method_id,value);
}


int wmi_lenovo_gamezone_powermode_store(enum legion_wmi_powermode power_mode) {
	int err = 0;

	err = wmi_lenovo_gamezone_set_value(WMI_METHOD_ID_SETSMARTFANMODE,power_mode);
	if (err)
		return err;

	return wait_for_gamezone_powermode_changed(power_mode, 500);
}
