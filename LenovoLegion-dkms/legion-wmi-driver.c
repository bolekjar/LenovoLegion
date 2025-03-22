// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "legion-wmi-driver.h"
#include "legion-wmi.h"
#include "legion-wmi-gamezone.h"
#include <linux/wmi.h>
#include "legion-pprof.h"

enum LEGION_WMI_EVENT {
	LENOVO_GAMEZONE_SMART_FAN_MODE_EVENT    = 1,
	LENOVO_GAMEZONE_POWER_CHARGE_MODE_EVENT = 2,
};

struct legion_wmi_private {
	enum LEGION_WMI_EVENT event;
	struct device 		 *parent;
};

static struct legion_wmi_private lenovo_wmi_context_gamezone_smart_fan = {
	.event  = LENOVO_GAMEZONE_SMART_FAN_MODE_EVENT,
	.parent = NULL,
};

static struct legion_wmi_private lenovo_wmi_context_gamezone_power_charge = {
	.event  = LENOVO_GAMEZONE_POWER_CHARGE_MODE_EVENT,
	.parent = NULL,
};


static int legion_wmi_probe(struct wmi_device *wdev, const void *context)
{
	struct legion_wmi_private *wpriv = devm_kzalloc(&wdev->dev, sizeof(*wpriv), GFP_KERNEL);
	if (!wpriv)
		return -ENOMEM;

	*wpriv = *(const struct legion_wmi_private *)context;

	dev_set_drvdata(&wdev->dev, wpriv);

	return 0;
}

static void legion_wmi_notify(struct wmi_device *wdev, union acpi_object *data)
{
	struct legion_wmi_private* wpriv = dev_get_drvdata(&wdev->dev);


	switch (wpriv->event) {

		case LENOVO_GAMEZONE_SMART_FAN_MODE_EVENT:
		{
			goto send_notification;
		}
		break;

		case LENOVO_GAMEZONE_POWER_CHARGE_MODE_EVENT:
		{
			if(data->type == ACPI_TYPE_INTEGER) {
				goto send_notification_cycle;
			}

			dev_err(&wdev->dev, "Power charge mode error unknown data type: %d\n", data->type);
			goto no_send_notification;
		}
		break;
	default:
		dev_err(&wdev->dev,"Unknown Event: legion type: %d;  ACPI type: %d\n",wpriv->event, data->type);
		goto no_send_notification;
		break;
	}

send_notification:
	platform_profile_notify(((struct legion_data*)dev_get_drvdata(wpriv->parent))->pprof_device);
	return;
send_notification_cycle:
	platform_profile_cycle();
	return;
no_send_notification:
	return;
}

static const struct wmi_device_id legion_wmi_ids[] = {
	{ LENOVO_GAMEZONE_SMART_FAN_MODE_EVENT_GUID, &lenovo_wmi_context_gamezone_smart_fan },
	{ LENOVO_GAMEZONE_POWER_CHARGE_MODE_EVENT_GUID, &lenovo_wmi_context_gamezone_power_charge },
	{ }
};

MODULE_DEVICE_TABLE(wmi, legion_wmi_ids);

static struct wmi_driver legion_wmi_driver = {
	.driver = {
		.name = "lenovo-legion-wmi",
	},
	.id_table = legion_wmi_ids,
	.probe    = legion_wmi_probe,
	.notify   = legion_wmi_notify
};


int legion_wmi_driver_init(struct device *parent) {

	lenovo_wmi_context_gamezone_smart_fan.parent 	= parent;
	lenovo_wmi_context_gamezone_power_charge.parent = parent;

	return wmi_driver_register(&legion_wmi_driver);
}

void legion_wmi_driver_exit(void) {
	wmi_driver_unregister(&legion_wmi_driver);
}
