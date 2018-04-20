/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

/*
 * This file contains the configuration parameters for qemu-mips target.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_MISC_INIT_R

/* is created by U-Boot showing the U-Boot version */
#define CONFIG_VERSION_VARIABLE

#define CONFIG_BOOTDELAY	0	/* autoboot after 10 seconds */
#define CONFIG_BAUDRATE		115200

#define BOOTARGS_CHARGERMODE "androidboot.mode=charger console=ttyS0,115200 root=/dev/ram0 rw rdinit=/init"
#define BOOTARGS_COMMON "console=ttyS0,115200 root=/dev/ram0 rw rdinit=/init"
#define CONFIG_BOOTARGS BOOTARGS_COMMON

#define CONFIG_TIMESTAMP		/* Print image info with timestamp */

#define CONFIG_BOOTCOMMAND "boota mmc0\0" 
//#define CONFIG_BOOTCOMMAND "fastboot\0" 
/*
 * for android
 */
#define CONFIG_ANDROID_BOOT_IMAGE

/*
 * Command line configuration.
 */
#define CONFIG_CMD_ELF
#define CONFIG_CMD_BOOTA
#define CONFIG_CMD_LOADB
#define CONFIG_CMD_MEMORY /* MEM test */
#define	CONFIG_CMD_FAT
#define CONFIG_CMD_MMC
#define CONFIG_CMD_EXT4
#define CONFIG_FAT_WRITE 
#define	CONFIG_DOS_PARTITION
#define HAVE_BLOCK_DEVICE
#define CONFIG_EFI_PARTITION
#define CONFIG_BOOTP_PXE
#define CONFIG_RANDOM_UUID
#define CONFIG_PARTITION_UUIDS
#define CONFIG_CMD_GPT
#define CONFIG_LIB_RAND

#define CONFIG_PARTS_DEFAULT \
    "name=mcu,size=10M;" \
    "name=boot,size=40M;" \
    "name=recovery,size=40M;" \
    "name=misc,size=60M;" \
    "name=cache,size=600M;" \
    "name=system,size=1024M;" \
    "name=private,size=8M;" \
    "name=userdata,size=-" \

/* File system Support */
#define CONFIG_FS_EXT4
#define CONFIG_EXT4_WRITE

/* Monitor Command Prompt */
#define CONFIG_SYS_PROMPT		"clouder$ " 


#define CONFIG_SKIP_LOWLEVEL_INIT

#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_HUSH_PARSER

/* Console I/O Buffer Size */
#define CONFIG_SYS_CBSIZE		256
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
/* max number of command args */
#define CONFIG_SYS_MAXARGS		16

#define CONFIG_SYS_MHZ			800

#define CONFIG_SYS_MIPS_TIMER_FREQ	(CONFIG_SYS_MHZ * 1000000)

#define CONFIG_SYS_HZ			1000

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CONFIG_SYS_DCACHE_BASE          0x80000000
#define CONFIG_SYS_DCACHE_SIZE		    32768
#define CONFIG_SYS_ICACHE_SIZE		    32768
#define CONFIG_SYS_CACHELINE_SIZE	    32

#define  CONFIG_SYS_NO_FLASH

/* default load address */
#define CONFIG_SYS_LOAD_ADDR		0x83000000

/* Memory BASE ADDR */
#define CONFIG_SYS_SDRAM_BASE		0x80000000
#define CONFIG_SYS_INIT_SP_OFFSET	0x2000000 /* 16M */

/* SDRAM SIZE */
#define CONFIG_SYS_SDRAM_SIZE   (256 * 1024 * 1024) /* 256MB */


/* DDR TEST */
//#define CONFIG_DDR_HDR_TEST 0
#define CONFIG_SYS_MEMTEST_START    0x80100000
#define CONFIG_SYS_MEMTEST_END      0x80800000

#define CONFIG_SYS_MALLOC_LEN		(1 * 1024 * 1024)
#define CONFIG_SYS_BOOTPARAMS_LEN	(128 * 1024)

/*------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_SYS_LOAD_BASE        0x80000000
#define CONFIG_SYS_TEXT_BASE        0x80000000
#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_MONITOR_LEN		(512 << 10)

#define CONFIG_ENV_IS_NOWHERE 1
/* Address and size of Primary Environment Sector */
#define CONFIG_ENV_SIZE		            0x8000
#define CONFIG_ENV_OVERWRITE	        1

/*------------------------------------------------------
 * MMC CONFIG
 *---------------------------------------------------- */
//#define CONFIG_MMC_TRACE
//#define DEBUG_MSHC
#define	CONFIG_MMC
#define	CONFIG_GENERIC_MMC
#define CONFIG_CLOUDER_MMC
#define CONFIG_CLOUDER_MMC0_BASE        0xbfd52000
#define CLOUDER_USE_MMC0
#define CONFIG_CLOUDER_MMC1_BASE        0xbfd53000
#define CONFIG_CLOUDER_MMC2_BASE        0xbfd54000
#define CLOUDER_USE_MMC2

#define CONFIG_SUPPORT_EMMC_BOOT
 /*------------------------------------------------------
 * LCD CONFIG
 *---------------------------------------------------- */
#define CONFIG_CLOUDER_LCDC_BASE        0xbfd55000
#define CONFIG_CLOUDER_LCD
/*------------------------------------------------------
 * UART CONFIG
 *---------------------------------------------------- */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_CONS_INDEX		        1

#define CONFIG_SYS_NS16550_REG_SIZE   (-4)
#define CONFIG_SYS_NS16550_CLK        (1843200 * 13)
#define CONFIG_SYS_NS16550_COM1       0xbfd00000

/*------------------------------------------------------
 * NAND CONFIG
 *---------------------------------------------------- */
//#define CONFIG_CMD_NAND
#define CONFIG_SYS_MAX_NAND_DEVICE  1

/* NANDC CONFIG */
#define CONFIG_NAND_CLOUDER_DNL
#define CONFIG_DNL_MAX_BANK 2

#define CONFIG_NAND_FLASH_REG 0xbfd56000
#define CONFIG_NAND_FLASH_RAM 0xbfcc0000
#define CONFIG_NAND_DMA_BUFFER 0x85000000

/* SYS NAND BASE */
#define CONFIG_SYS_NAND_BASE ((1UL << 32) - 1)


/* gpio cfg */
#define CONFIG_CLOUDER_GPIO

/* designware I2C */
#define CONFIG_CX_I2C
#define CONFIG_SYS_I2C_BASE  0x1FD08000
#define CONFIG_PMU_BUS 0
#define CONFIG_SYS_I2C_SLAVE 0x32

/* motor control*/
#define CONFIG_MOTOR

/* pinctrl */
#define CONFIG_PINCTRL
#define CONFIG_PINCTRL_CX800

/*
 * ------------------ Fastboot ---------------------
 */
#define CONFIG_CLOUDER_USB_BASE 0xbfd80000
#define CONFIG_FASTBOOT
#define CONFIG_USB_GADGET
#define CONFIG_USBDOWNLOAD_GADGET
#define CONFIG_DFU_FUNCTION
#define CONFIG_CMD_FASTBOOT
#define CONFIG_FASTBOOT_FLASH
#define CONFIG_FASTBOOT_FLASH_MMC_DEV 0
#define CONFIG_USB_DEVICE
#define CONFIG_CMD_DFU
#define CONFIG_CMD_HASH
#define CONFIG_CX_UDC_DMA
#define CONFIG_USB_GADGET_DUALSPEED
#define CONFIG_USB_GADGET_VBUS_DRAW 2
#define CONFIG_G_DNL_MANUFACTURER "MIPS-clouder"
#define CONFIG_G_DNL_PRODUCT_NUM 0x6601
#define CONFIG_G_DNL_VENDOR_NUM 0x04E8
#define CONFIG_USB_FASTBOOT_BUF_ADDR 0x83000000
#define CONFIG_USB_FASTBOOT_BUF_SIZE 0x07000000

/*
 * Android Recovery infomation
 */
#define CONFIG_ANDROID_CACHE_PARTITION_MMC "cache"
#define CONFIG_ANDROID_RECOVERY_CMD_FILE "/recovery/command"

/*
 * MCU configuration
 */
#define CONFIG_CLOUDER_MCU_PROC
#define CONFIG_CLOUDER_BINARY_SRAM 0xbfd10000

#endif /* __CONFIG_H */
