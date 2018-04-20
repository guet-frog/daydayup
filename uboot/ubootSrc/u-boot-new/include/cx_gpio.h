/* 
 * Copyright (c) 2014-2018, Shanghai SemiClouder Ltd. All rights reserved.
 *
 * Author:
 *	Warnus   <huxianan@cloudersemi.com>
 *       
 *  Revision History: 
 *  ----------------- 
 *  1.1  XXX 12/29/2014 XXX      Initialized by warnus
 *  
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details
 *
 */
#ifndef __GPIO_H
#define __GPIO_H

#include <asm-generic/gpio.h> 
#include <linux/list.h>

#define	DWAPB_GPIO_SWPORTA_DR 0x00
#define DWAPB_GPIO_SWPORTA_DRR 0x04
#define DWAPB_GPIO_SWPORTA_CTL 0x08
#define DWAPB_GPIO_SWPORTB_DR 0x0c
#define DWAPB_GPIO_SWPORTB_DRR 0x10
#define DWAPB_GPIO_SWPORTB_CTL 0x14
#define DWAPB_GPIO_SWPORTC_DR 0x18
#define DWAPB_GPIO_SWPORTC_DRR 0x1c
#define DWAPB_GPIO_SWPORTC_CTL 0x20
#define DWAPB_GPIO_SWPORTD_DR 0x24
#define DWAPB_GPIO_SWPORTD_DRR 0x28
#define DWAPB_GPIO_SWPORTD_CTL 0x2c
#define	DWAPB_GPIO_INTEN 0x30
#define	DWAPB_GPIO_INTMASK 0x34
#define	DWAPB_GPIO_INTTYPE_LEVEL 0x38
#define	DWAPB_GPIO_INT_POLARITY 0x3c
#define	DWAPB_GPIO_INTSTATUS 0x40
#define	DWAPB_GPIO_RAW_INTSTATUS 0x44
#define	DWAPB_GPIO_DEBOUNCE 0x48
#define	DWAPB_GPIO_PORTA_EOI 0x4c
#define DWAPB_GPIO_EXT_PORTA 0x50
#define DWAPB_GPIO_EXT_PORTB 0x54
#define DWAPB_GPIO_EXT_PORTC 0x58
#define DWAPB_GPIO_EXT_PORTD 0x5c
#define	DWAPB_GPIO_LS_SYNC 0x60
#define	DWAPB_GPIO_ID_CODE 0x64
#define	DWAPB_GPIO_VER_ID_CODE 0x6c
#define	DWAPB_GPIO_CONFIG_REG1 0x74
#define	DWAPB_GPIO_CONFIG_REG2 0x70

#define DWAPB_PORTA_SINGLE_CTL (1 << 4)
#define DWAPB_PORTB_SINGLE_CTL (1 << 5)
#define DWAPB_PORTC_SINGLE_CTL (1 << 6)
#define DWAPB_PORTD_SINGLE_CTL (1 << 7)

#define DWAPB_HW_PORTA (1 << 8)
#define DWAPB_HW_PORTB (1 << 9)
#define DWAPB_HW_PORTC (1 << 10)
#define DWAPB_HW_PORTD (1 << 11)

#define DWAPB_GPIO_CONFIG_DEBOUNCE (1 << 13)
#define DWAPB_GPIO_CONFIG_REG1_PORTA_INTR (1 << 12)

#define DWAPB_GPIO_PN_MASK(x) (((x >> 2) & 0x3) + 1)

#define ENCODED_ID_PWIDTH_A(x) (((x >> 0) & 0x1f) + 1)
#define ENCODED_ID_PWIDTH_B(x) (((x >> 5) & 0x1f) + 1)
#define ENCODED_ID_PWIDTH_C(x) (((x >> 10) & 0x1f) + 1)
#define ENCODED_ID_PWIDTH_D(x) (((x >> 15) & 0x1f) + 1)

#define GPIO_BIT_MASK(x) ((x) & (~(0x1f << 8)))
#define GPIO_BIT(x) (((x) >> 8) & 0x1f)
		
#define BIT(x)			(1 << (x))

struct cx_bank_reg {               
	uint8_t  dr;
	uint8_t  ddr;
	uint8_t  ctl;
	uint8_t  ext_port;
};

struct cx_gpio_platdata {
	phys_addr_t io;
	int chip_id;
	int max_banks;
	struct cx_bank_reg *porta;
	struct cx_bank_reg *portb;
	struct cx_bank_reg *portc;
	struct cx_bank_reg *portd;
};

struct cx_gpio_bank {
	struct cx_bank_reg *port;
	char *label;
	unsigned base;

	u8 ngpio;
	struct gpio_desc *desc;

	struct cx_gpio_chip *chip;
};

struct cx_gpio_chip {
	void *mmap;
	struct cx_gpio_platdata *pdata;

	int idx;
	unsigned char isup;

	char *label;
	unsigned base;
	u16 ngpio;

	struct list_head node;

	int nbank;
	struct cx_gpio_bank *banks;
};

#endif
