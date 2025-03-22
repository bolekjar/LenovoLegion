// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "legion-wmi.h"
#include "legion-wmi-gamezone.h"
#include "legion-common.h"

#include <linux/acpi.h>


int wmi_get_value_u32(void *args, size_t args_size,const char *guid,u32 method_id,u32 *value) {

    struct acpi_buffer output = {
        ACPI_ALLOCATE_BUFFER,
        NULL
    };

    struct acpi_buffer input  = {
        args_size,
        args
    };


    if (ACPI_FAILURE(wmi_evaluate_method(guid, 0, method_id, &input, &output))) {
        return -EIO;
    }

    if (((union acpi_object *)output.pointer)->type == ACPI_TYPE_INTEGER) {
        *value = ((union acpi_object *)output.pointer)->integer.value;
    }
    else {
        kfree((union acpi_object *)output.pointer);
        return -EIO;
    }

    kfree((union acpi_object *)output.pointer);

    return 0;
}

int wmi_set_value(void *args, size_t args_size,const char *guid,u32 method_id) {

    struct acpi_buffer input  = {
        args_size,
        args
    };


    if (ACPI_FAILURE(wmi_evaluate_method(guid, 0, method_id, &input, NULL))) {
        return -EIO;
    }


    return 0;
}


int wmi_get_value_buffer(void *args, size_t args_size,const char *guid,u32 method_id,u8 *res,size_t ressize) {

    struct acpi_buffer output = {
        ACPI_ALLOCATE_BUFFER,
        NULL
    };

    struct acpi_buffer input  = {
        args_size,
        args
    };

    if (ACPI_FAILURE(wmi_evaluate_method(guid, 0, method_id, &input, &output))) {
        return -EIO;
    }

    if (((union acpi_object *)output.pointer)->type == ACPI_TYPE_BUFFER) {
    	for (size_t i = 0; i < ressize; ++i)
    		res[i] = ((union acpi_object *)output.pointer)->buffer.pointer[i];
    }
    else {
        kfree((union acpi_object *)output.pointer);
        return -EIO;
    }

    return 0;
}
