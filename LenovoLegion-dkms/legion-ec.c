// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "legion-ec.h"

#include <linux/delay.h>
#include <asm/io.h>
#include <linux/types.h>


static int ec_wait( u16 port, u8 flag, u8 value) {

    for(int i = 0;i < 100;i++)
    {
        unsigned char result = inb(port);

        if(((result>>flag) & 0x1) == value)
        {
            return 0;
        }
        msleep(1);
    }

    return -1;
}



ssize_t ec_lenovo_write( u16 port, u8 value) {
    if(ec_wait(EC_SC,IBF,0) != 0) return -1;
    outb(WR_EC, EC_SC);
    if(ec_wait(EC_SC,IBF,0) != 0) return -1;
    outb(port, EC_DATA);
    if(ec_wait(EC_SC,IBF,0) != 0) return -1;
    outb(value, EC_DATA);
    if(ec_wait(EC_SC,IBF,0) != 0) return -1;

    return 0;
}

ssize_t ec_lenovo_read(u16 port,u8* value)
{
    if(ec_wait(EC_SC,IBF,0) != 0) return -1;
    outb(RD_EC,EC_SC);
    if(ec_wait(EC_SC,IBF,0) != 0) return -1;
    outb(port, EC_DATA);
    if(ec_wait(EC_SC,OBF,1) != 0) return -1;
    *value = inb(EC_DATA);

    return 0;
}
