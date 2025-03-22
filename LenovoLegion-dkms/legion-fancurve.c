// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "legion-fancurve.h"
#include "legion-wmi.h"
#include "legion-common.h"

#include <linux/device.h>
#include <uapi/asm-generic/errno.h>
#include <linux/hwmon-sysfs.h>

#define WMI_GUID_LENOVO_FAN_METHOD "92549549-4bde-4f06-ac04-ce8bf898dbaa"

// fan table in custom mode
#define WMI_METHOD_ID_FAN_GET_TABLE 5
#define WMI_METHOD_ID_FAN_SET_TABLE 6

struct WMIFanTable {
    u8 FSTM; //FSMD
    u8 FSID;
    u32 FSTL; //FSST
    u16 FSS0;
    u16 FSS1;
    u16 FSS2;
    u16 FSS3;
    u16 FSS4;
    u16 FSS5;
    u16 FSS6;
    u16 FSS7;
    u16 FSS8;
    u16 FSS9;
} __packed;

struct WMIFanTableRead {
    u32 FSFL;
    u32 FSS0;
    u32 FSS1;
    u32 FSS2;
    u32 FSS3;
    u32 FSS4;
    u32 FSS5;
    u32 FSS6;
    u32 FSS7;
    u32 FSS8;
    u32 FSS9;
    u32 FSSA;
} __packed;

static ssize_t wmi_read_fancurve_custom(struct fancurve *fancurve)
{
    u8 buffer[88] = {0};
    int err;


    err = wmi_get_value_buffer(NULL,0,WMI_GUID_LENOVO_FAN_METHOD, WMI_METHOD_ID_FAN_GET_TABLE, buffer, sizeof(buffer));
    if (!err) {
        struct WMIFanTableRead *fantable =(struct WMIFanTableRead *)&buffer[0];
        fancurve->points[0] = fantable->FSS0;
        fancurve->points[1] = fantable->FSS1;
        fancurve->points[2] = fantable->FSS2;
        fancurve->points[3] = fantable->FSS3;
        fancurve->points[4] = fantable->FSS4;
        fancurve->points[5] = fantable->FSS5;
        fancurve->points[6] = fantable->FSS6;
        fancurve->points[7] = fantable->FSS7;
        fancurve->points[8] = fantable->FSS8;
        fancurve->points[9] = fantable->FSS9;
    }

    return err;
}


struct bios_args {
    u32 value;
} __packed;



static ssize_t wmi_write_fancurve_custom(struct fancurve *fancurve)
{
    u8 buffer[0x20] = {0};


	memset(buffer, 0, sizeof(buffer));

	buffer[0x06] = fancurve->points[0];
	buffer[0x08] = fancurve->points[1];
	buffer[0x0A] = fancurve->points[2];
	buffer[0x0C] = fancurve->points[3];
	buffer[0x0E] = fancurve->points[4];
	buffer[0x10] = fancurve->points[5];
	buffer[0x12] = fancurve->points[6];
	buffer[0x14] = fancurve->points[7];
	buffer[0x16] = fancurve->points[8];
	buffer[0x18] = fancurve->points[9];

	return wmi_set_value(buffer,sizeof(buffer),WMI_GUID_LENOVO_FAN_METHOD,WMI_METHOD_ID_FAN_SET_TABLE);
}



static ssize_t fancurve_show(struct device *dev,struct device_attribute *devattr, char *buf)
{
	struct legion_data *priv = dev_get_drvdata(dev);
	struct fancurve fancurve;


	{
		guard(mutex)(&priv->mutex);

		if(wmi_read_fancurve_custom(&fancurve) != 0)
		{
			dev_err(dev,"failed to read fancurve\n");
			goto error;
		}
	}

	return sprintf(buf, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu\n",fancurve.points[0],
																				fancurve.points[1],
																				fancurve.points[2],
																				fancurve.points[3],
																				fancurve.points[4],
																				fancurve.points[5],
																				fancurve.points[6],
																				fancurve.points[7],
																				fancurve.points[8],
																				fancurve.points[9]);
error:
	return -EIO;
}

static ssize_t fancurve_store(struct device *dev,
			       struct device_attribute *devattr,
			       const char *buf, size_t count)
{

	struct legion_data *priv = dev_get_drvdata(dev);
	struct fancurve fancurve;


	guard(mutex)(&priv->mutex);


	if(sscanf(buf,"%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu",&fancurve.points[0],
																   &fancurve.points[1],
																   &fancurve.points[2],
																   &fancurve.points[3],
																   &fancurve.points[4],
																   &fancurve.points[5],
																   &fancurve.points[6],
																   &fancurve.points[7],
																   &fancurve.points[8],
																   &fancurve.points[9]) != MAX_FANCURVE_SIZE)
	{
		dev_err(dev,"parsing hwmon store failed: %s.\n",buf);
		goto error;
	}

	if(wmi_write_fancurve_custom(&fancurve) != 0)
	{
		dev_err(dev,"write to fancurve  failed: %s.\n",buf);
		goto error;
	}

	return count;
error:
	return count;
}


static SENSOR_DEVICE_ATTR_RW(pwm1_auto_points_pwm, fancurve,0);


static struct attribute *fancurve_hwmon_attributes[] = {
	&sensor_dev_attr_pwm1_auto_points_pwm.dev_attr.attr,
	NULL
};

static const struct attribute_group legion_hwmon_fancurve_group = {
	.attrs = fancurve_hwmon_attributes
};

const struct attribute_group* get_fancurve_attaribute_group(void) {
	return &legion_hwmon_fancurve_group;
}
