/*
 * Copyright (C) 2013 Shanghai Semiclouder, Ltd, All Rights Reserved.

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 *  Author      Version     Time            Email
 *  warnus      v1.0        23/02/2013      ......
 */

#ifndef __LINUX_BOOTARGS_H
#define __LINUX_BOOTARGS_H

#include <asm/types.h>

/* struct bootarg: bootargs.
 *
 */
struct bootarg {            
	u32 reserved0[6]; 
	u8  reserved1;
	u32 reserved2[3]; 
	u8  reserved[3];
};

enum {
	REBOOT_NORMAL = 0,
	REBOOT_FASTBOOT,
	REBOOT_RECOVERY,
	REBOOT_CANCELCHARGE,
};

struct bootarg *bootargs;

#endif
