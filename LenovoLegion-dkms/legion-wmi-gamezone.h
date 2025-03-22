// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#ifndef LINUX_WMI_GAMEZONE_H
#define LINUX_WMI_GAMEZONE_H

#include <linux/types.h>

#define LEGION_WMI_GAMEZONE_GUID "887B54E3-DDDC-4B2C-8B88-68A26A8835D0"


enum LEGION_WMI_METHOD_ID {
	// GPU over clock
	WMI_METHOD_ID_ISSUPPORTGPUOC =  4,

	// Fan speed
	// only fully implemented for some models here
	// often implemented in other classes and methods too
	// below
	WMI_METHOD_ID_GETFAN1SPEED=8,
	WMI_METHOD_ID_GETFAN2SPEED=9,

	// Version of ACPI
	WMI_METHOD_ID_GETVERSION=11,

	// Does it support CPU overclock?
	WMI_METHOD_ID_ISSUPPORTCPUOC=14,


	// Temperatures
	// only fully implemented for some models here
	// often implemented in other classes and methods too
	// below
	WMI_METHOD_ID_GETCPUTEMP=18,
	WMI_METHOD_ID_GETGPUTEMP=19,

	// two state keyboard light
	WMI_METHOD_ID_GETKEYBOARDLIGHT=37,
	WMI_METHOD_ID_SETKEYBOARDLIGHT=36,

	// toggle win key
	// 0 = win key enabled; 1 = win key disabled
	WMI_METHOD_ID_ISSUPPORTDISABLEWINKEY=21,
	WMI_METHOD_ID_GETWINKEYSTATUS=23,
	WMI_METHOD_ID_SETWINKEYSTATUS=22,


	// toggle touchpad
	//0 = touchpad enabled; 1 = touchpad disabled
	WMI_METHOD_ID_ISSUPPORTDISABLETP=24,
	WMI_METHOD_ID_GETTPSTATUS=26,
	WMI_METHOD_ID_SETTPSTATUS=25,

	// GSync
	WMI_METHOD_ID_ISSUPPORTGSYNC=40,
	WMI_METHOD_ID_GETGSYNCSTATUS=41,
	WMI_METHOD_ID_SETGSYNCSTATUS=42,

	//smartFanMode = powermode
	WMI_METHOD_ID_ISSUPPORTSMARTFAN=49,
	WMI_METHOD_ID_GETSMARTFANMODE=45,
	WMI_METHOD_ID_SETSMARTFANMODE=44,

	// power charge mode
	WMI_METHOD_ID_GETPOWERCHARGEMODE=47,

	// overdrive of display to reduce latency
	// 0=off, 1=on
	WMI_METHOD_ID_ISSUPPORTOD=49,
	WMI_METHOD_ID_GETODSTATUS=50,
	WMI_METHOD_ID_SETODSTATUS=51,

	// thermal mode = power mode used for cooling
	WMI_METHOD_ID_GETTHERMALMODE=55,

	// get max frequency of core 0
	WMI_METHOD_ID_GETCPUMAXFREQUENCY=60,

	// check if AC adapter has enough power to overclock
	WMI_METHOD_ID_ISACFITFOROC=62,

	// set iGPU (GPU packaged with CPU) state
	WMI_METHOD_ID_ISSUPPORTIGPUMODE=63,
	WMI_METHOD_ID_GETIGPUMODESTATUS=64,
	WMI_METHOD_ID_SETIGPUMODESTATUS=65,
	WMI_METHOD_ID_NOTIFYDGPUSTATUS=66
};

#define LENOVO_GAMEZONE_SMART_FAN_MODE_EVENT_GUID     "D320289E-8FEA-41E0-86F9-611D83151B5F"
#define LENOVO_GAMEZONE_POWER_CHARGE_MODE_EVENT_GUID  "D320289E-8FEA-41E0-86F9-711D83151B5F"

enum legion_wmi_power_charge_mode {
	LEGION_WMI_POWER_CHARGE_MODE_AC     = 1,
	LEGION_WMI_POWER_CHARGE_MODE_BATERY = 2
};

enum legion_wmi_powermode {
    LEGION_WMI_POWERMODE_QUIET          = 1,
    LEGION_WMI_POWERMODE_BALANCED       = 2,
    LEGION_WMI_POWERMODE_PERFORMANCE    = 3,
    LEGION_WMI_POWERMODE_CUSTOM         = 255
};


/*
 * Special function to set the power mode, waits for the mode to be set
 */
int wmi_lenovo_gamezone_powermode_store(enum legion_wmi_powermode power_mode);


int wmi_lenovo_gamezone_set_value(u32 method_id,u32 value);
int wmi_lenovo_gamezone_get_value(u32 method_id,u32 *value);



#endif // LINUX_WMI_GAMEZONE_H
