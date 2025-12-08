// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#ifndef LEGION_WMI_FM_H_
#define LEGION_WMI_FM_H_

#include "legion-wmi-gamezone.h"

#include <linux/notifier.h>

struct component_master_ops;
struct wmi_device;
struct kset;

enum fm_events_type {
	LEGION_WMI_FM_GET_FAN_MAX_SPEED = 0
};

struct legion_wmi_fm_priv {
	struct component_master_ops 	*ops;
	struct wmi_device 				*wdev;

	struct notifier_block 			hwmon_nb;    /* hwmon*/

	struct legion_wmi_ftable_list 	*ft_list; 	/* only valid after ft bind */
	int    					   fan_max_speed;   /* only valid after ft bind */

	//SysFs
	struct device *fw_attr_dev;
	struct kset   *fw_attr_kset;
	int ida_id;
};

int legion_wmi_fm_notifier_call(void *data,enum gamezone_events_type gamezone_events_type);
int devm_lenovo_wmi_fm_register_notifier(struct device *dev,struct notifier_block *nb);

int  legion_wmi_fm_driver_init(struct device *parent);
void legion_wmi_fm_driver_exit(void);

#endif /* LEGION_WMI_FM_H_ */
