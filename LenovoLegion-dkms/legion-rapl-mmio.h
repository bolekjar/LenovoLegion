// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include <linux/io.h>
#include <linux/pci.h>

#ifndef LEGION_RAPL_MMIO_H_
#define LEGION_RAPL_MMIO_H_

struct legion_rapl_mmio_private {
	void __iomem *rapl_mmio_base;
	struct pci_dev *thermal_dev;
	resource_size_t bar_start;
	resource_size_t bar_len;
	struct mutex lock;
};

int  legion_rapl_mmio_init(struct legion_rapl_mmio_private* rapl_mmio_private);
void legion_rapl_mmio_exit(struct legion_rapl_mmio_private* rapl_mmio_private);

int  legion_pl1_power_read(struct legion_rapl_mmio_private* rapl_mmio_private,unsigned int* pl1);
int  legion_pl2_power_read(struct legion_rapl_mmio_private* rapl_mmio_private,unsigned int* pl2);
int  legion_is_locked_power_read(struct legion_rapl_mmio_private* rapl_mmio_private,bool* isLocked);
int  legion_set_and_lock(struct legion_rapl_mmio_private* rapl_mmio_private,unsigned int pl1_mw,unsigned int pl2_mw);

int  legion_pl4_power_read(struct legion_rapl_mmio_private* rapl_mmio_private,unsigned int* pl4);
int  legion_pl4_lock_read(struct legion_rapl_mmio_private* rapl_mmio_private,bool* isLocked);
int  legion_pl4_set_and_lock(struct legion_rapl_mmio_private* rapl_mmio_private,unsigned int pl4_mw);


#endif /* LEGION_RAPL_MMIO_H_ */
