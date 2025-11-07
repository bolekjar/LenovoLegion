// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "legion-ec.h"
#include "legion-ec-sysfs.h"
#include "legion-firmware-attributes-class.h"
#include "legion-common.h"

#include <linux/types.h>
#include <linux/delay.h>
#include <linux/component.h>
#include <linux/container_of.h>
#include <linux/device.h>
#include <linux/gfp_types.h>
#include <linux/idr.h>
#include <linux/device.h>
#include <linux/sysfs.h>

static DEFINE_IDA(legion_ec_ida);

#define LEGION_EC_FW_ATTR_BASE_PATH "legion-ec"



static ssize_t full_speed_show(struct kobject *kobj,struct kobj_attribute *kattr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(kobj_to_dev(kobj->parent));

    unsigned char value = 0;
    int err = 0;

    if (!priv)
        return -ENODEV;

    mutex_lock(&priv->mutex);
    err = ec_lenovo_read(EC_FULLFANSPEED,&value);
    mutex_unlock(&priv->mutex);

    if (err) {
        return err;
    }

    return sysfs_emit(buf, "%d\n", (value == EC_FANFULLSPEED_ON) ? true : false );
}

static ssize_t full_speed_store(struct kobject *kobj, struct kobj_attribute *kattr,
		const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(kobj_to_dev(kobj->parent));
	bool isOn = false;

	if (!priv)
		return -ENODEV;

	int err = kstrtobool(buf,&isOn);
    if (err) {
        return -EINVAL;
    }

    mutex_lock(&priv->mutex);
    err = ec_lenovo_write(EC_FULLFANSPEED, isOn ? EC_FANFULLSPEED_ON : EC_FANFULLSPEED_OFF);
    mutex_unlock(&priv->mutex);

	if (err) {
		return err;
	}

	return count;
}

static struct kobj_attribute ec_full_speed_attr = __ATTR_RW(full_speed);




static ssize_t lock_show(struct kobject *kobj,struct kobj_attribute *kattr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(kobj_to_dev(kobj->parent));

    unsigned char value = 0;
    int err = 0;

    if (!priv)
        return -ENODEV;

    mutex_lock(&priv->mutex);
    err = ec_lenovo_read(EC_LOCKFANCONTROLLER,&value);
    mutex_unlock(&priv->mutex);

    if (err) {
        return err;
    }

    return sysfs_emit(buf, "%d\n", (value == EC_LOCKFANCONTROLLER_ON) ? true : false );
}

static ssize_t lock_store(struct kobject *kobj, struct kobj_attribute *kattr,
		const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(kobj_to_dev(kobj->parent));
	bool isOn = false;

	if (!priv)
		return -ENODEV;

	int err = kstrtobool(buf,&isOn);
    if (err) {
        return -EINVAL;
    }

    mutex_lock(&priv->mutex);
    err = ec_lenovo_write(EC_LOCKFANCONTROLLER, isOn ? EC_LOCKFANCONTROLLER_ON : EC_LOCKFANCONTROLLER_OFF);
    mutex_unlock(&priv->mutex);

	if (err) {
		return err;
	}

	return count;
}

static struct kobj_attribute ec_lock_attr = __ATTR_RW(lock);





static ssize_t min_curve_show(struct kobject *kobj,struct kobj_attribute *kattr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(kobj_to_dev(kobj->parent));

    unsigned char value = 0;
    int err = 0;

    if (!priv)
        return -ENODEV;

    mutex_lock(&priv->mutex);
    err = ec_lenovo_read(EC_MINIFANCUVE_ON_COOL,&value);
    mutex_unlock(&priv->mutex);

    if (err) {
        return err;
    }

    return sysfs_emit(buf, "%d\n", (value == EC_MINIFANCUVE_ON_COOL_ON) ? true : false );
}

static ssize_t min_curve_store(struct kobject *kobj, struct kobj_attribute *kattr,
		const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(kobj_to_dev(kobj->parent));
	bool isOn = false;

	if (!priv)
		return -ENODEV;

	int err = kstrtobool(buf,&isOn);
    if (err) {
        return -EINVAL;
    }

    mutex_lock(&priv->mutex);
    err = ec_lenovo_write(EC_MINIFANCUVE_ON_COOL, isOn ? EC_MINIFANCUVE_ON_COOL_ON : EC_MINIFANCUVE_ON_COOL_OFF);
    mutex_unlock(&priv->mutex);

	if (err) {
		return err;
	}

	return count;
}

static struct kobj_attribute ec_min_curve_attr = __ATTR_RW(min_curve);

static ssize_t display_name_show(struct kobject *kobj,struct kobj_attribute *kattr, char *buf)
{
	return sysfs_emit(buf, "Fan control EC related settings\n" );
}

static struct kobj_attribute ec_display_name_attr = __ATTR_RO(display_name);

static struct attribute *legion_sysfs_fm_fan_ec_attributes[] = {
		&ec_full_speed_attr.attr,
		&ec_lock_attr.attr,
		&ec_min_curve_attr.attr,
		&ec_display_name_attr.attr,
		NULL
};

static const struct attribute_group legion_sysfs_fm_group[] = {
		{
			.name = "fan_control",
			.attrs = legion_sysfs_fm_fan_ec_attributes
		},
		{}
};

static int legion_ec_fw_attr_add(struct legion_data *priv)
{
	int err = 0;
	unsigned int i   = 0;
	priv->ec_ida_id = ida_alloc(&legion_ec_ida, GFP_KERNEL);
	if (priv->ec_ida_id < 0)
		return priv->ec_ida_id;

	priv->ec_fw_attr_dev = device_create(&legion_firmware_attributes_class, NULL,
					  MKDEV(0, 0), priv, "%s-%u",
					  LEGION_EC_FW_ATTR_BASE_PATH,
					  priv->ec_ida_id);
	if (IS_ERR(priv->ec_fw_attr_dev)) {
		err = PTR_ERR(priv->ec_fw_attr_dev);
		goto err_free_ida;
	}

	priv->ec_fw_attr_kset = kset_create_and_add("attributes", NULL,
						 &priv->ec_fw_attr_dev->kobj);
	if (!priv->ec_fw_attr_kset) {
		err = -ENOMEM;
		goto err_unregister_fw_attr_dev;
	}

	for (i = 0; i < ARRAY_SIZE(legion_sysfs_fm_group) - 1; i++) {
		err = sysfs_create_group(&priv->ec_fw_attr_kset->kobj,
				&legion_sysfs_fm_group[i]);
		if (err)
			goto err_remove_groups;
	}

	return 0;
err_remove_groups:
	while (i--)
		sysfs_remove_group(&priv->ec_fw_attr_kset->kobj,&legion_sysfs_fm_group[i]);
	kset_unregister(priv->ec_fw_attr_kset);
err_unregister_fw_attr_dev:
	device_unregister(priv->ec_fw_attr_dev);
err_free_ida:
	ida_free(&legion_ec_ida, priv->ec_ida_id);
	return err;
}

static void legion_wmi_ec_fw_attr_remove(struct legion_data *priv)
{
	/* Remove sysfs group first */
	for (unsigned int i = 0; i < ARRAY_SIZE(legion_sysfs_fm_group) - 1; i++)
		sysfs_remove_group(&priv->ec_fw_attr_kset->kobj,&legion_sysfs_fm_group[i]);

	/* Unregister kset before device since kset is child of device's kobj */
	kset_unregister(priv->ec_fw_attr_kset);

	/* Device unregister will clean up remaining kobject hierarchy */
	device_unregister(priv->ec_fw_attr_dev);

	ida_free(&legion_ec_ida, priv->ec_ida_id);
}


int  legion_ec_sysfs_init(struct device *parent) {
	struct legion_data* priv = dev_get_drvdata(parent);
	if (!priv)
		return -ENODEV;

	return legion_ec_fw_attr_add(priv);
}

void legion_ec_sysfs_exit(struct device *parent) {
	struct legion_data* priv = dev_get_drvdata(parent);

	if (!priv)
		return;

	legion_wmi_ec_fw_attr_remove(priv);
}
