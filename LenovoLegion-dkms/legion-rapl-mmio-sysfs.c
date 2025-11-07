// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */


#include "legion-rapl-mmio-sysfs.h"
#include "legion-rapl-mmio.h"
#include "legion-common.h"


#include <linux/device.h>
#include <linux/export.h>
#include <linux/gfp_types.h>
#include <linux/idr.h>

#define LEGION_RAPL_MMIO_BASE_PATH "rapl-mmio"


static DEFINE_IDA(legion_machine_rapl_mmio_sysfs_ida);

const struct class legion_rapl_mmio_class = {
	.name = "legion-rapl-mmio"
};


static ssize_t pl1_current_value_show(struct device *dev,struct device_attribute *attr, char *buf) {
	struct legion_data *priv = dev_get_drvdata(dev);
	unsigned int pwr = 0;
	int  ret = 0;

	/* Critical: Prevent NULL pointer dereference and system freeze */
	if (!priv) {
		return -ENODEV;
	}

	ret = legion_pl1_power_read(&priv->rapl_mmio_private,&pwr);
	if(ret)
	{
		return ret;
	}

	return sysfs_emit(buf, "%d\n", pwr);
}

static ssize_t pl2_current_value_show(struct device *dev,struct device_attribute *attr, char *buf) {
	struct legion_data *priv = dev_get_drvdata(dev);
	unsigned int pwr = 0;
	int  ret = 0;

	/* Critical: Prevent NULL pointer dereference and system freeze */
	if (!priv) {
		return -ENODEV;
	}

	ret = legion_pl2_power_read(&priv->rapl_mmio_private,&pwr);
	if(ret)
	{
		return ret;
	}

	return sysfs_emit(buf, "%d\n", pwr);
}

static ssize_t is_locked_show(struct device *dev,struct device_attribute *attr, char *buf) {
	struct legion_data *priv = dev_get_drvdata(dev);
	bool isLocked = 0;
	int  ret = 0;

	/* Critical: Prevent NULL pointer dereference and system freeze */
	if (!priv) {
		return -ENODEV;
	}

	ret = legion_is_locked_power_read(&priv->rapl_mmio_private,&isLocked);
	if(ret)
	{
		return ret;
	}

	return sysfs_emit(buf, "%d\n", isLocked);
}


static ssize_t set_and_lock_store(struct device *dev,struct device_attribute *attr,const char *buf,size_t count) {
	struct legion_data *priv = dev_get_drvdata(dev);
	unsigned int pl1_mw, pl2_mw;
	int ret;


	/* Critical: Prevent NULL pointer dereference and system freeze */
	if (!priv) {
		return -ENODEV;
	}

    /* Parse "PL1_mW,PL2_mW" */
    if (sscanf(buf, "%u,%u", &pl1_mw, &pl2_mw) != 2) {
        return -EINVAL;
    }
    ret = legion_set_and_lock(&priv->rapl_mmio_private,pl1_mw,pl2_mw);
    if(ret)
    {
    	return ret;
    }
    return count;
}

static ssize_t pl4_current_value_show(struct device *dev,struct device_attribute *attr, char *buf) {
	struct legion_data *priv = dev_get_drvdata(dev);
	unsigned int pwr = 0;
	int  ret = 0;

	/* Critical: Prevent NULL pointer dereference and system freeze */
	if (!priv) {
		return -ENODEV;
	}

	ret = legion_pl4_power_read(&priv->rapl_mmio_private,&pwr);
	if(ret)
	{
		return ret;
	}

	return sysfs_emit(buf, "%d\n", pwr);
}

static ssize_t is_pl4_locked_show(struct device *dev,struct device_attribute *attr, char *buf) {
	struct legion_data *priv = dev_get_drvdata(dev);
	bool isLocked = 0;
	int  ret = 0;

	/* Critical: Prevent NULL pointer dereference and system freeze */
	if (!priv) {
		return -ENODEV;
	}

	ret = legion_pl4_lock_read(&priv->rapl_mmio_private,&isLocked);
	if(ret)
	{
		return ret;
	}

	return sysfs_emit(buf, "%d\n", isLocked);
}

static ssize_t set_pl4_and_lock_store(struct device *dev,struct device_attribute *attr,const char *buf,size_t count) {
	struct legion_data *priv = dev_get_drvdata(dev);
	unsigned int pl4_mw;
	int ret;

	/* Critical: Prevent NULL pointer dereference and system freeze */
	if (!priv) {
		return -ENODEV;
	}

    /* Parse "PL1_mW,PL2_mW" */
    if (sscanf(buf, "%u", &pl4_mw) != 1) {
        return -EINVAL;
    }

    ret = legion_pl4_set_and_lock(&priv->rapl_mmio_private,pl4_mw);
    if(ret)
    {
    	return ret;
    }
    return count;
}


static DEVICE_ATTR_RO(pl1_current_value);
static DEVICE_ATTR_RO(pl2_current_value);
static DEVICE_ATTR_RO(pl4_current_value);



static DEVICE_ATTR_RO(is_locked);
static DEVICE_ATTR_RO(is_pl4_locked);


static DEVICE_ATTR_WO(set_and_lock);
static DEVICE_ATTR_WO(set_pl4_and_lock);

static struct attribute *legion_rapl_mmio_sysfs_attributes[]  = {
	    &dev_attr_pl1_current_value.attr,
		&dev_attr_pl2_current_value.attr,
		&dev_attr_is_locked.attr,
		&dev_attr_set_and_lock.attr,
		&dev_attr_pl4_current_value.attr,
		&dev_attr_is_pl4_locked.attr,
		&dev_attr_set_pl4_and_lock.attr,
		NULL
};


static const struct attribute_group legion_rapl_mmio_sysfs_attributes_group = {
	    .attrs = legion_rapl_mmio_sysfs_attributes,
		{}
};

int  legion_rapl_mmio_sysfs_init(struct device *parent) {
	int ret = 0;
	struct legion_data* data = dev_get_drvdata(parent);

	if (!data)
	    return -ENODEV;

	ret = legion_rapl_mmio_init(&data->rapl_mmio_private);
    if (ret)
        return ret;

    ret = class_register(&legion_rapl_mmio_class);
    if (ret) {
    	goto err_remove_thermal_dev;
    }

    ret = data->rapl_mmio_sysfs_private.ida_id = ida_alloc(&legion_machine_rapl_mmio_sysfs_ida, GFP_KERNEL);
	if (data->rapl_mmio_sysfs_private.ida_id < 0)
		goto err_unregister_class;

	data->rapl_mmio_sysfs_private.dev = device_create(&legion_rapl_mmio_class, NULL,
					  MKDEV(0, 0), data, "%s-%u",
					  LEGION_RAPL_MMIO_BASE_PATH,
					  data->rapl_mmio_sysfs_private.ida_id);
	if (IS_ERR(data->rapl_mmio_sysfs_private.dev)) {
		ret = PTR_ERR(data->rapl_mmio_sysfs_private.dev);
		goto err_free_ida;
	}


	ret = device_add_group(data->rapl_mmio_sysfs_private.dev,&legion_rapl_mmio_sysfs_attributes_group);
    if (ret) {
    	goto err_unregister_dev;
    }

	return 0;

err_unregister_dev:
	device_unregister(data->rapl_mmio_sysfs_private.dev);
err_free_ida:
	ida_free(&legion_machine_rapl_mmio_sysfs_ida, data->rapl_mmio_sysfs_private.ida_id);
err_unregister_class:
	class_unregister(&legion_rapl_mmio_class);
err_remove_thermal_dev:
	legion_rapl_mmio_exit(&data->rapl_mmio_private);
	return ret;
}

void legion_rapl_mmio_sysfs_exit(struct device *parent) {
	struct legion_data* data = dev_get_drvdata(parent);

	if (!data)
		return;

	/* Remove sysfs group first */
	device_remove_group(data->rapl_mmio_sysfs_private.dev,&legion_rapl_mmio_sysfs_attributes_group);

	/* Device unregister will clean up remaining kobject hierarchy
	 * and wait for all outstanding sysfs operations to complete */
	device_unregister(data->rapl_mmio_sysfs_private.dev);

	legion_rapl_mmio_exit(&data->rapl_mmio_private);

	ida_free(&legion_machine_rapl_mmio_sysfs_ida, data->rapl_mmio_sysfs_private.ida_id);

	class_unregister(&legion_rapl_mmio_class);
}
