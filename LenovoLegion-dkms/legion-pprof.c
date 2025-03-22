// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "legion-wmi-gamezone.h"

#include <linux/platform_device.h>
#include <linux/device.h>
#include "legion-pprof.h"

static int legion_platform_profile_get(struct device *dev,enum platform_profile_option *profile)
{
	struct legion_data *data = dev_get_drvdata(dev);

    u32 power_mode = LEGION_WMI_POWERMODE_BALANCED;

    {
    	int err   = 0;
		guard(mutex)(&data->mutex);
		err = wmi_lenovo_gamezone_get_value(WMI_METHOD_ID_GETSMARTFANMODE,&power_mode);

		if(err) {
			return err;
		}

    }

	switch (power_mode) {
		case LEGION_WMI_POWERMODE_BALANCED:
			*profile = PLATFORM_PROFILE_BALANCED;
			break;
		case LEGION_WMI_POWERMODE_PERFORMANCE:
			*profile = PLATFORM_PROFILE_PERFORMANCE;
			break;
		case LEGION_WMI_POWERMODE_QUIET:
			*profile = PLATFORM_PROFILE_QUIET;
			break;
		case LEGION_WMI_POWERMODE_CUSTOM:
			*profile = PLATFORM_PROFILE_BALANCED_PERFORMANCE;
			break;
		default:
			return -EINVAL;
	}
	return 0;
}

static int legion_platform_profile_set(struct device *dev,enum platform_profile_option profile)
{
	int power_mode = 0;
	int power_charge_mode = 0;
	int err = 0;
	struct legion_data *data = dev_get_drvdata(dev);

	guard(mutex)(&data->mutex);

    err = wmi_lenovo_gamezone_get_value(WMI_METHOD_ID_GETPOWERCHARGEMODE,&power_charge_mode);
    if (err) {
        return err;
    }


    if(power_charge_mode == LEGION_WMI_POWER_CHARGE_MODE_BATERY) {
    	switch (profile) {
		case PLATFORM_PROFILE_QUIET:
		case PLATFORM_PROFILE_BALANCED_PERFORMANCE:
		case PLATFORM_PROFILE_PERFORMANCE:
			power_mode = LEGION_WMI_POWERMODE_QUIET;
			break;
		case PLATFORM_PROFILE_BALANCED:
			power_mode = LEGION_WMI_POWERMODE_BALANCED;
			break;
		default:
			return -EOPNOTSUPP;
		}
	} else if (power_charge_mode == LEGION_WMI_POWER_CHARGE_MODE_AC) {
    	switch (profile) {
    	case PLATFORM_PROFILE_BALANCED:
    		power_mode = LEGION_WMI_POWERMODE_BALANCED;
    		break;
    	case PLATFORM_PROFILE_PERFORMANCE:
    		power_mode = LEGION_WMI_POWERMODE_PERFORMANCE;
    		break;
    	case PLATFORM_PROFILE_QUIET:
    		power_mode = LEGION_WMI_POWERMODE_QUIET;
    		break;
    	case PLATFORM_PROFILE_BALANCED_PERFORMANCE:
    		power_mode = LEGION_WMI_POWERMODE_CUSTOM;
    		break;
    	default:
    		return -EOPNOTSUPP;
    	}
    }
	else {
		return -EOPNOTSUPP;
	}

	return wmi_lenovo_gamezone_powermode_store(power_mode);
}

static int legion_platform_profile_probe(void *, unsigned long *choices)
{
	set_bit(PLATFORM_PROFILE_QUIET, choices);
	set_bit(PLATFORM_PROFILE_BALANCED,choices);
	set_bit(PLATFORM_PROFILE_BALANCED_PERFORMANCE,choices);
	set_bit(PLATFORM_PROFILE_PERFORMANCE,choices);

	return 0;
}

static const struct platform_profile_ops lenovo_wmi_platform_profile_ops = {
	.probe = legion_platform_profile_probe,
	.hidden_choices = NULL,
	.profile_get = legion_platform_profile_get,
	.profile_set = legion_platform_profile_set,
};


int legion_platform_profile_init(struct device *dev)
{
	struct legion_data* data = dev_get_drvdata(dev);

	data->pprof_device = devm_platform_profile_register(dev,"lenovo-legion-pprof",data,&lenovo_wmi_platform_profile_ops);

	if (IS_ERR(data->pprof_device)) {
		return PTR_ERR(data->pprof_device);
	}

	return 0;
}

void legion_platform_profile_exit(struct device *)
{}
