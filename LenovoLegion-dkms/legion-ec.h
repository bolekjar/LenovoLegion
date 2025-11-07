// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#ifndef LEGION_EC_H
#define LEGION_EC_H

#include <linux/types.h>

//Embedded Controller Command, EC_SC
#define  EC_SC  0x66
//Embedded Controller Data
#define  EC_DATA  0x62
//Input Buffer Full (IBF) flag, Input Buffer is full 1, Input Buffer is empty 0
#define IBF  1
//Output Buffer Full (OBF) flag, Output buffer is full 1, Output buffer is empty 0
#define OBF  0
//Read Embedded Controller(RD_EC)
#define RD_EC  0x80
//Write Embedded Controller(WR_EC)
#define WR_EC  0x81
//Query Embedded Controller(QR_EC)
#define QR_EC  0x84

/*
 * Maximum speed control
 */
#define EC_FULLFANSPEED  0xBD
#define EC_FANFULLSPEED_ON  0x40
#define EC_FANFULLSPEED_OFF 0x80

/*
 * Lock fan controler
 */
#define EC_LOCKFANCONTROLLER     0xc4AB
#define EC_LOCKFANCONTROLLER_ON  0x08
#define EC_LOCKFANCONTROLLER_OFF 0x00

/*
 * Minimum fan curve
 */
#define EC_MINIFANCUVE_ON_COOL 		0xC536
#define EC_MINIFANCUVE_ON_COOL_ON   0x04
#define EC_MINIFANCUVE_ON_COOL_OFF  0xA0


ssize_t ec_lenovo_write(u16 port, u8 value);
ssize_t ec_lenovo_read(u16 port, u8 *value);


#endif // LEGION_EC_H
