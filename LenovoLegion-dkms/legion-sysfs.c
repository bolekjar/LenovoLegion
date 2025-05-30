// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "legion-sysfs.h"
#include "legion-wmi-gamezone.h"
#include "legion-ec.h"
#include "legion-wmi-other.h"

#include <linux/types.h>
#include <linux/delay.h>
#include <uapi/asm-generic/errno-base.h>


static ssize_t fan_mode_show(struct device *dev, struct device_attribute *attr,
                              char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    int err =0;
    u32 power_mode;

	mutex_lock(&priv->mutex);
	err = wmi_lenovo_gamezone_get_value(WMI_METHOD_ID_GETSMARTFANMODE,&power_mode);
	mutex_unlock(&priv->mutex);
	if(err)
	{
		dev_err(dev,"failed to read power mode\n");
		return err;
	}

	return sysfs_emit(buf, "%d\n", power_mode);
}

static ssize_t fan_mode_store(struct device *dev,
                               struct device_attribute *attr, const char *buf,
                               size_t count)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    int powermode = LEGION_WMI_POWERMODE_BALANCED;
    int err = 0;

    if (kstrtouint(buf, 0, &powermode))
        return -EINVAL;


    switch(powermode)
    {
    case LEGION_WMI_POWERMODE_QUIET:
    case LEGION_WMI_POWERMODE_BALANCED:
    case LEGION_WMI_POWERMODE_PERFORMANCE:
    case LEGION_WMI_POWERMODE_CUSTOM:
        break;
    default:
        dev_err(dev,"unexpected power mode value ignored: %d\n",powermode);
        return -EINVAL;
        break;
    }


	mutex_lock(&priv->mutex);
	err = wmi_lenovo_gamezone_powermode_store(powermode);
	mutex_unlock(&priv->mutex);
	if(err)
	{
		dev_err(dev,"failed to set power mode\n");
		return err;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 14, 0)
    platform_profile_notify(priv->pprof_device);
#else
    platform_profile_notify();
#endif

    return count;
}

static DEVICE_ATTR_RW(fan_mode);



static ssize_t fan_lock_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    unsigned char value = 0;
    int err = 0;

    mutex_lock(&priv->mutex);
    err = ec_lenovo_read(EC_LOCKFANCONTROLLER,&value);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to read lock fan controller\n");
        return err;
    }

    return sysfs_emit(buf, "%d\n", (value == EC_LOCKFANCONTROLLER_ON) ? true : false );
}

static ssize_t fan_lock_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    bool isLockFanControlOn = false;
    int err = 0;

    err = kstrtobool(buf,&isLockFanControlOn);
    if (err) {
    	dev_err(dev,"failed to parse lock fan controller value\n");
        return err;
    }

    mutex_lock(&priv->mutex);
    err = ec_lenovo_write(EC_LOCKFANCONTROLLER, isLockFanControlOn ? EC_LOCKFANCONTROLLER_ON : EC_LOCKFANCONTROLLER_OFF);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to set lock fan controller\n");
        return err;
    }

    return count;
}

static DEVICE_ATTR_RW(fan_lock);


static ssize_t fan_fullspeed_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    unsigned char value = 0;
    int err = 0;

    mutex_lock(&priv->mutex);
    err = ec_lenovo_read(EC_MAXIMUMFANSPEED,&value);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to read fan full speed\n");
        return err;
    }

    return sysfs_emit(buf, "%d\n", (value == EC_FANFULLSPEED_ON) ? true : false );
}

static ssize_t fan_fullspeed_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    bool isFullSpeedOn = false;
    int err = 0;

    err = kstrtobool(buf,&isFullSpeedOn);
    if (err) {
    	dev_err(dev,"failed to parse fan full speed value\n");
        return err;
    }

    mutex_lock(&priv->mutex);
    err = ec_lenovo_write(EC_MAXIMUMFANSPEED, isFullSpeedOn ? EC_FANFULLSPEED_ON : EC_FANFULLSPEED_OFF);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to set fan full speed\n");
        return err;
    }

    return count;
}

static DEVICE_ATTR_RW(fan_fullspeed);


static ssize_t cpu_st_plimit_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_get_value(OtherMethodFeature_CPU_SHORT_TERM_POWER_LIMIT,&value);;
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to read cpu short term power limit\n");
        return err;
    }

    return sysfs_emit(buf, "%d\n",  value);
}

static ssize_t cpu_st_plimit_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    err = kstrtouint(buf, 0, &value);
    if (err) {
    	dev_err(dev,"failed to parse cpu short term power limit value\n");
        return err;
    }

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_set_value(OtherMethodFeature_CPU_SHORT_TERM_POWER_LIMIT, value);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to set cpu short term power limit\n");
        return err;
    }

    return count;
}

static DEVICE_ATTR_RW(cpu_st_plimit);


static ssize_t cpu_lt_plimit_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_get_value(OtherMethodFeature_CPU_LONG_TERM_POWER_LIMIT,&value);;
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to read cpu long term power limit\n");
        return err;
    }

    return sysfs_emit(buf, "%d\n",  value);
}

static ssize_t cpu_lt_plimit_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    err = kstrtouint(buf, 0, &value);
    if (err) {
    	dev_err(dev,"failed to parse cpu long term power limit value\n");
        return err;
    }

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_set_value(OtherMethodFeature_CPU_LONG_TERM_POWER_LIMIT, value);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to set cpu long term power limit\n");
        return err;
    }

    return count;
}

static DEVICE_ATTR_RW(cpu_lt_plimit);

static ssize_t cpu_cl_plimit_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_get_value(OtherMethodFeature_CPU_CROSS_LOAD_POWER_LIMIT,&value);;
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to read cpu cross loading power limit\n");
        return err;
    }

    return sysfs_emit(buf, "%d\n",  value);
}

static ssize_t cpu_cl_plimit_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    err = kstrtouint(buf, 0, &value);
    if (err) {
    	dev_err(dev,"failed to parse cpu cross loading power limit value\n");
        return err;
    }

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_set_value(OtherMethodFeature_CPU_CROSS_LOAD_POWER_LIMIT, value);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to set cpu cross loading power limit\n");
        return err;
    }

    return count;
}

static DEVICE_ATTR_RW(cpu_cl_plimit);

static ssize_t cpu_temp_limit_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_get_value(OtherMethodFeature_CPU_TEMPERATURE_LIMIT,&value);;
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to read cpu temperature limit\n");
        return err;
    }

    return sysfs_emit(buf, "%d\n",  value);
}

static ssize_t cpu_temp_limit_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    err = kstrtouint(buf, 0, &value);
    if (err) {
    	dev_err(dev,"failed to parse cpu temperature limit value\n");
    	return err;
    }

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_set_value(OtherMethodFeature_CPU_TEMPERATURE_LIMIT, value);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to set cpu temperature limit\n");
        return err;
    }

    return count;
}

static DEVICE_ATTR_RW(cpu_temp_limit);




static ssize_t gpu_ctgp_limit_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_get_value(OtherMethodFeature_GPU_cTGP,&value);;
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to read gpu cTGP power limit\n");
        return err;
    }

    return sysfs_emit(buf, "%d\n",  value);
}

static ssize_t gpu_ctgp_limit_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    err = kstrtouint(buf, 0, &value);
    if (err) {
    	dev_err(dev,"failed to parse gpu cTGP power limit value\n");
        return err;
    }

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_set_value(OtherMethodFeature_GPU_cTGP, value);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to set gpu cTGP power limit\n");
    	return err;
    }

    return count;
}

static DEVICE_ATTR_RW(gpu_ctgp_limit);



static ssize_t gpu_temp_limit_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_get_value(OtherMethodFeature_GPU_TEMPERATURE_LIMIT,&value);;
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to read gpu temperature limit\n");
    	return err;
    }

    return sysfs_emit(buf, "%d\n",  value);
}

static ssize_t gpu_temp_limit_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    err = kstrtouint(buf, 0, &value);
    if (err) {
    	dev_err(dev,"failed to parse gpu temperature limi value\n");
    	return err;
    }

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_set_value(OtherMethodFeature_GPU_TEMPERATURE_LIMIT, value);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to set gpu temperature limit\n");
        return err;
    }

    return count;
}

static DEVICE_ATTR_RW(gpu_temp_limit);



static ssize_t gpu_pwr_boost_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_get_value(OtherMethodFeature_GPU_POWER_BOOST,&value);;
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to read gpu boost clock\n");
        return err;
    }

    return sysfs_emit(buf, "%d\n",  value);
}

static ssize_t gpu_pwr_boost_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    err = kstrtouint(buf, 0, &value);
    if (err) {
    	dev_err(dev,"failed to parse gpu boost clock value\n");
    	return err;
    }

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_set_value(OtherMethodFeature_GPU_POWER_BOOST, value);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to set gpu boost clock\n");
        return err;
    }

    return count;
}

static DEVICE_ATTR_RW(gpu_pwr_boost);


static ssize_t cpu_l1_tau_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_get_value(OtherMethodFeature_CPU_L1_TAU,&value);;
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to read cpu L1 tau\n");
        return err;
    }

    return sysfs_emit(buf, "%d\n",  value);
}

static ssize_t cpu_l1_tau_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    err = kstrtouint(buf, 0, &value);
    if (err) {
    	dev_err(dev,"failed to parse cpu L1 tau value\n");
        return err;
    }

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_set_value(OtherMethodFeature_CPU_L1_TAU, value);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to set cpu L1 tau\n");
        return err;
    }

    return count;
}

static DEVICE_ATTR_RW(cpu_l1_tau);


static ssize_t cpu_ac_tpwr_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_get_value(OtherMethodFeature_GPU_POWER_TARGET_ON_AC_OFFSET_FROM_BASELINE,&value);;
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to read cpu total ac\n");
        return err;
    }

    return sysfs_emit(buf, "%d\n",  value);
}

static ssize_t cpu_ac_tpwr_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    err = kstrtouint(buf, 0, &value);
    if (err) {
    	dev_err(dev,"failed to parse cpu total ac value\n");
        return err;
    }

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_set_value(OtherMethodFeature_GPU_POWER_TARGET_ON_AC_OFFSET_FROM_BASELINE, value);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to set cpu total ac\n");
        return err;
    }

    return count;
}

static DEVICE_ATTR_RW(cpu_ac_tpwr);


static ssize_t cpu_pwr_boost_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_get_value(OtherMethodFeature_CPU_POWER_POWER_BOOST,&value);;
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to read cpu power boost\n");
        return err;
    }

    return sysfs_emit(buf, "%d\n",  value);
}

static ssize_t cpu_pwr_boost_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    err = kstrtouint(buf, 0, &value);
    if (err) {
    	dev_err(dev,"failed to parse cpu power boost value\n");
        return err;
    }

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_other_set_value(OtherMethodFeature_CPU_POWER_POWER_BOOST, value);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to set cpu power boost\n");
        return err;
    }

    return count;
}

static DEVICE_ATTR_RW(cpu_pwr_boost);


static ssize_t pwr_chargemode_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    struct legion_data *priv = dev_get_drvdata(dev);
    u32 value = 0;
    int err = 0;

    mutex_lock(&priv->mutex);
    err = wmi_lenovo_gamezone_get_value(WMI_METHOD_ID_GETPOWERCHARGEMODE,&value);
    mutex_unlock(&priv->mutex);
    if (err) {
    	dev_err(dev,"failed to read power charge mode\n");
        return err;
    }

    return sysfs_emit(buf, "%d\n",  value);
}


static DEVICE_ATTR_RO(pwr_chargemode);


static struct attribute *legion_sysfs_attributes[] = {

    &dev_attr_fan_mode.attr,
    &dev_attr_fan_lock.attr,
    &dev_attr_fan_fullspeed.attr,

	&dev_attr_cpu_st_plimit.attr,
    &dev_attr_cpu_lt_plimit.attr,
    &dev_attr_cpu_cl_plimit.attr,
    &dev_attr_cpu_temp_limit.attr,
	&dev_attr_cpu_l1_tau.attr,
	&dev_attr_cpu_ac_tpwr.attr,
	&dev_attr_cpu_pwr_boost.attr,

	&dev_attr_gpu_ctgp_limit.attr,
    &dev_attr_gpu_temp_limit.attr,
    &dev_attr_gpu_pwr_boost.attr,

	&dev_attr_pwr_chargemode.attr,
    NULL
};

static const struct attribute_group legion_sysfs_attribute_group = {
    .attrs = legion_sysfs_attributes
};

int  legion_sysfs_init(struct device *parent) {
    return device_add_group(parent,&legion_sysfs_attribute_group);
}


void legion_sysfs_exit(struct device *parent) {
    device_remove_group(parent,&legion_sysfs_attribute_group);
}
