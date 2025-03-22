// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#ifndef LEGION_FANCURVE_H
#define LEGION_FANCURVE_H

#include <linux/limits.h>
#include <linux/sysfs.h>


#define MAX_FANCURVE_SIZE 10

struct fancurve {
    u8 points[MAX_FANCURVE_SIZE];
};


const struct attribute_group* get_fancurve_attaribute_group(void);

#endif // LEGION_FANCURVE_H
