// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "legion-common.h"
#include "legion-wmi-other-sysfs.h"
#include "legion-wmi-other.h"
#include "legion-wmi-helpers.h"
#include "legion-wmi-capdata01.h"
#include "legion-wmi-gamezone.h"
#include "legion-firmware-attributes-class.h"

#include <linux/types.h>
#include <linux/delay.h>
#include <linux/component.h>
#include <linux/container_of.h>
#include <linux/device.h>
#include <linux/export.h>
#include <linux/gfp_types.h>
#include <linux/idr.h>
#include <linux/wmi.h>

static DEFINE_IDA(legion_wmi_other_ida);

#define LEGION_WMI_OTHER_FW_ATTR_BASE_PATH "legion-wmi-other"


enum attribute_property {
	DEFAULT_VAL,
	MAX_VAL,
	MIN_VAL,
	STEP_VAL,
	SUPPORTED,
	STEPS_VAL
};


static ssize_t legion_wmi_other_cap01_attr_show_no_steps_val(struct legion_wmi_cd01_list *cd01_list,enum attribute_property prop,const u32 attribute_id_raw,enum thermal_mode modes[THERMAL_MODES_SIZE],char *buf,ssize_t* buf_len) {
	struct legion_wmi_capdata01 capdata;
	ssize_t 					len = 0;

	modes[THERMAL_MODES_SIZE - 1] = LEGION_WMI_GZ_THERMAL_MODE_END;
	for(int i = 0;(i < THERMAL_MODES_SIZE) && (modes[i] != LEGION_WMI_GZ_THERMAL_MODE_END);i++){
		int ret 	= 0;
		int value 	= 0;

		len += sysfs_emit_at(buf,len,"%d=",modes[i]);

		ret = legion_wmi_cd01_get_data(cd01_list,attribute_id_raw | FIELD_PREP(LEGION_WMI_MODE_ID_MASK, modes[i]), &capdata);
		if (ret) {
			if(modes[i + 1] != LEGION_WMI_GZ_THERMAL_MODE_END)
				len += sysfs_emit_at(buf,len,",");
			continue;
		}

		switch (prop) {
		case DEFAULT_VAL:
			value = capdata.default_value;
			break;
		case MAX_VAL:
			value = capdata.max_value;
			break;
		case MIN_VAL:
			value = capdata.min_value;
			break;
		case STEP_VAL:
			value = capdata.step;
			break;
		case SUPPORTED:
			value = capdata.supported;
			break;
		default:
			return -EINVAL;
		}

		len += sysfs_emit_at(buf,len,"%d",value);

    	if(modes[i + 1] != LEGION_WMI_GZ_THERMAL_MODE_END)
			len += sysfs_emit_at(buf,len,",");

	}
	(*buf_len) += len;

	return 0;
}

static ssize_t legion_wmi_other_cap01_attr_show_steps_val(struct legion_wmi_dd_list *dd_list,const u32 attribute_id_raw,enum thermal_mode modes[THERMAL_MODES_SIZE],char *buf,ssize_t* buf_len) {
	struct legion_wmi_ddata     lddata[48] = {{0,0}};
	ssize_t 					len 	   = 0;

	modes[THERMAL_MODES_SIZE - 1] = LEGION_WMI_GZ_THERMAL_MODE_END;
	for(int i = 0;(i < THERMAL_MODES_SIZE) && (modes[i] != LEGION_WMI_GZ_THERMAL_MODE_END);i++){
		int ret = 0;

		len += sysfs_emit_at(buf,len,"%d=",modes[i]);

		ret = legion_wmi_dd_get_data(dd_list,attribute_id_raw | FIELD_PREP(LEGION_WMI_MODE_ID_MASK, modes[i]), lddata,sizeof(lddata)/sizeof(lddata[0]) - 1);
		if (ret) {
			if(modes[i + 1] != LEGION_WMI_GZ_THERMAL_MODE_END)
				len += sysfs_emit_at(buf,len,",");
			continue;
		}

		for(int i = 0;i < (sizeof(lddata)/sizeof(lddata[0])) && lddata[i].id != 0;++i)
		{
			len += sysfs_emit_at(buf,len,"%d",lddata[i].value);

			if(lddata[i + 1].id != 0)
				len += sysfs_emit_at(buf,len,"|");
		}

    	if(modes[i + 1] != LEGION_WMI_GZ_THERMAL_MODE_END)
			len += sysfs_emit_at(buf,len,",");
	}

	(*buf_len) += len;
	return 0;
}

/**
 * legion_wmi_other_cap01_attr_show() - Get the value of the specified attribute property
 *
 * @kobj: Pointer to the driver object.
 * @kattr: Pointer to the attribute calling this function.
 * @buf: The buffer to write to.
 * @tunable_attr: The attribute to be read.
 * @prop: The property of this attribute to be read.
 *
 * Retrieves the given property from the capability data 01 struct for the
 * specified attribute's "custom" thermal mode. This function is intended
 * to be generic so it can be called from any integer attributes "_show"
 * function.
 *
 * If the WMI is success the sysfs attribute is notified.
 *
 * Return: Either number of characters written to buf, or an error code.
 */
static ssize_t legion_wmi_other_cap01_attr_show(struct kobject *kobj,struct kobj_attribute *kattr, char *buf, enum attribute_property prop,u32 attribute_id)
{
	struct legion_wmi_other_priv *priv = dev_get_drvdata(kobj_to_dev(kobj->parent));
	ssize_t len = 	0;
	int 	ret =	0;
	enum thermal_mode modes[THERMAL_MODES_SIZE] = {LEGION_WMI_GZ_THERMAL_MODE_END};

	ret = legion_wmi_other_notifier_call(&modes,LEGION_WMI_GZ_GET_SUPPORTED_THERMAL_MODES);
	if (ret)
		return ret;

	if(prop == STEPS_VAL)
	{
		ret = legion_wmi_other_cap01_attr_show_steps_val(priv->dd_list,attribute_id  & (~LEGION_WMI_MODE_ID_MASK),modes,buf,&len);
		if(ret)
			return ret;
	}
	else
	{
		ret = legion_wmi_other_cap01_attr_show_no_steps_val(priv->cd01_list,prop,attribute_id  & (~LEGION_WMI_MODE_ID_MASK),modes,buf,&len);
		if(ret)
			return ret;
	}

	len += sysfs_emit_at(buf,len,"\n");
	return len;
}

/**
 * legion_wmi_attr_current_value_store() - Set the current value of the given attribute
 * @kobj: Pointer to the driver object.
 * @kattr: Pointer to the attribute calling this function.
 * @buf: The buffer to read from, this is parsed to `int` type.
 * @count: Required by sysfs attribute macros, pass in from the callee attr.
 * @tunable_attr: The attribute to be stored.
 *
 * Sets the value of the given attribute when operating under the "custom"
 * smartfan profile. The current smartfan profile is retrieved from the
 * lenovo-wmi-gamezone driver and error is returned if the result is not
 * "custom". This function is intended to be generic so it can be called from
 * any integer attribute's "_store" function. The integer to be sent to the WMI
 * method is range checked and an error code is returned if out of range.
 *
 * If the value is valid and WMI is success, then the sysfs attribute is
 * notified.
 *
 * Return: Either count, or an error code.
 */
static ssize_t legion_wmi_attr_current_value_store(struct kobject *kobj,
					struct kobj_attribute *kattr,
					const char *buf, size_t count,u32 attribute_id)
{
	struct legion_wmi_other_priv *priv = dev_get_drvdata(kobj_to_dev(kobj->parent));
	u32 attribute_id_raw = attribute_id  & (~LEGION_WMI_MODE_ID_MASK);

	struct wmi_method_args_32 args;
	struct legion_wmi_capdata01 capdata;
	enum thermal_mode mode;

	u32 value;


	int ret;

	ret = legion_wmi_other_notifier_call(&mode,LEGION_WMI_GZ_GET_THERMAL_MODE);
	if (ret)
		return ret;

	if (mode != LEGION_WMI_GZ_THERMAL_MODE_CUSTOM)
		return -EBUSY;


	ret = legion_wmi_cd01_get_data(priv->cd01_list,  attribute_id_raw | FIELD_PREP(LEGION_WMI_MODE_ID_MASK, LEGION_WMI_GZ_THERMAL_MODE_CUSTOM), &capdata);
	if (ret)
		return ret;

	ret = kstrtouint(buf, 10, &value);
	if (ret)
		return ret;

	if (capdata.supported == 0)
	{
		return -ENODEV;
	}

	if(capdata.max_value == 0 && capdata.min_value == 0 && capdata.step == 0)
	{
		struct legion_wmi_ddata     lddata[48] = {{0,0}};
		bool contains_value					   = false;

		ret = legion_wmi_dd_get_data(priv->dd_list,attribute_id_raw | FIELD_PREP(LEGION_WMI_MODE_ID_MASK, LEGION_WMI_GZ_THERMAL_MODE_CUSTOM), lddata,sizeof(lddata)/sizeof(lddata[0]) - 1);
		if (ret)
			return ret;

		for(int i = 0;i < (sizeof(lddata)/sizeof(lddata[0])) && lddata[i].id != 0;++i)
		{
			if(lddata[i].value == value) {
				contains_value = true;
				break;
			}
		}
	}
	else
	{
		if (value < capdata.min_value || value > capdata.max_value)
			return -EINVAL;
	}

	args.arg0 = attribute_id_raw;
	args.arg1 = value;

	ret = legion_wmi_dev_evaluate_int(priv->wdev, 0x0, LEGION_WMI_OTHER_FEATURE_VALUE_SET,
				    (unsigned char *)&args, sizeof(args), NULL);
	if (ret)
		return ret;

	return count;
};


/**
 * legion_wmi_attr_current_value_show() - Get the current value of the given attribute
 * @kobj: Pointer to the driver object.
 * @kattr: Pointer to the attribute calling this function.
 * @buf: The buffer to write to.
 * @tunable_attr: The attribute to be read.
 *
 * Retrieves the value of the given attribute for the current smartfan profile.
 * The current smartfan profile is retrieved from the lenovo-wmi-gamezone driver.
 * This function is intended to be generic so it can be called from any integer
 * attribute's "_show" function.
 *
 * If the WMI is success the sysfs attribute is notified.
 *
 * Return: Either number of characters written to buf, or an error code.
 */
static ssize_t legion_wmi_attr_current_value_show(struct kobject *kobj,
				       struct kobj_attribute *kattr, char *buf,u32 attribute_id)
{
	struct legion_wmi_other_priv *priv = dev_get_drvdata(kobj_to_dev(kobj->parent));
	u32 attribute_id_raw = attribute_id  & (~LEGION_WMI_MODE_ID_MASK);
	struct wmi_method_args_32 args;
	enum thermal_mode mode;
	int retval;
	int ret;

	ret = legion_wmi_other_notifier_call(&mode,LEGION_WMI_GZ_GET_THERMAL_MODE);
	if (ret) {
		return ret;
	}

	args.arg0 = attribute_id_raw;

	ret = legion_wmi_dev_evaluate_int(priv->wdev, 0x0, LEGION_WMI_OTHER_FEATURE_VALUE_GET,
				    (unsigned char *)&args, sizeof(args),
				    &retval);
	if (ret) {
		return ret;
	}

	return sysfs_emit(buf, "%d\n", retval);
}

/* Attribute property read only */
#define __LEGION_WMI_RO_CAP01(_attrname,_func, _prop_type, _attr_id)                \
	static ssize_t func_##_func##_show(                             					    \
		struct kobject *kobj, struct kobj_attribute *kattr, char *buf) 						\
	{                                                                       				\
		return legion_wmi_other_cap01_attr_show(kobj, kattr, buf,_prop_type,_attr_id);      \
	}                                                                       				\
	static struct kobj_attribute kobj_attr_##_func##_prop =               				\
	__LEGION_WMI_ATTR_RO(_attrname, _func)

/* Attribute current value read/write */
#define __LEGION_WMI_CURRENT_VALUE(_attrname,_func,_attr_id)                         	\
	static ssize_t func_##_func##_set(                        							\
		struct kobject *kobj, struct kobj_attribute *kattr,            					\
		const char *buf, size_t count)                                 					\
	{                                                                      				\
		return legion_wmi_attr_current_value_store(kobj, kattr, buf, count,      		\
				_attr_id);                   											\
	}                                                                      				\
	static ssize_t func_##_func##_get(                         						    \
		struct kobject *kobj, struct kobj_attribute *kattr, char *buf) 					\
	{                                                                      				\
		return legion_wmi_attr_current_value_show(kobj, kattr, buf,_attr_id );  		\
	}                                                                      				\
	static struct kobj_attribute kobj_attr_##_func##_prop =        					    \
	__LEGION_WMI_ATTR_RW(_attrname, _func)

#define __LEGION_WMI_OTHER(_attrname,_attr_id,_name) \
__LEGION_WMI_RO_CAP01(default_value,_attrname##_default_value,DEFAULT_VAL,_attr_id);\
__LEGION_WMI_RO_CAP01(max_value,_attrname##_max_value,MAX_VAL,_attr_id);\
__LEGION_WMI_RO_CAP01(min_value,_attrname##_min_value,MIN_VAL,_attr_id);\
__LEGION_WMI_RO_CAP01(scalar_increment,_attrname##_scalar_increment,STEP_VAL,_attr_id);\
__LEGION_WMI_RO_CAP01(steps,_attrname##_steps,STEPS_VAL,_attr_id);\
__LEGION_WMI_RO_CAP01(supported,_attrname##_supported,SUPPORTED,_attr_id);\
__LEGION_WMI_CURRENT_VALUE(current_value,_attrname##_current_value,_attr_id);\
__LEGION_WMI_KOBJ_ATTR_RO_STATIC_STRING(type,"integer",_attrname##_type);\
__LEGION_WMI_KOBJ_ATTR_RO_STATIC_STRING(display_name,_name,_attrname##_display_name);\
\
static struct attribute *legion_sysfs_##_attrname##_attributes[] = {\
	&kobj_attr_##_attrname##_default_value_prop.attr,\
	&kobj_attr_##_attrname##_max_value_prop.attr,\
	&kobj_attr_##_attrname##_min_value_prop.attr,\
	&kobj_attr_##_attrname##_scalar_increment_prop.attr,\
	&kobj_attr_##_attrname##_steps_prop.attr,\
	&kobj_attr_##_attrname##_supported_prop.attr,\
	&kobj_attr_##_attrname##_current_value_prop.attr,\
	&kobj_attr_##_attrname##_type.attr,\
	&kobj_attr_##_attrname##_display_name.attr,\
	NULL\
};






#define __LEGION_WMI_OTHER_RW_VALUE(_attrname,_func,_attr_id,_possible_states)\
static ssize_t func_##_func##_set(\
	struct kobject *kobj, struct kobj_attribute *kattr,\
	const char *buf, size_t count)\
{\
	int ret,value;\
\
	ret = kstrtouint(buf, 10, &value);\
	if (ret)\
		return ret;\
\
	for (size_t i = 0; i < sizeof(_possible_states)/sizeof(_possible_states[0]);++i)\
	{\
		if(value == _possible_states[i])\
		{\
			struct legion_wmi_other_priv *priv = dev_get_drvdata(kobj_to_dev(kobj->parent));\
			struct wmi_method_args_32 args;\
\
			args.arg0 = _attr_id;\
			args.arg1 = value;\
\
			ret = legion_wmi_dev_evaluate_int(priv->wdev, 0x0, LEGION_WMI_OTHER_FEATURE_VALUE_SET,\
						    (unsigned char *)&args, sizeof(args), NULL);\
			if (ret)\
				return ret;\
\
			return count;\
		}\
\
	}\
	return -EINVAL;\
}\
static ssize_t func_##_func##_get(struct kobject *kobj, struct kobj_attribute *kattr, char *buf)\
{\
	struct legion_wmi_other_priv *priv = dev_get_drvdata(kobj_to_dev(kobj->parent));\
\
	struct wmi_method_args_32 args;\
	int ret;\
	int retval;\
\
	args.arg0 = _attr_id;\
\
	ret = legion_wmi_dev_evaluate_int(priv->wdev, 0x0, LEGION_WMI_OTHER_FEATURE_VALUE_GET,\
				    (unsigned char *)&args, sizeof(args),\
				    &retval);\
	if (ret) {\
		return ret;\
	}\
\
	return sysfs_emit(buf, "%d\n", retval);\
}\
static struct kobj_attribute kobj_attr_##_func##_prop =        					    \
__LEGION_WMI_ATTR_RW(_attrname, _func)


#define __LEGION_WMI_OTHER_RO_VALUE(_attrname,_func,_attr_id)\
static ssize_t func_##_func##_show(struct kobject *kobj, struct kobj_attribute *kattr, char *buf)\
{\
	struct legion_wmi_other_priv *priv = dev_get_drvdata(kobj_to_dev(kobj->parent));\
\
	struct wmi_method_args_32 args;\
	int ret;\
	int retval;\
\
	args.arg0 = _attr_id;\
\
	ret = legion_wmi_dev_evaluate_int(priv->wdev, 0x0, LEGION_WMI_OTHER_FEATURE_VALUE_GET,\
				    (unsigned char *)&args, sizeof(args),\
				    &retval);\
	if (ret) {\
		return ret;\
	}\
\
	return sysfs_emit(buf, "%d\n", retval);\
}\
static struct kobj_attribute kobj_attr_##_func##_prop =        					    \
__LEGION_WMI_ATTR_RO(_attrname, _func)


/*
 * IGPUModeStates
 */
static u32 IGPUModeStates[] = {
		0, //Default
		1, //IGPUOnly
		2  //Auto
};
__LEGION_WMI_OTHER_RW_VALUE(current_value,igpu_mode_current_value,IGPUMode,IGPUModeStates);
__LEGION_WMI_OTHER_RO_VALUE(change_status,igpu_mode_change_status,IGPUModeChangeStatus);
__LEGION_WMI_KOBJ_ATTR_RO_STATIC_STRING(display_name,"IGPU mode related settings",igpu_mode_display_name);

static struct attribute *legion_sysfs_igpu_mode_attributes[] = {
	&kobj_attr_igpu_mode_current_value_prop.attr,
	&kobj_attr_igpu_mode_change_status_prop.attr,
	&kobj_attr_igpu_mode_display_name.attr,
	NULL
};


/*
 * CPU
 */
__LEGION_WMI_OTHER(cpu_stp_limit,CPUShortTermPowerLimit,"Set the CPU short term power limit");
__LEGION_WMI_OTHER(cpu_ltp_limit,CPULongTermPowerLimit,"Set the CPU long term power limit");
__LEGION_WMI_OTHER(cpu_pp_limit,CPUPeakPowerLimit,"Set the CPU peak power limit");
__LEGION_WMI_OTHER(cpu_tmp_limit,CPUTemperatureLimit ,"Set the CPU temperature limit");
__LEGION_WMI_OTHER(apus_pptp_limit,APUsPPTPowerLimit ,"Set the APUs ppt power limit");
__LEGION_WMI_OTHER(cpu_clp_limit,CPUCrossLoadingPowerLimit ,"Set the CPU cross loading power limit");
__LEGION_WMI_OTHER(cpu_pl1_tau,CPUPL1Tau,"Set the CPU PL1 Tau limit");


/*
 * GPU
 */
__LEGION_WMI_OTHER(gpu_power_boost,GPUPowerBoost,"Set the GPU power boost");
__LEGION_WMI_OTHER(gpu_configurable_tgp,GPUConfigurableTGP,"Set the GPU configurable TGP");
__LEGION_WMI_OTHER(gpu_temperature_limit,GPUTemperatureLimit,"Set the GPU temperature limit");
__LEGION_WMI_OTHER(gpu_total_onac,GPUTotalProcessingPowerTargetOnAcOffsetFromBaseline ,"Set the GPU total processing power target on AC offset from base line limit");
__LEGION_WMI_OTHER(gpu_to_cpu_dynamic_boost,GPUToCPUDynamicBoost ,"Set the GPU To CPU dynamic boost");


/*
 * GPU Other
 */
__LEGION_WMI_OTHER_RO_VALUE(status,gpu_other_status,GPUStatus);
__LEGION_WMI_OTHER_RO_VALUE(did_vid,gpu_other_did_vid,GPUDidVid);
__LEGION_WMI_KOBJ_ATTR_RO_STATIC_STRING(display_name,"GPU other related settings",gpu_other_display_name);

static struct attribute *legion_sysfs_gpu_other_attributes[] = {
	&kobj_attr_gpu_other_status_prop.attr,
	&kobj_attr_gpu_other_did_vid_prop.attr,
	&kobj_attr_gpu_other_display_name.attr,
	NULL
};

/*
 * Instant Boot
 */

 static u32 StateInstantBoot[] = {
 		0, //Off
 		1  //On
 };

__LEGION_WMI_OTHER_RW_VALUE(ac,instant_boot_other_ac,InstantBootAc,StateInstantBoot);
__LEGION_WMI_OTHER_RW_VALUE(usb_power_delivery,instant_boot_other_did_vid,InstantBootUsbPowerDelivery,StateInstantBoot);
__LEGION_WMI_KOBJ_ATTR_RO_STATIC_STRING(display_name,"Instant boot related settings",instant_boot_other_display_name);

static struct attribute *legion_sysfs_instant_boot_other_attributes[] = {
	&kobj_attr_instant_boot_other_ac_prop.attr,
	&kobj_attr_instant_boot_other_did_vid_prop.attr,
	&kobj_attr_instant_boot_other_display_name.attr,
	NULL
};


/*
 * Other
 */
static u32 StateOther[] = {
		0, //Off
		1  //On
};
__LEGION_WMI_OTHER_RW_VALUE(fan_full_speed,other_fan_full_speed,FanFullSpeed,StateOther);
__LEGION_WMI_OTHER_RW_VALUE(over_drive,other_over_drive,OverDrive,StateOther);
__LEGION_WMI_OTHER_RW_VALUE(god_mode_fnq_switchable,other_god_mode_fnq_switchable,GodModeFnQSwitchable,StateOther);
__LEGION_WMI_OTHER_RO_VALUE(zone_support_version,other_zone_support_version,LegionZoneSupportVersion);
__LEGION_WMI_OTHER_RO_VALUE(supported_power_modes,other_supported_power_modes,SupportedPowerModes);
__LEGION_WMI_OTHER_RO_VALUE(nvidia_gpu_dd_switching,other_nvidia_gpu_dd_switching,NvidiaGPUDynamicDisplaySwitching);
__LEGION_WMI_OTHER_RO_VALUE(flip_to_start,flip_to_start,FlipToStart);
__LEGION_WMI_KOBJ_ATTR_RO_STATIC_STRING(display_name,"Other related settings",other_display_name);

static struct attribute *legion_sysfs_other_attributes[] = {
	&kobj_attr_other_fan_full_speed_prop.attr,
	&kobj_attr_other_over_drive_prop.attr,
	&kobj_attr_other_god_mode_fnq_switchable_prop.attr,
	&kobj_attr_other_zone_support_version_prop.attr,
	&kobj_attr_other_supported_power_modes_prop.attr,
	&kobj_attr_other_nvidia_gpu_dd_switching_prop.attr,
	&kobj_attr_flip_to_start_prop.attr,
	&kobj_attr_other_display_name.attr,
	NULL
};

/*
 * AMD
 */
static u32 StateAMD[] = {
		0, //Off
		1  //On
};
__LEGION_WMI_OTHER_RW_VALUE(smart_shiftmode,amd_smart_shiftmode,AMDSmartShiftMode,StateAMD);
__LEGION_WMI_OTHER_RW_VALUE(skin_temperature_tracking,amd_skin_temperature_tracking,AMDSkinTemperatureTracking,StateAMD);
__LEGION_WMI_KOBJ_ATTR_RO_STATIC_STRING(display_name,"AMD related settings",amd_display_name);

static struct attribute *legion_sysfs_amd_attributes[] = {
	&kobj_attr_amd_smart_shiftmode_prop.attr,
	&kobj_attr_amd_skin_temperature_tracking_prop.attr,
	&kobj_attr_amd_display_name.attr,
	NULL
};

static const struct attribute_group legion_sysfs_other_group[] = {
		{
			.name = "igpu_mode",
			.attrs = legion_sysfs_igpu_mode_attributes
		},


		{
			.name = "cpu_stp_limit",
			.attrs = legion_sysfs_cpu_stp_limit_attributes
		},
		{
			.name = "cpu_ltp_limit",
			.attrs = legion_sysfs_cpu_ltp_limit_attributes
		},
		{
			.name = "cpu_pp_limit",
			.attrs = legion_sysfs_cpu_pp_limit_attributes
		},
		{
			.name = "cpu_clp_limit",
			.attrs = legion_sysfs_cpu_clp_limit_attributes
		},
		{
			.name = "cpu_tmp_limit",
			.attrs = legion_sysfs_cpu_tmp_limit_attributes
		},
		{
			.name = "apus_pptp_limit",
			.attrs = legion_sysfs_apus_pptp_limit_attributes
		},
		{
			.name = "cpu_pl1_tau",
			.attrs = legion_sysfs_cpu_pl1_tau_attributes
		},



		{
			.name = "gpu_power_boost",
			.attrs = legion_sysfs_gpu_power_boost_attributes
		},
		{
			.name = "gpu_configurable_tgp",
			.attrs = legion_sysfs_gpu_configurable_tgp_attributes
		},
		{
			.name = "gpu_temperature_limit",
			.attrs = legion_sysfs_gpu_temperature_limit_attributes
		},
		{
			.name = "gpu_total_onac",
			.attrs = legion_sysfs_gpu_total_onac_attributes
		},
		{
			.name = "gpu_to_cpu_dynamic_boost",
			.attrs = legion_sysfs_gpu_to_cpu_dynamic_boost_attributes
		},

		{
			.name = "gpu_other",
			.attrs = legion_sysfs_gpu_other_attributes
		},
		{
			.name = "instant_boot",
			.attrs = legion_sysfs_instant_boot_other_attributes
		},
		{
			.name = "other",
			.attrs = legion_sysfs_other_attributes
		},
		{
			.name = "amd",
			.attrs = legion_sysfs_amd_attributes
		},

		{}
};

/**
 * lwmi_om_fw_attr_add() - Register all firmware_attributes_class members
 * @priv: The Other Mode driver data.
 *
 * Return: Either 0, or an error code.
 */
static int legion_wmi_other_fw_attr_add(struct legion_wmi_other_priv *priv)
{
	int err = 0;
	unsigned int i   = 0;
	priv->ida_id = ida_alloc(&legion_wmi_other_ida, GFP_KERNEL);
	if (priv->ida_id < 0)
		return priv->ida_id;

	priv->fw_attr_dev = device_create(&legion_firmware_attributes_class, NULL,
					  MKDEV(0, 0), priv, "%s-%u",
					  LEGION_WMI_OTHER_FW_ATTR_BASE_PATH,
					  priv->ida_id);
	if (IS_ERR(priv->fw_attr_dev)) {
		err = PTR_ERR(priv->fw_attr_dev);
		goto err_free_ida;
	}

	priv->fw_attr_kset = kset_create_and_add("attributes", NULL,
						 &priv->fw_attr_dev->kobj);
	if (!priv->fw_attr_kset) {
		err = -ENOMEM;
		goto err_unregister_fw_attr_dev;
	}

	for (i = 0; i < ARRAY_SIZE(legion_sysfs_other_group) - 1; i++) {
		err = sysfs_create_group(&priv->fw_attr_kset->kobj,
				&legion_sysfs_other_group[i]);
		if (err)
			goto err_remove_groups;
	}

	return 0;
err_remove_groups:
	while (i--)
		sysfs_remove_group(&priv->fw_attr_kset->kobj,&legion_sysfs_other_group[i]);
	kset_unregister(priv->fw_attr_kset);
err_unregister_fw_attr_dev:
	device_unregister(priv->fw_attr_dev);
err_free_ida:
	ida_free(&legion_wmi_other_ida, priv->ida_id);
	return err;
}

/**
 * legion_wmi_gz_fw_attr_remove() - Unregister all capability data attribute groups
 * @priv: the lenovo-wmi-other driver data.
 */
static void legion_wmi_other_fw_attr_remove(struct legion_wmi_other_priv *priv)
{
	/* Remove sysfs group first */
	for (unsigned int i = 0; i < ARRAY_SIZE(legion_sysfs_other_group) - 1; i++)
		sysfs_remove_group(&priv->fw_attr_kset->kobj,&legion_sysfs_other_group[i]);

	/* Unregister kset before device since kset is child of device's kobj */
	kset_unregister(priv->fw_attr_kset);

	/* Device unregister will clean up remaining kobject hierarchy */
	device_unregister(priv->fw_attr_dev);

	ida_free(&legion_wmi_other_ida, priv->ida_id);
}

int  legion_wmi_other_sysfs_init(struct legion_wmi_other_priv *priv) {
	return legion_wmi_other_fw_attr_add(priv);
}

void legion_wmi_other_sysfs_exit(struct legion_wmi_other_priv *priv) {
    legion_wmi_other_fw_attr_remove(priv);
}
