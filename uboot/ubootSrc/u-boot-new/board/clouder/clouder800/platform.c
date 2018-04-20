/*
 * (C) Copyright 2007
 * Vlad Lungu vlad.lungu@windriver.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm-generic/gpio.h>
#include <asm/io.h>
#include <cx_gpio_map.h>
#include <bootargs.h>
extern int cx_gpio_init(struct cx_gpio_platdata **pdata, int ngpio);

#ifdef CONFIG_CX_UDC
#include <cx-hsotg.h>

extern int cx_high_speed_otg_init(struct cx_hsotg_plat *plat);

static int udc_phy_init(int type)
{
	return 0;
}

static int udc_phy_exit(int type)
{
	return 0;
}

/* hign speed otg platform resource */
struct cx_hsotg_plat udc_plat = {
	.mmap = (void*)CONFIG_CLOUDER_USB_BASE,
	.dma = CX_HSOTG_DMA_NONE,
	.is_osc = 0,
	.phy_type = HS_UTMI_INTERFACE,
	.phyif = USB_PHYIF_16,
	.force = USB_FORCE_DEVICE,
	.phy_init = udc_phy_init,
	.phy_exit = udc_phy_exit
};
#endif

#ifdef CONFIG_CLOUDER_MMC 
#include <mci.h>
#include <mmc.h>

extern int clouder_mmc_init(struct mci_host **host, int mmax);

#define CLOUDER_MMC_BOARD_CONFIG(_channel, _idmac, mode) \
	struct mci_host clouder_mmc##_channel##_bcfg = { \
		.name = "sdmmc", \
		.ioaddr = (void*)(CONFIG_CLOUDER_MMC##_channel##_BASE), \
		.use_idmac = _idmac, \
		.clksrc = 0,    \
		.bus_hz = 25000000, /* 50MHZ */ \
		.dev_id = _channel,  \
		.buswidth = mode,  \
	};

CLOUDER_MMC_BOARD_CONFIG(0, 1, MMC_MODE_4BIT);
CLOUDER_MMC_BOARD_CONFIG(1, 1, MMC_MODE_4BIT);
CLOUDER_MMC_BOARD_CONFIG(2, 1, MMC_MODE_8BIT);

struct mci_host *host[] = {
	&clouder_mmc2_bcfg,
	&clouder_mmc1_bcfg,
	&clouder_mmc0_bcfg,
};
#endif

#ifdef CONFIG_CX_UDC_DMA

#include <usb/cx_udc.h>

extern int cx_udc_init(struct cx_plat_otg_data *pdata);

struct cx_plat_otg_data  otg_plat = {
	.regs_otg = CONFIG_CLOUDER_USB_BASE
};
#endif

phys_size_t initdram(int board_type)
{
	return CONFIG_SYS_SDRAM_SIZE;
}

int checkboard(void)
{
	return 0;
}


int misc_init_r(void)
{
	set_io_port_base(0);
	return 0;
}

int board_mmc_init(bd_t *bis)
{
#ifdef CONFIG_CLOUDER_MMC
	clouder_mmc_init(host, 3);
#endif

	return 0;
}

int usb_gadget_init(void)
{
#ifdef CONFIG_CX_UDC
	cx_high_speed_otg_init(&udc_plat);
#endif

#ifdef CONFIG_CX_UDC_DMA
	cx_udc_init(&otg_plat);
#endif

	return 0;
}

#ifdef CONFIG_FASTBOOT
int g_dnl_bind_fixup(struct usb_device_descriptor *dev, const char *name) 
{
	g_dnl_set_serialnumber("mips");

	return 0;
}

int g_dnl_board_usb_cable_connected(void) 
{
	return usb_gadget_cable_connected();
}
#endif

#ifdef CONFIG_CLOUDER_GPIO
#include <cx_gpio.h>

struct cx_bank_reg swporta = {              
	.dr             = DWAPB_GPIO_SWPORTA_DR,     
	.ddr            = DWAPB_GPIO_SWPORTA_DRR,    
	.ctl            = DWAPB_GPIO_SWPORTA_CTL,    
	.ext_port       = DWAPB_GPIO_EXT_PORTA       
};                                                   

struct cx_bank_reg swportb = {              
	.dr             = DWAPB_GPIO_SWPORTB_DR,     
	.ddr            = DWAPB_GPIO_SWPORTB_DRR,              
	.ctl            = DWAPB_GPIO_SWPORTB_CTL,    
	.ext_port       = DWAPB_GPIO_EXT_PORTB       
};                                                   

struct cx_bank_reg swportc = {              
	.dr             = DWAPB_GPIO_SWPORTC_DR,     
	.ddr            = DWAPB_GPIO_SWPORTC_DRR,    
	.ctl            = DWAPB_GPIO_SWPORTC_CTL,    
	.ext_port       = DWAPB_GPIO_EXT_PORTC       
};                                                   

struct cx_bank_reg swportd = {              
	.dr             = DWAPB_GPIO_SWPORTD_DR,     
	.ddr            = DWAPB_GPIO_SWPORTD_DRR,    
	.ctl            = DWAPB_GPIO_SWPORTD_CTL,    
	.ext_port       = DWAPB_GPIO_EXT_PORTD       
};

#define CX_GPIO_PLATDATA(_io, _id, _max, _a, _b, _c, _d)		\
	static struct cx_gpio_platdata cx_gpio##_id##_platdata = {	\
		.io		=	_io,				\
		.chip_id	=	_id,				\
		.max_banks	=	_max,				\
		.porta		=	_a,				\
		.portb		=	_b,				\
		.portc		=	_c,				\
		.portd		=	_d,				\
	};								\

CX_GPIO_PLATDATA(0x1FD04000, 0, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD05000, 1, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD06000, 2, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD07000, 3, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD3B000, 4, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD3C000, 5, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD3D000, 6, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD3E000, 7, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD3F000, 8, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD40000, 9, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD44000, 10, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD45000, 11, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD46000, 12, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD47000, 13, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD48000, 14, 4, &swporta, &swportb, &swportc, &swportd);
CX_GPIO_PLATDATA(0x1FD49000, 15, 4, &swporta, &swportb, &swportc, &swportd);

struct cx_gpio_platdata *cx_gpio_pdata[] = {
	&cx_gpio0_platdata,
	&cx_gpio1_platdata,
	&cx_gpio2_platdata,
	&cx_gpio3_platdata,
	&cx_gpio4_platdata,
	&cx_gpio5_platdata,
	&cx_gpio6_platdata,
	&cx_gpio7_platdata,
	&cx_gpio8_platdata,
	&cx_gpio9_platdata,
	&cx_gpio10_platdata,
	&cx_gpio11_platdata,
	&cx_gpio12_platdata,
	&cx_gpio13_platdata,
	&cx_gpio14_platdata,
	&cx_gpio15_platdata
};

int board_gpio_init(void)
{
	return cx_gpio_init(cx_gpio_pdata, ARRAY_SIZE(cx_gpio_pdata));
}

#endif

#if defined(CONFIG_PINCTRL_CX800) && defined(CONFIG_PINCTRL)

#include <linux/pinctrl/machine.h>
#include <linux/pinctrl/consumer.h>
#include <linux/sizes.h>
#include <cx_pmx.h>

static struct pinctrl_map mapping[] = {
        PIN_MAP_MUX_GROUP("uart.0", PINCTRL_STATE_DEFAULT, "pincx", "uart0grp", "uart"),
        PIN_MAP_MUX_GROUP("i2c.0", PINCTRL_STATE_DEFAULT, "pincx", "i2c0grp", "i2c"),
        PIN_MAP_MUX_GROUP("mmc.0", PINCTRL_STATE_DEFAULT, "pincx", "mmc04bgrp", "mmc"),
        PIN_MAP_MUX_GROUP("mmc.1", PINCTRL_STATE_DEFAULT, "pincx", "mmc1grp", "mmc"),
        PIN_MAP_MUX_GROUP("mmc.2", PINCTRL_STATE_DEFAULT, "pincx", "mmc28bgrp", "mmc"),
        PIN_MAP_MUX_GROUP("spi.0", PINCTRL_STATE_DEFAULT, "pincx", "spi0grp", "spi"),
        PIN_MAP_MUX_GROUP("pwm.0", PINCTRL_STATE_DEFAULT, "pincx", "pwm0grp", "pwm"),
	PIN_MAP_MUX_GROUP("clouder_i80", PINCTRL_STATE_DEFAULT, "pincx", "i80grp", "i80"),
	PIN_MAP_MUX_GROUP("csi.0", PINCTRL_STATE_DEFAULT, "pincx", "csigrp", "csi"),
};

static void map_default_pinctrl(const char *dev)
{
        pinctrl_get_select(dev, PINCTRL_STATE_DEFAULT);
}

struct cx_pmx_platdata cx800_pmx = {
	.pinctrl = "pincx",
	.start = 0x1FD03000,
	.physize = SZ_4K,
};

int board_pinctrl_init(void)
{
	int ret;

	ret = pinctrl_register_mappings(mapping, ARRAY_SIZE(mapping));
	if (ret) {
		printf("failed to register pinctrl mappings\n");
		return -1;
	}

	ret = clouder_pmx_init(&cx800_pmx);
	if (ret) {
		printf("board pinctrl [un]registered\n");
		return -1;
	}

	map_default_pinctrl(cx800_pmx.pinctrl);

	return 0;
}

#endif


#ifdef CONFIG_CX_I2C

#include <cx_i2c.h>

#define I2C_BOARD_INFO(__bus, __mmap, __clock) \
	struct i2c_board_info cx_i2c##__bus##_info = { \
		.mmap	= __mmap,			\
		.bus	= __bus,			\
		.clock	= __clock,			\
	};

I2C_BOARD_INFO(0, CONFIG_SYS_I2C_BASE, 25000000);

int board_i2c_init(void)
{
	i2c_cx_bus_init(&cx_i2c0_info);

	return 0;
}


#endif

#ifdef CONFIG_CLOUDER_MCU_PROC
#include <part.h>
#include <mmc.h>

static block_dev_desc_t *dev_desc = NULL;

int board_load_mcu_proc_binary(void)
{
	struct mmc *mmc;
	disk_partition_t info;
	int err = 0;
	char *imgbin = (char*)CONFIG_CLOUDER_BINARY_SRAM;

	mmc = find_mmc_device(CONFIG_FASTBOOT_FLASH_MMC_DEV);
	if (!mmc) {
		printf("boota: cannot find '%d' mmc device\n", CONFIG_FASTBOOT_FLASH_MMC_DEV);
		return -1;
	}

	dev_desc = get_dev("mmc", CONFIG_FASTBOOT_FLASH_MMC_DEV);
	if (NULL == dev_desc) {
		printf("** Block device MMC %d not supported\n", CONFIG_FASTBOOT_FLASH_MMC_DEV);
		printf("invalid mmc device\n");
		return -1;

	}

	err = mmc_init(mmc);
	if (err) {
		printf("boot mmc init failed: err - %d\n", err);
		return -1;
	}

	err = get_partition_info_efi_by_name(dev_desc, "mcu", &info);
	if (err) {
		printf("cannot the mcu efi partition info");
		return -1;
	}

	printf("mcu partion start %lu, size %lu .......\n", info.start, info.size);

	if (mmc->block_dev.block_read(CONFIG_FASTBOOT_FLASH_MMC_DEV,
			info.start, 64, (void*)imgbin) < 0) {
		printf("loading mcu binary code failed.\n");
		return -1;
	}

	printf("mcu process running ...\n");
	/* mcu process running */
	writel(0x08000000, 0xbfd03050);
	writel(0x00040000, 0xbfd03060);

	return 0;
}

#endif

/* boot select, 1: recovery, 0: normal */
int board_check_boot_mode(void)
{
	int bootf = REBOOT_NORMAL, index, is_pwr = 0;
	u8 pwrmon = 0;
	char env_charger[] = BOOTARGS_CHARGERMODE;

#if 0 /* Charging Mode */
	if((bootargs->reserved[1] != REBOOT_CANCELCHARGE) && (bootargs->reserved[1] != REBOOT_RECOVERY)) {
		if(g_dnl_board_usb_cable_connected()){
			setenv ("bootargs",env_charger);
		}
        }
#endif
	i2c_read(CONFIG_SYS_I2C_SLAVE, 0x14, 1, &pwrmon, 1);
	if ((pwrmon & 0x1) == 0x1) /* power key down */
	{
		for(index = 0; index < 10; index ++)
		{
			mdelay(200);
			i2c_read(CONFIG_SYS_I2C_SLAVE, 0x14, 1, &pwrmon, 1);
			if ((pwrmon & 0x1) == 0x0) {
				is_pwr = 0x1;
				break;
			}
		}
		if(is_pwr != 0x1) {
			bootf = REBOOT_FASTBOOT;
			return bootf;
		}
	}

	switch(bootargs->reserved[1]) {
	case REBOOT_FASTBOOT:
		bootf = REBOOT_FASTBOOT; 
		break;
	case REBOOT_RECOVERY:
		bootf = REBOOT_RECOVERY;
		break;
	case REBOOT_NORMAL:
	default:
		break;
	}
	bootargs->reserved[1] = (u8)REBOOT_NORMAL;
	if (bootf != REBOOT_NORMAL)
		return bootf;
	
	debug("boot mode checking ...\n");

	if (i2c_set_bus_num(CONFIG_PMU_BUS))
		return bootf;

	if (i2c_read(CONFIG_SYS_I2C_SLAVE, 0x14, 1, &pwrmon, 1)) {
		debug("pmu attach i2c.%d pwrmon %02x\n", 
				CONFIG_PMU_BUS, pwrmon);
		if (!(pwrmon & 0x1)) /* power key down */
			return bootf;
	}
	printf("pads power key detect ...\n");

	if (!gpio_request(GPIO7_A4, "gpio")) {
		if (gpio_direction_input(GPIO7_A4))
			return bootf;
	}

	/* recovery mode detect */
	if (!gpio_get_value(GPIO7_A4)) {
		printf("droid recovery mode.\n");
		bootf = REBOOT_RECOVERY;
	}
	return bootf;
}
