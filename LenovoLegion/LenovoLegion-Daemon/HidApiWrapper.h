// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#pragma once

#include <hidapi.h>
#include <dlfcn.h>

namespace LenovoLegionDaemon {


/*-----------------------------------------------------*\
| Type definitions for libhidapi function pointers      |
\*-----------------------------------------------------*/
typedef int                 (*hidapi_wrapper_send_feature_report)   (hid_device*, const unsigned char*, size_t);
typedef int                 (*hidapi_wrapper_get_feature_report)    (hid_device*, unsigned char*, size_t);
typedef int                 (*hidapi_wrapper_get_serial_number_string) (hid_device*, wchar_t*, size_t);
typedef hid_device*         (*hidapi_wrapper_open_path)             (const char*);
typedef hid_device_info*    (*hidapi_wrapper_enumerate)             (unsigned short, unsigned short);
typedef void                (*hidapi_wrapper_free_enumeration)      (hid_device_info*);
typedef void                (*hidapi_wrapper_close)                 (hid_device*);
typedef const wchar_t*      (*hidapi_wrapper_error)                 (hid_device*);

/*-----------------------------------------------------*\
|  See comment at top of HyperXQuadcastSDetect.cpp for  |
|  details about the hidapi wrapper for this device     |
\*-----------------------------------------------------*/
struct hidapi_wrapper
{
    void*                                   dyn_handle;
    hidapi_wrapper_send_feature_report      hid_send_feature_report;
    hidapi_wrapper_get_feature_report       hid_get_feature_report;
    hidapi_wrapper_get_serial_number_string hid_get_serial_number_string;
    hidapi_wrapper_open_path                hid_open_path;
    hidapi_wrapper_enumerate                hid_enumerate;
    hidapi_wrapper_free_enumeration         hid_free_enumeration;
    hidapi_wrapper_close                    hid_close;
    hidapi_wrapper_error                    hid_error;
};

}
