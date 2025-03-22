// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#ifndef _LEGION_WMI_H
#define _LEGION_WMI_H

#include <linux/types.h>



#define LEGION_WMI_LENOVO_OTHER_METHOD_GUID "dc2a8805-3a8c-41ba-a6f7-092e0089cd3b"

#define WMI_METHOD_ID_GET_FEATURE_VALUE 17
#define WMI_METHOD_ID_SET_FEATURE_VALUE 18




enum OtherMethodFeature {
    OtherMethodFeature_U1                                              = 0x010000,  //->PC00.LPCB.EC0.REJF
    OtherMethodFeature_U2                                              = 0x0F0000,  //->C00.PEG1.PXP._STA?
    OtherMethodFeature_U3                                              = 0x030000,  //->PC00.LPCB.EC0.FLBT?
    OtherMethodFeature_CPU_SHORT_TERM_POWER_LIMIT                      = 0x01010000,
    OtherMethodFeature_CPU_LONG_TERM_POWER_LIMIT                       = 0x01020000,
    OtherMethodFeature_CPU_PEAK_POWER_LIMIT                            = 0x01030000,
    OtherMethodFeature_CPU_TEMPERATURE_LIMIT                           = 0x01040000,

    OtherMethodFeature_APU_PPT_POWER_LIMIT                             = 0x01050000,

    OtherMethodFeature_CPU_CROSS_LOAD_POWER_LIMIT                      = 0x01060000,
    OtherMethodFeature_CPU_L1_TAU                                      = 0x01070000,


    OtherMethodFeature_GPU_POWER_BOOST                                 = 0x02010000,
    OtherMethodFeature_GPU_cTGP                                        = 0x02020000,
    OtherMethodFeature_GPU_TEMPERATURE_LIMIT                           = 0x02030000,
    OtherMethodFeature_GPU_POWER_TARGET_ON_AC_OFFSET_FROM_BASELINE     = 0x02040000,
	OtherMethodFeature_CPU_POWER_POWER_BOOST					       = 0x020b0000,


    OtherMethodFeature_CPU_SPEED                                       = 0x04030001,
    OtherMethodFeature_GPU_SPEED                                       = 0x04030002,


    OtherMethodFeature_C_U1                                            = 0x05010000,
    OtherMethodFeature_TEMP_CPU                                        = 0x05040000,
    OtherMethodFeature_TEMP_GPU                                        = 0x05050000
};


int wmi_lenovo_other_set_value(enum OtherMethodFeature feature_id,u32 value);
int wmi_lenovo_other_get_value(enum OtherMethodFeature feature_id,u32 *value);

#endif
