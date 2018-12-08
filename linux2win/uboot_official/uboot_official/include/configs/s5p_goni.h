/*
 * Copyright (C) 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 * Kyungmin Park <kyungmin.park@samsung.com>
 *
 * Configuation settings for the SAMSUNG Universal (s5pc100) board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* High Level Configuration Options */
#define CONFIG_SAMSUNG		1	/* in a SAMSUNG core */
#define CONFIG_S5P		1	/* which is in a S5P Family */
#define CONFIG_S5PC110		1	/* which is in a S5PC110 */
#define CONFIG_MACH_GONI	1	/* working with Goni */

#include <asm/arch/cpu.h>		/* get chip and board defs */

#define CONFIG_ARCH_CPU_INIT
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/* input clock of PLL: has 24MHz input clock at S5PC110 */
//#define CONFIG_SYS_CLK_FREQ_C110	24000000

/* DRAM Base */
//#define CONFIG_SYS_SDRAM_BASE		0x30000000

/* Text Base */
//#define CONFIG_SYS_TEXT_BASE		0x34800000

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_EDITING

/*
 * Size of malloc() pool
 * 1MB = 0x100000, 0x100000 = 1024 * 1024
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (1 << 20))
/*
 * select serial console configuration
 */
#define CONFIG_SERIAL2			1	/* use SERIAL2 */
#define CONFIG_BAUDRATE			115200

/* MMC */
//#define CONFIG_GENERIC_MMC
//#define CONFIG_MMC
////#define CONFIG_SDHCI
////#define CONFIG_S5P_SDHCI

/* PWM */
#define CONFIG_PWM			1

/* It should define before config_cmd_default.h */
#define CONFIG_SYS_NO_FLASH		1

/* Command definition */
#include <config_cmd_default.h>

#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_MISC
#undef CONFIG_CMD_NET
#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_XIMG
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_REGINFO
//#define CONFIG_CMD_ONENAND
#define CONFIG_CMD_MTDPARTS
#define CONFIG_CMD_MMC

#define CONFIG_BOOTDELAY		1
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS

/* - macro define for env */
#define MTDPARTS_DEFAULT	"mtdparts=samsung-onenand:1m(bootloader)"\
				",256k(params)"\
				",2816k(config)"\
				",8m(csa)"\
				",7m(kernel)"\
				",1m(log)"\
				",12m(modem)"\
				",60m(qboot)"\
				",-(UBI)\0"

#define NORMAL_MTDPARTS_DEFAULT MTDPARTS_DEFAULT

#define CONFIG_BOOTCOMMAND	"run ubifsboot"

#define CONFIG_DEFAULT_CONSOLE	"console=ttySAC2,115200n8\0"

#define CONFIG_RAMDISK_BOOT	"root=/dev/ram0 rw rootfstype=ext2" \
		" ${console} ${meminfo}"

#define CONFIG_COMMON_BOOT	"${console} ${meminfo} ${mtdparts}"

#define CONFIG_BOOTARGS	"root=/dev/mtdblock8 ubi.mtd=8 ubi.mtd=3 ubi.mtd=6" \
		" rootfstype=cramfs " CONFIG_COMMON_BOOT
/* - end of macro define */

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser	*/
#define CONFIG_SYS_PROMPT	"Goni # "
#define CONFIG_SYS_CBSIZE	256	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE	384	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16	/* max number of command args */
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + 0x5000000)
#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x4000000)

#define CONFIG_SYS_HZ			1000

/* FLASH and environment organization */
//#define CONFIG_ENV_IS_IN_ONENAND	1
#define CONFIG_ENV_IS_IN_ONENAND  	0
#define CONFIG_ENV_IS_IN_MMC		1	// -- 等效于#define CONFIG_ENV_IS_IN_MMC

// CONFIG_ENV_IS_IN_ONENAND=0
// CONFIG_ENV_IS_IN_MMC=y		-- not 1

#define CONFIG_SYS_MMC_ENV_DEV	0

//#define CONFIG_ENV_SIZE			(256 << 10)	/* 256 KiB, 0x40000 */
#define CONFIG_ENV_SIZE			0x4000	/* - 16KB, 32blk */
#define CONFIG_ENV_ADDR			(1 << 20)	/* 1 MB, 0x100000 */

#define CONFIG_USE_ONENAND_BOARD_INIT
#define CONFIG_SAMSUNG_ONENAND		1
#define CONFIG_SYS_ONENAND_BASE		0xB0000000

#define CONFIG_DOS_PARTITION		1

//#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_LOAD_ADDR - 0x1000000)

#define CONFIG_SYS_CACHELINE_SIZE       64

#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_MAX8998

#include <asm/arch/gpio.h>
/*
 * I2C Settings
 */
#define CONFIG_SOFT_I2C_GPIO_SCL s5pc110_gpio_get_nr(j4, 3)
#define CONFIG_SOFT_I2C_GPIO_SDA s5pc110_gpio_get_nr(j4, 0)

#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_SOFT		/* I2C bit-banged */
#define CONFIG_SYS_I2C_SOFT_SPEED	50000
#define CONFIG_SYS_I2C_SOFT_SLAVE	0x7F
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_SYS_MAX_I2C_BUS	7
#define CONFIG_USB_GADGET
#define CONFIG_USB_GADGET_S3C_UDC_OTG
#define CONFIG_USB_GADGET_DUALSPEED

// ----------------------------------------------------------------------
/* - common board config */
#define CONFIG_EVT1
#define CONFIG_MCP_SINGLE
/* - end of common board config */

/* clk config */
/* - input clk */
#define CONFIG_SYS_CLK_FREQ_C110	24000000

#define CONFIG_CLK_1000_200_166_133
/* end of clk config */

/* sdram config */
#if defined(CONFIG_CLK_533_133_100_100)
#if defined(CONFIG_MCP_SINGLE)
#define DMC0_TIMINGA_REF	0x40e
#define DMC0_TIMING_ROW		0x10233206
#define DMC0_TIMING_DATA	0x12130005
#define	DMC0_TIMING_PWR		0x0E100222

#define DMC1_TIMINGA_REF	0x40e
#define DMC1_TIMING_ROW		0x10233206
#define DMC1_TIMING_DATA	0x12130005
#define	DMC1_TIMING_PWR		0x0E100222
#else /* CONFIG_MCP_SINGLE */
#error "You should define memory type (AC type or H type or B type)"
#endif /* CONFIG_MCP_SINGLE */
#elif defined(CONFIG_CLK_800_200_166_133) || \
	defined(CONFIG_CLK_1000_200_166_133) || \
	defined(CONFIG_CLK_800_100_166_133) || \
	defined(CONFIG_CLK_400_200_166_133) || \
	defined(CONFIG_CLK_400_100_166_133) /* CONFIG_CLK_533_133_100_100 */
#if defined(CONFIG_MCP_SINGLE)
#define DMC0_MEMCONTROL		0x00212400	// MemControl	BL=4, 1Chip, DDR2 Type, dynamic self refresh, force precharge, dynamic power down off
#define DMC0_MEMCONFIG_0	0x30F01313	// MemConfig0	256MB config, 8 banks,Mapping Method[12:15]0:linear, 1:linterleaved, 2:Mixed
#define DMC0_MEMCONFIG_1	0x40F01313	// MemConfig1
#define DMC0_TIMINGA_REF        0x00000618      // TimingAref   7.8us*133MHz=1038(0x40E), 100MHz=780(0x30C), 20MHz=156(0x9C), 10MHz=78(0x4E)
#define DMC0_TIMING_ROW         0x28233287      // TimingRow    for @200MHz
#define DMC0_TIMING_DATA        0x23240304      // TimingData   CL=3
#define DMC0_TIMING_PWR         0x09C80232      // TimingPower

#define	DMC1_MEMCONTROL		0x00202400	// MemControl	BL=4, 2 chip, DDR2 type, dynamic self refresh, force precharge, dynamic power down off
#define DMC1_MEMCONFIG_0	0x40F01313	// MemConfig0	512MB config, 8 banks,Mapping Method[12:15]0:linear, 1:linterleaved, 2:Mixed
#define DMC1_MEMCONFIG_1	0x00F01313	// MemConfig1
#define DMC1_TIMINGA_REF        0x00000618      // TimingAref   7.8us*133MHz=1038(0x40E), 100MHz=780(0x30C), 20MHz=156(0x9C), 10MHz=78(0x4E)
#define DMC1_TIMING_ROW         0x28233289      // TimingRow    for @200MHz
#define DMC1_TIMING_DATA        0x23240304      // TimingData   CL=3
#define DMC1_TIMING_PWR         0x08280232      // TimingPower
#if defined(CONFIG_CLK_800_100_166_133) || defined(CONFIG_CLK_400_100_166_133)
#define DMC0_MEMCONFIG_0	0x20E01323	// MemConfig0	256MB config, 8 banks,Mapping Method[12:15]0:linear, 1:linterleaved, 2:Mixed
#define DMC0_MEMCONFIG_1	0x40F01323	// MemConfig1
#define DMC0_TIMINGA_REF	0x0000030C	// TimingAref	7.8us*133MHz=1038(0x40E), 100MHz=780(0x30C), 20MHz=156(0x9C), 10MHz=78(0x4E)
#define DMC0_TIMING_ROW		0x28233287	// TimingRow	for @200MHz
#define DMC0_TIMING_DATA	0x23240304	// TimingData	CL=3
#define	DMC0_TIMING_PWR		0x09C80232	// TimingPower

#define	DMC1_MEMCONTROL		0x00202400	// MemControl	BL=4, 2 chip, DDR2 type, dynamic self refresh, force precharge, dynamic power down off
#define DMC1_MEMCONFIG_0	0x40C01323	// MemConfig0	512MB config, 8 banks,Mapping Method[12:15]0:linear, 1:linterleaved, 2:Mixed
#define DMC1_MEMCONFIG_1	0x00E01323	// MemConfig1
#define DMC1_TIMINGA_REF	0x0000030C	// TimingAref	7.8us*133MHz=1038(0x40E), 100MHz=780(0x30C), 20MHz=156(0x9C), 10MHz=78(0x4
#define DMC1_TIMING_ROW		0x28233289	// TimingRow	for @200MHz
#define DMC1_TIMING_DATA	0x23240304	// TimingData	CL=3
#define	DMC1_TIMING_PWR		0x08280232	// TimingPower
#endif
#else /* CONFIG_MCP_SINGLE */
#error "You should define memory type (AC type or H type)"
#endif /* CONFIG_MCP_SINGLE */
#else /* CONFIG_CLK_533_133_100_100 */
#define DMC0_TIMINGA_REF	0x50e
#define DMC0_TIMING_ROW		0x14233287
#define DMC0_TIMING_DATA	0x12130005
#define	DMC0_TIMING_PWR		0x0E140222

#define DMC1_TIMINGA_REF	0x618
#define DMC1_TIMING_ROW		0x11344309
#define DMC1_TIMING_DATA	0x12130005
#define	DMC1_TIMING_PWR		0x0E190222
#endif /* CONFIG_CLK_533_133_100_100 */

/* DRAM banks && size config */
#define CONFIG_NR_DRAM_BANKS	2
#define PHYS_SDRAM_1			CONFIG_SYS_SDRAM_BASE
#define PHYS_SDRAM_1_SIZE		(256 << 20)		/* 256MB in Bank #0 */
#define PHYS_SDRAM_2			0x40000000		/* mDDR DMC1 Bank #1 */
#define PHYS_SDRAM_2_SIZE		(256 << 20)		/* 256 MB in Bank #1 */

#define CONFIG_SYS_MONITOR_BASE		0x00000000
#define CONFIG_SYS_MONITOR_LEN		(256 << 10)	/* 256 KiB */

/* - DRAM Base */
#define CONFIG_SYS_SDRAM_BASE		0x30000000

/* - Text Base */
#define CONFIG_SYS_TEXT_BASE		0x34800000

/* - stack addr init */
#define CONFIG_SYS_INIT_SP_ADDR	    0x34700000
/* end of sdram config */

/* - sd/mmc config */
#define CONFIG_GENERIC_MMC
#define CONFIG_MMC
//#define CONFIG_SDHCI
//#define CONFIG_S5P_SDHCI

/* - add config */
#define CONFIG_S3C_HSMMC

/* The macro for MMC channel 0 is defined by default and can't be undefined */
#define USE_MMC0
//#define USE_MMC0_8BIT //lqm added for test
#define USE_MMC2
//#define USE_MMC2_8BIT
#define MMC_MAX_CHANNEL		4

/* - env offset(byte) in sd/mmc */
#define CONFIG_ENV_OFFSET	(17 * 512)

/* - end of sd/mmc config */



#endif	/* __CONFIG_H */
