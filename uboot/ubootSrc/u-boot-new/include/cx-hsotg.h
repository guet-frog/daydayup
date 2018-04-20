/* arch/mips/clouder/include/mach/udec-hs.h
 *
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *      Ben Dooks <ben@simtec.co.uk>
 *      http://armlinux.simtec.co.uk/
 *
 * S3C USB2.0 High-speed / OtG platform information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __LINUX_CX_HOSTOTG_H
#define __LINUX_CX_HOSTOTG_H

enum cx_hsotg_dmamode {
	CX_HSOTG_DMA_NONE,	/* do not use DMA at-all */
	CX_HSOTG_DMA_ONLY,	/* always use DMA */
	CX_HSOTG_DMA_DRV,	/* DMA is chosen by driver */
};

enum {
	USB_PHYIF_8,
	USB_PHYIF_16,
	USB_PHYIF_DEF = USB_PHYIF_16,
};

typedef enum {
	HS_UTMI_INTERFACE,
	HS_ULPI_INTERFACE,
} hs_phy_t;

enum {
	USB_FORCE_NONE = 0,
	USB_FORCE_HOST = 1<<29,
	USB_FORCE_DEVICE = 1<<30
};

/**
 * struct cx_hsotg_plat - platform data for high-speed otg/udc
 * @dma: Whether to use DMA or not.
 * @is_osc: The clock source is an oscillator, not a crystal
 * @phy_type: ulpi 0r utmi.
 * @phyif: phyif.
 * @hs_psel: hs phy sel, 1: full speed, 0: high speed
 * @has_regulator: regulator need.
 */
struct cx_hsotg_plat {
	void *mmap;
	enum cx_hsotg_dmamode	dma;
	unsigned int		is_osc : 1;
	hs_phy_t phy_type;
	u32	phyif;
	uint32_t force;

	int (*phy_init)(int type);
	int (*phy_exit)(int type);
};

#endif /* __LINUX_CX_HOSTOTG_H */
