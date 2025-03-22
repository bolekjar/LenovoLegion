// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "legion-wmi-other.h"
#include "legion-hwmon.h"
#include "legion-common.h"
#include "legion-sysfs.h"
#include "legion-wmi.h"
#include "legion-wmi-gamezone.h"
#include "legion-wmi-driver.h"
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/acpi.h>
#include <linux/dmi.h>
#include "legion-pprof.h"


static const struct dmi_system_id supported_models[] = {
		{
		.ident = "N2CN",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "LENOVO"),
			DMI_MATCH(DMI_PRODUCT_NAME, "83DE"),
			DMI_MATCH(DMI_BIOS_VERSION, "N2CN"),
			}
		}
};

static int legion_pm_resume(struct device *dev)
{
	dev_dbg(dev, "PM  Resuming Lenovo Legion platform driver\n");

    return 0;
}

static SIMPLE_DEV_PM_OPS(legion_pm, NULL, legion_pm_resume);

static int legion_probe(struct platform_device *pdev)
{
    int err = 0;

    struct legion_data* 		 data    = NULL;

	dev_info(&pdev->dev, "Legion Pro 7 16IRX9H platform driver probing:\n");

	dev_info(
		&pdev->dev,
		"\tRead identifying information: DMI_SYS_VENDOR: %s; DMI_PRODUCT_NAME: %s; DMI_BIOS_VERSION:%s\n",
		dmi_get_system_info(DMI_SYS_VENDOR),
		dmi_get_system_info(DMI_PRODUCT_NAME),
		dmi_get_system_info(DMI_BIOS_VERSION)
		);

	if(dmi_first_match(supported_models) == NULL )
	{
		dev_info(&pdev->dev, "\tUnsupported model\n");
		goto err_model_mismtach;
	}
	else
	{
		dev_info(&pdev->dev, "\tSupported model\n");
	}

    data = devm_kzalloc(&pdev->dev, sizeof(struct legion_data), GFP_KERNEL);
    if (!data) {
    	dev_err(&pdev->dev, "\tFailed to allocate memory for legion_data\n");
        return -ENOMEM;
    }

    mutex_init(&data->mutex);


    platform_set_drvdata(pdev, data);


	err = legion_platform_profile_init(&pdev->dev);
	if (err) {
		dev_err(&pdev->dev, "\tFailed to create platform profile interface: %d\n", err);
		goto err_platform_profile;
	}
	dev_info(&pdev->dev,"\tPlatform profile interface was created\n");

    err = legion_wmi_driver_init(&pdev->dev);
    if (err != 0) {
    	dev_err(&pdev->dev, "\tFailed to create WMI interface: %d\n",err);
		goto err_wmi;
	}
    dev_info(&pdev->dev,"\tWMI interface was created\n");

    err = legion_sysfs_init(&pdev->dev);
    if (err) {
        dev_err(&pdev->dev, "\tFailed to create SysFs interface: %d\n",err);
        goto err_sysfs_init;
    }
	dev_info(&pdev->dev,"\tSysFs interface was created\n");

    err = legion_hwmon_init(&pdev->dev);
    if (err != 0) {
    	dev_err(&pdev->dev, "\tFailed to create HWMon interface: %d\n",err);
        goto err_hwmon_init;
    }
    dev_info(&pdev->dev,"\tHWMon interface was created\n");

    dev_info(&pdev->dev,"Legion Pro 7 16IRX9H platform driver was loaded\n");

    return err;

err_wmi:
	legion_platform_profile_exit(&pdev->dev);
err_platform_profile:
	legion_hwmon_exit(&pdev->dev);
err_hwmon_init:
    legion_sysfs_exit(&pdev->dev);
err_sysfs_init:
    devm_kfree(&pdev->dev, data);
err_model_mismtach:
	dev_err(&pdev->dev, "Legion Pro 7 16IRX9H platform not loaded for this device\n");
    return -ENODEV;
}

static void legion_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "Legion Pro 7 16IRX9H platform driver removing:\n");

    legion_hwmon_exit(((struct legion_data*)platform_get_drvdata(pdev))->hwmon_dev);
    dev_info(&pdev->dev, "\tHWMon interface was removed\n");

    legion_sysfs_exit(&pdev->dev);
    dev_info(&pdev->dev, "\tSysFs interface was removed\n");

    legion_wmi_driver_exit();
    dev_info(&pdev->dev, "\tWMI interface was removed\n");

    legion_platform_profile_exit(&pdev->dev);
    dev_info(&pdev->dev, "\tPlatform profile interface was removed\n");

    dev_info(&pdev->dev, "Legion Pro 7 16IRX9H platform driver was removed\n");
}

static int legion_resume(struct platform_device *pdev)
{
	dev_dbg(&pdev->dev, "Resuming Lenovo Legion platform driver\n");
    return 0;
}

static void legion_shutdown(struct platform_device *pdev)
{
	dev_dbg(&pdev->dev, "Shutting down Lenovo Legion platform driver\n");
}

static int legion_suspend(struct platform_device *pdev, pm_message_t state)
{
	dev_dbg(&pdev->dev, "Suspending Lenovo Legion platform driver\n");
    return 0;
}

/*
 * VPC2004 Virtual power connector
 */
static const struct acpi_device_id legion_device_ids[] =
{
    { "VPC2004", 0 },
    { "", 0 },
};

MODULE_DEVICE_TABLE(acpi, legion_device_ids);

static struct platform_driver legion_driver = {
    .probe      = legion_probe,
    .remove     = legion_remove,
    .shutdown   = legion_shutdown,
    .suspend    = legion_suspend,
    .resume     = legion_resume,
    .driver = {
        .name   = "lenovo-legion",
        .pm     = &legion_pm,
        .acpi_match_table = ACPI_PTR(legion_device_ids),
		.owner  = THIS_MODULE
    }
};

static int __init legion_init(void)
{
    int err;
    err = platform_driver_register(&legion_driver);
    if (err) {
        return err;
    }
    return 0;
}


static void __exit legion_exit(void)
{
    platform_driver_unregister(&legion_driver);
}

module_init(legion_init);
module_exit(legion_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jaroslav Bolek");
MODULE_DESCRIPTION("Lenovo Legion Pro 7 16IRX9H DKMS driver");
MODULE_VERSION("0.9.10");
