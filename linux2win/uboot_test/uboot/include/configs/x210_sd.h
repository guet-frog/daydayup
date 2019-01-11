/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <gj@denx.de>
 * David Mueller <d.mueller@elsoft.ch>
 *
 * Configuation settings for the SAMSUNG SMDK6400(mDirac-III) board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

//#define FPGA_SMDKC110
//定义CONFIG_CHECK_X210CV3，则为工装检测模式，不定义，为正常启动模式
//#define CONFIG_CHECK_X210CV3

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_MPAD		1		//lxg added
#define CONFIG_S5PC110		1		/* in a SAMSUNG S3C6410 SoC */
#define CONFIG_S5PC11X		1		/* in a SAMSUNG S3C64XX Family  */
#define CONFIG_X210		1

#define CONFIG_MCP_SINGLE	1
#define CONFIG_EVT1		1		/* EVT1 */
//#define CONFIG_SMDKV210_REV02	1	/* Rev 0.2 and PMIC Type is MAX8998 */

#define CONFIG_FASTBOOT		1
//#define CONFIG_FUSED		1		/* Fused chip */
//#define CONFIG_SECURE_BOOT	1		/* secure booting */

#define CONFIG_SW_WORKAROUND	1		/* Software around */
#if defined(CONFIG_SW_WORKAROUND)
#define CONFIG_CHECK_MPLL_LOCK	1		/* Check MPLL is locked */
#endif


#if defined(CONFIG_SECURE_BOOT)
#define CONFIG_SECURE_KERNEL_BASE	0x20008000
#define CONFIG_SECURE_KERNEL_SIZE	0x00271000
#define CONFIG_SECURE_ROOTFS_BASE	0x30A00000
#define CONFIG_SECURE_ROOTFS_SIZE	0x0013D000
#endif

#define BOOT_ONENAND		0x1
#define BOOT_NAND		0x2
#define BOOT_MMCSD		0x3
#define BOOT_NOR		0x4
#define BOOT_SEC_DEV		0x5

#define AT070TN92		1
#define VGA_800X600		2
#define VGA_1024X768	3
#define TRULY043		4
#define VGA_1440X900		5
#define VGA_1280X1024		6

#define DISP_MODE		AT070TN92
//#define DISP_MODE		VS070CXN
//#define DISP_MODE		TRULY043
//#define DISP_MODE		VGA_800X600
//#define DISP_MODE		VGA_1024X768
//#define DISP_MODE		VGA_1440X900
//#define DISP_MODE		VGA_1280X1024

/* skip to load BL2 */
//#define FAST_BOOT		1

#define MEMORY_BASE_ADDRESS	0x30000000
//#define MEMORY_BASE_ADDRESS	0x20000000
#define MEMORY_BASE_ADDRESS2	0x40000000

/* input clock of PLL */
#define CONFIG_SYS_CLK_FREQ	24000000	/* the SMDK6400 has 24MHz input clock */

#define CONFIG_ENABLE_MMU

#ifdef CONFIG_ENABLE_MMU
#define virt_to_phys(x)	virt_to_phy_smdkc110(x)
#else
#define virt_to_phys(x)	(x)
#endif

#define CONFIG_MEMORY_UPPER_CODE

#undef CONFIG_USE_IRQ				/* we don't need IRQ/FIQ stuff */

#define CONFIG_INCLUDE_TEST

#define CONFIG_ZIMAGE_BOOT
#define CONFIG_IMAGE_BOOT

#define BOARD_LATE_INIT

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG

/*
 * Architecture magic and machine type
 */

#define MACH_TYPE		2456

#define UBOOT_MAGIC		(0x43090000 | MACH_TYPE)

/* Power Management is enabled */
#define CONFIG_PM

#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

#undef CONFIG_SKIP_RELOCATE_UBOOT
#undef CONFIG_USE_NOR_BOOT

/*
 * Size of malloc() pool
 */
#define CFG_MALLOC_LEN		(CFG_ENV_SIZE + 896*1024)
#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

#define CFG_STACK_SIZE		512*1024

/*
 * Hardware drivers
 */
#define DM9000_16BIT_DATA

#define CONFIG_DRIVER_DM9000	1

#ifdef CONFIG_DRIVER_DM9000
#define CONFIG_DM9000_BASE		(0x88000300)
#define DM9000_IO			(CONFIG_DM9000_BASE)
#if defined(DM9000_16BIT_DATA)
#define DM9000_DATA			(CONFIG_DM9000_BASE+4)
#else
#define DM9000_DATA			(CONFIG_DM9000_BASE+1)
#endif
#endif
/*
 * select serial console configuration
 */
//#define CONFIG_SERIAL1          1	/* we use UART0 on SMDKC110 */
#define CONFIG_SERIAL3          1	/* we use UART2 on SMDKC110 */

#define CFG_HUSH_PARSER			/* use "hush" command parser	*/
#ifdef CFG_HUSH_PARSER
#define CFG_PROMPT_HUSH_PS2	"> "
#endif

#define CONFIG_CMDLINE_EDITING

#undef CONFIG_S3C64XX_I2C		/* this board has H/W I2C */
#ifdef CONFIG_S3C64XX_I2C
#define CONFIG_HARD_I2C		1
#define CFG_I2C_SPEED		50000
#define CFG_I2C_SLAVE		0xFE
#endif

#define CONFIG_DOS_PARTITION
#define CONFIG_SUPPORT_VFAT

#define CONFIG_USB_OHCI
#undef CONFIG_USB_STORAGE
#define CONFIG_S3C_USBD

#define USBD_DOWN_ADDR		0xc0000000

/************************************************************
 * RTC
 ************************************************************/
/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE		115200

/***********************************************************
 * Command definition
 ***********************************************************/
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_USB
#define CONFIG_CMD_REGINFO

//#define	CONFIG_CMD_NAND
#define	CONFIG_CMD_FLASH

#ifndef FPGA_SMDKC110
//#define CONFIG_CMD_ONENAND
#define CONFIG_CMD_MOVINAND
#endif
#define CONFIG_CMD_PING
#define CONFIG_CMD_DATE

#include <config_cmd_default.h>

#define CONFIG_CMD_ELF
#define CONFIG_CMD_DHCP
//#define CONFIG_CMD_I2C

#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_BOOTPATH

#define CONFIG_BOOTARGS    	"console=ttySAC2,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext3"
//#define CONFIG_BOOTARGS    	"console=ttySAC0,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext3"
#define CONFIG_ETHADDR		00:40:5c:26:0a:5b
#define CONFIG_NETMASK      	255.255.0.0
#define CONFIG_IPADDR		192.168.1.88
#define CONFIG_SERVERIP		192.168.1.102
#define CONFIG_GATEWAYIP	192.168.0.1

#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_NET_MULTI
#undef	CONFIG_NET_MULTI

#ifdef CONFIG_NET_MULTI
#define CONFIG_DRIVER_SMC911X_BASE	0x98800300
#define CONFIG_DRIVER_SMC911X_16_BIT
#define CONFIG_DRIVER_CS8900
#define CS8900_BASE	  		0x18800300
#define CS8900_BUS16
#else
#define CONFIG_DRIVER_SMC911X_16_BIT
#define CONFIG_DRIVER_SMC911X_BASE	0x98800300
#undef	CONFIG_DRIVER_CS8900
#endif

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	115200		/* speed to run kgdb serial port */
/* what's this ? it's not used anywhere */
#define CONFIG_KGDB_SER_INDEX	1		/* which serial port to use */
#endif

/*
 * Miscellaneous configurable options
 */
#define CFG_LONGHELP				/* undef to save memory		*/

#define CFG_PROMPT              "x210 # "   /* Monitor Command Prompt       */

#define CFG_CBSIZE		256		/* Console I/O Buffer Size	*/
#define CFG_PBSIZE		384		/* Print Buffer Size */
#define CFG_MAXARGS		16		/* max number of command args	*/
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size	*/

#define CFG_MEMTEST_START	MEMORY_BASE_ADDRESS	/* memtest works on	*/

#define CFG_MEMTEST_END		MEMORY_BASE_ADDRESS + 0x3E00000		/* 256 MB in DRAM	*/

#undef CFG_CLKS_IN_HZ		/* everything, incl board info, in Hz */

#define CFG_LOAD_ADDR		MEMORY_BASE_ADDRESS	/* default load address	*/

/* the PWM TImer 4 uses a counter of 41687 for 10 ms, so we need */
/* it to wrap 100 times (total 4168750) to get 1 sec. */
#define CFG_HZ			4168750		// at PCLK 66MHz

/* valid baudrates */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	0x40000		/* regular stack 256KB */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

//#define CONFIG_CLK_667_166_166_133
//#define CONFIG_CLK_533_133_100_100
//#define CONFIG_CLK_800_200_166_133
//#define CONFIG_CLK_800_100_166_133
#define CONFIG_CLK_1000_200_166_133
//#define CONFIG_CLK_400_200_166_133
//#define CONFIG_CLK_400_100_166_133

#if defined(CONFIG_CLK_667_166_166_133)
#define APLL_MDIV       0xfa
#define APLL_PDIV       0x6
#define APLL_SDIV       0x1
#elif defined(CONFIG_CLK_533_133_100_100)
#define APLL_MDIV       0x215
#define APLL_PDIV       0x18
#define APLL_SDIV       0x1
#elif defined(CONFIG_CLK_800_200_166_133) || \
	defined(CONFIG_CLK_800_100_166_133) || \
	defined(CONFIG_CLK_400_200_166_133) || \
	defined(CONFIG_CLK_400_100_166_133)
#define APLL_MDIV       0x64
#define APLL_PDIV       0x3
#define APLL_SDIV       0x1
#elif defined(CONFIG_CLK_1000_200_166_133)
#define APLL_MDIV       0x7d
#define APLL_PDIV       0x3
#define APLL_SDIV       0x1
#endif

//#define APLL_LOCKTIME_VAL	0x2cf
#define APLL_LOCKTIME_VAL	0xe10 //lxg changed.
#define MPLL_LOCKTIME_VAL	0xe10 //lxg added

#if defined(CONFIG_EVT1)
/* Set AFC value */
#define AFC_ON		0x00000000
#define AFC_OFF		0x10000010
#endif

#if defined(CONFIG_CLK_533_133_100_100)
#define MPLL_MDIV	0x190
#define MPLL_PDIV	0x6
#define MPLL_SDIV	0x2
#else
#define MPLL_MDIV	0x29b
#define MPLL_PDIV	0xc
#define MPLL_SDIV	0x1
#endif

#define EPLL_MDIV	0x60
#define EPLL_PDIV	0x6
#define EPLL_SDIV	0x2

#define VPLL_MDIV	0x6c
#define VPLL_PDIV	0x6
#define VPLL_SDIV	0x3

/* CLK_DIV0 */
#define APLL_RATIO	0
#define A2M_RATIO	4
#define HCLK_MSYS_RATIO	8
#define PCLK_MSYS_RATIO	12
#define HCLK_DSYS_RATIO	16
#define PCLK_DSYS_RATIO 20
#define HCLK_PSYS_RATIO	24
#define PCLK_PSYS_RATIO 28

#define CLK_DIV0_MASK	0x7fffffff
/*******lxg added**************/
#define CLK_DIV1_MASK	0xffffffff
#define CLK_DIV2_MASK	0x0fff
#define CLK_DIV3_MASK	0x7fffffff
#define CLK_DIV4_MASK	0xffffffff
#define CLK_DIV6_MASK	0xffffffff
/*******end**************/
#define set_pll(mdiv, pdiv, sdiv)	(1<<31 | mdiv<<16 | pdiv<<8 | sdiv)

#define APLL_VAL	set_pll(APLL_MDIV,APLL_PDIV,APLL_SDIV)
#define MPLL_VAL	set_pll(MPLL_MDIV,MPLL_PDIV,MPLL_SDIV)
#define EPLL_VAL	set_pll(EPLL_MDIV,EPLL_PDIV,EPLL_SDIV)
#define VPLL_VAL	set_pll(VPLL_MDIV,VPLL_PDIV,VPLL_SDIV)

#if defined(CONFIG_CLK_667_166_166_133)
#define CLK_DIV0_VAL    ((0<<APLL_RATIO)|(3<<A2M_RATIO)|(3<<HCLK_MSYS_RATIO)|(1<<PCLK_MSYS_RATIO)\
			|(3<<HCLK_DSYS_RATIO)|(1<<PCLK_DSYS_RATIO)|(4<<HCLK_PSYS_RATIO)|(1<<PCLK_PSYS_RATIO))
#elif defined(CONFIG_CLK_533_133_100_100)
#define CLK_DIV0_VAL    ((0<<APLL_RATIO)|(3<<A2M_RATIO)|(3<<HCLK_MSYS_RATIO)|(1<<PCLK_MSYS_RATIO)\
			|(3<<HCLK_DSYS_RATIO)|(1<<PCLK_DSYS_RATIO)|(3<<HCLK_PSYS_RATIO)|(1<<PCLK_PSYS_RATIO))
#elif defined(CONFIG_CLK_800_200_166_133)
#define CLK_DIV0_VAL    ((0<<APLL_RATIO)|(3<<A2M_RATIO)|(3<<HCLK_MSYS_RATIO)|(1<<PCLK_MSYS_RATIO)\
			|(3<<HCLK_DSYS_RATIO)|(1<<PCLK_DSYS_RATIO)|(4<<HCLK_PSYS_RATIO)|(1<<PCLK_PSYS_RATIO))
#elif defined(CONFIG_CLK_800_100_166_133)
#define CLK_DIV0_VAL    ((0<<APLL_RATIO)|(7<<A2M_RATIO)|(7<<HCLK_MSYS_RATIO)|(1<<PCLK_MSYS_RATIO)\
			|(3<<HCLK_DSYS_RATIO)|(1<<PCLK_DSYS_RATIO)|(4<<HCLK_PSYS_RATIO)|(1<<PCLK_PSYS_RATIO))
#elif defined(CONFIG_CLK_400_200_166_133)
#define CLK_DIV0_VAL    ((1<<APLL_RATIO)|(3<<A2M_RATIO)|(1<<HCLK_MSYS_RATIO)|(1<<PCLK_MSYS_RATIO)\
			|(3<<HCLK_DSYS_RATIO)|(1<<PCLK_DSYS_RATIO)|(4<<HCLK_PSYS_RATIO)|(1<<PCLK_PSYS_RATIO))
#elif defined(CONFIG_CLK_400_100_166_133)
#define CLK_DIV0_VAL    ((1<<APLL_RATIO)|(7<<A2M_RATIO)|(3<<HCLK_MSYS_RATIO)|(1<<PCLK_MSYS_RATIO)\
			|(3<<HCLK_DSYS_RATIO)|(1<<PCLK_DSYS_RATIO)|(4<<HCLK_PSYS_RATIO)|(1<<PCLK_PSYS_RATIO))			
#elif defined(CONFIG_CLK_1000_200_166_133)
#define CLK_DIV0_VAL    ((0<<APLL_RATIO)|(4<<A2M_RATIO)|(4<<HCLK_MSYS_RATIO)|(1<<PCLK_MSYS_RATIO)\
			|(3<<HCLK_DSYS_RATIO)|(1<<PCLK_DSYS_RATIO)|(4<<HCLK_PSYS_RATIO)|(1<<PCLK_PSYS_RATIO))
#endif

#define CLK_DIV1_VAL	((1<<16)|(1<<12)|(1<<8)|(1<<4))
#define CLK_DIV2_VAL	(1<<0)
#define CLK_DIV4_VAL	0x99990000
#define CLK_DIV6_VAL	0x71000

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


#if defined(CONFIG_CLK_533_133_100_100)
#define UART_UBRDIV_VAL		26
#define UART_UDIVSLOT_VAL	0x0808
#else
#define UART_UBRDIV_VAL		34
#define UART_UDIVSLOT_VAL	0xDDDD
#endif

#define CONFIG_NR_DRAM_BANKS    2          /* we have 2 bank of DRAM */
#define SDRAM_BANK_SIZE         0x10000000    /* 512 MB lqm*/
//#define SDRAM_BANK_SIZE         0x20000000    /* 1GB lqm*/
#define PHYS_SDRAM_1            MEMORY_BASE_ADDRESS /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE       SDRAM_BANK_SIZE
#define PHYS_SDRAM_2            MEMORY_BASE_ADDRESS2 /* SDRAM Bank #2 */
#define PHYS_SDRAM_2_SIZE       SDRAM_BANK_SIZE

#define CFG_FLASH_BASE		0x80000000

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_MX_LV640EB		/* MX29LV640EB */
//#define CONFIG_AMD_LV800		/* AM29LV800BB */

#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */

#if	defined(CONFIG_MX_LV640EB)
#define CFG_MAX_FLASH_SECT	135
#define PHYS_FLASH_SIZE		0x800000	/* 8MB */
#elif	defined(CONFIG_AMD_LV800)
#define CFG_MAX_FLASH_SECT	19
#define PHYS_FLASH_SIZE		0x100000	/* 1MB */
#else
#define CFG_MAX_FLASH_SECT	512
#define PHYS_FLASH_SIZE		0x100000	/* 1MB */
#endif

#define CFG_FLASH_LEGACY_512Kx16
#define CONFIG_FLASH_CFI_LEGACY
#define CFG_FLASH_CFI

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(5*CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(5*CFG_HZ) /* Timeout for Flash Write */

#define CFG_ENV_ADDR		0
#define CFG_ENV_SIZE		0x4000	/* Total Size of Environment Sector */

/*
 * SMDKC110 board specific data
 */

#define CONFIG_IDENT_STRING	" for x210"

/* total memory required by uboot */
#define CFG_UBOOT_SIZE		(2*1024*1024)

 /* base address for uboot */
#ifdef CONFIG_ENABLE_MMU
#define CFG_UBOOT_BASE		0xc3e00000
#else
#define CFG_UBOOT_BASE		0x33e00000  /*yan*/
//#define CFG_UBOOT_BASE		0x23e00000  /*yan*/
#endif

#define CFG_PHY_UBOOT_BASE	MEMORY_BASE_ADDRESS + 0x3e00000
#define CFG_PHY_KERNEL_BASE	MEMORY_BASE_ADDRESS + 0x8000

#define CFG_ENV_OFFSET		0x0007C000

/* nand copy size from nand to DRAM.*/
#define	COPY_BL2_SIZE		0x80000

/* NAND configuration */
#define CFG_MAX_NAND_DEVICE     1
#define CFG_NAND_BASE           (0xE7200000)
#define NAND_MAX_CHIPS          1

#define NAND_DISABLE_CE()	(NFCONT_REG |= (1 << 1))
#define NAND_ENABLE_CE()	(NFCONT_REG &= ~(1 << 1))
#define NF_TRANSRnB()		do { while(!(NFSTAT_REG & (1 << 0))); } while(0)

#define CFG_NAND_SKIP_BAD_DOT_I	1  /* ".i" read skips bad blocks   */
#define	CFG_NAND_WP		1
#define CFG_NAND_YAFFS_WRITE	1  /* support yaffs write */

#define CFG_NAND_HWECC
#define CONFIG_NAND_BL1_8BIT_ECC
#undef	CFG_NAND_FLASH_BBT

/* IROM specific data */
#define SDMMC_BLK_SIZE        (0xD003A500)
#define COPY_SDMMC_TO_MEM     (0xD003E008)

/* SD/MMC configuration */
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_S3C_HSMMC
#undef DEBUG_S3C_HSMMC

/* The macro for MMC channel 0 is defined by default and can't be undefined */
#define USE_MMC0
//#define USE_MMC0_8BIT //lqm added for test
#define USE_MMC2
//#define USE_MMC2_8BIT
#define MMC_MAX_CHANNEL		4

/*
 * SD/MMC detection takes a little long time
 * So, you can ignore detection process for SD/MMC card
 */
#undef	CONFIG_NO_SDMMC_DETECTION

#define CONFIG_MTDPARTITION	"80000 400000 3000000"

/* OneNAND configuration */
#define CFG_ONENAND_BASE 	(0xB0000000)
#define CFG_MAX_ONENAND_DEVICE	1

#define CONFIG_BOOT_ONENAND_IROM
//#define CONFIG_NAND
//#define CONFIG_BOOT_NAND
//#define CONFIG_ONENAND
#define ONENAND_REG_DBS_DFS_WIDTH 	(0x160)
#define ONENAND_REG_FLASH_AUX_CNTRL     (0x300)

#define GPNCON_OFFSET		0x830
#define GPNDAT_OFFSET		0x834
#define GPNPUD_OFFSET		0x838

#define CFG_ENV_IS_IN_AUTO

/* Fastboot variables */
//#define CFG_FASTBOOT_TRANSFER_BUFFER		(0x3E000000)//(0x40000000)
//#define CFG_FASTBOOT_TRANSFER_BUFFER_SIZE	(0x11000000)   /* 272MB */
#define CFG_FASTBOOT_TRANSFER_BUFFER		(0x3C000000)
#define CFG_FASTBOOT_TRANSFER_BUFFER_SIZE	(0x13000000)
#define CFG_FASTBOOT_ADDR_KERNEL		(0xC0008000)
#define CFG_FASTBOOT_ADDR_RAMDISK		(0x30A00000)
#define CFG_FASTBOOT_PAGESIZE			(2048)	// Page size of booting device
#define CFG_FASTBOOT_SDMMC_BLOCKSIZE		(512)	// Block size of sdmmc

/* Just one BSP type should be defined. */
//#define CFG_FASTBOOT_ONENANDBSP
//#define CFG_FASTBOOT_NANDBSP
#define CFG_FASTBOOT_SDMMCBSP

/* LCD setting */
//#define CFG_LCD_TL2796		// for C110 - narrow LCD
#define CFG_LCD_NONAME1			// for V210 - wide LCD
#define CFG_LCD_FBUFFER				(0x34000000)

//#define CONFIG_BOOTDELAY	3
#define CONFIG_BOOTDELAY	6
#if defined(CFG_FASTBOOT_NANDBSP)
#define CONFIG_BOOTCOMMAND	"nand read C0008000 600000 400000; nand read 30A00000 B00000 180000; bootm C0008000 30A00000"
#elif defined(CFG_FASTBOOT_SDMMCBSP)
#define CONFIG_BOOTCOMMAND	"movi read kernel 30008000; movi read rootfs 30B00000 300000; bootm 30008000 30B00000"
//#define CONFIG_BOOTCOMMAND	"movi read kernel 20008000; movi read rootfs 30B00000 300000; bootm 20008000 30B00000"
#endif

#endif	/* __CONFIG_H */
