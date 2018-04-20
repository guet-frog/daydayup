/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <command.h>
#include <asm/reboot.h>
#include <asm/io.h>

#define CLOUDER_REBOOT_MAGIC (0x1234abcd)

void _machine_restart(void)
{
	printf("rebooting system now ...\n");
	writel(CLOUDER_REBOOT_MAGIC, 0xbfd03148);

	asm volatile ("1: b 1b");
}
