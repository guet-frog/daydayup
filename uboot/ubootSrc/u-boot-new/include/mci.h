/*
 * Semiclouder Multimedia Card Interface test and verify.
 *
 * Copyright (C) 2013 Shanghai Semiclouder, Ltd, All Rights Reserved.
 * 
 *      Author      Version     Time            Email
 *      warnus      v1.0        23/02/2013      ......
 */

#ifndef  CRSI_MMC_H
#define  CRSI_MMC_H

#include <asm/io.h>
#include <mmc.h>

/* Control Register */
#define  CTRL  0x00

/* 31~21 reserved present */
#define MMC_CTRL_USE_IDMAC	(1 << 25)	/* Use internal dmac */
#define MMC_CTRL_ENABLE_OD	(1 << 24)	/* External open-drain  pullup */
#define MMC_CTRL_CARD_VOLB	(1 << 20)	/* Card regulator-B voltage setting, output to card_volt_b port */
#define MMC_CTRL_CARD_VOLA  (1 << 16)   /* Card regulator-A voltage setting, output to card_volt_a port */
/* 15~12 reserved present */
#define MMC_CTRL_CEATE_INT_EN  (1 << 11)	/* Creat device interrupt status */
#define MMC_CTRL_SEND_AUTO_STOP_CCSD  (1 << 10)		/* Send auto stop ccsd */
#define MMC_CTRL_SEND_CCSD	(1 << 9)	/* Send ccsd */
#define MMC_CTRL_ABORT_READ_DATA (1 << 8)	/* Abort read data, used in SDIO card suspend sequence */
#define MMC_CTRL_SEND_IRQ_RSP	(1 << 7) /* Send irq response, bit automatically clears once response is sent */
#define MMC_CTRL_READ_WAIT	(1 << 6)	/* For sending read-wait to SDIO cards. */
#define MMC_CTRL_DMA_ENABLE	( 1 << 5)	/* Valid only if device configured for External DMA interface */
#define MMC_CTRL_INT_ENABLE ( 1 << 4) /* Global interrupt enable/disable bit */
#define MMC_CTRL_DMA_RESET	(1 << 2)	/* To reset DMA interface, firmware should set bit to 1 */
#define MMC_CTRL_FIFO_RESET	(1 << 1)	/* To reset FIFO, firmware should set bit to 1 */
#define MMC_CTRL_CTL_RESET	(1 << 0)	/* To reset controller, firmware should set bit to 1 */
#define MMC_MCI_RESET (MMC_CTRL_DMA_RESET | MMC_CTRL_FIFO_RESET | MMC_CTRL_CTL_RESET)

/* Power Enable Register, Power on/off switch for up to 16 cards, 
 * Once power is turned on, firmware should wait for regulator/switch 
 * ramp-up time before trying to initialize card. Only NUM_CARDS number of bits are implemented
 * 
 * | 31		30		29		28 -------- 2 1 0 |
 * |    reserved	<---------now use ------->|
 */
#define  PWREN  (0x04)

/* Clock Divider Register
 * | 31 ------------ 24 | 23 ------------ 16 | 15 ------------ 8 | 7 ------------ 0|
 *			  |					|						|					|
 * clk_div3 <--					|						|					|
 *				   clk_div2 <----						|					|
 *										  clk_div1 <-----					|
 *															  clk_div0 <-----	
 * value of 0 means divide by 2*0 = 0 (no division, bypass), 
 * a value of 1 means divide by 2*1 = 2, a value of “ff” means 
 * divide by 2*255 = 510, and so on
 */
#define  CLKDIV (0x08)


/* SD clock source register 
 * 00 – Clock divider 0
 * 01 – Clock divider 1
 * 10 – Clock divider 2
 * 11 – Clock divider 3
 *
 */
#define  CLKSRC (0x0c)


/* Clock enable register 
*/
#define  CLKENA (0x10)
#define MMC_CLKEN_LOWPOWER (1 << 16)	/* Low-power control for up to 16 SD card clocks 
					   and one MMC card clock supported. */
#define MMC_CLKEN_EN (1 << 0)	/* clock enable */


/* Timeout register */
#define  TMOUT (0x14)

#define MMC_TMOUT_DATA(n) (1 << n) /* value for card Data Read Timeout */
#define MMC_TMOUT_MASK 0xFFFFFF00
#define MMC_TMOUT_RESP(n) (n & 0xFF) /* Response timeout value */
#define MMC_TMOUT_RESP_MASK 0xFF 

/* Card type register */
#define  CTYPE (0x18)

#define MMC_CTYPE_8BIT (1 << 16) /* The card is in 8bit mode */
#define MMC_CTYPE_4BIT (1 << 0) /* The card is in 4bit mode */
#define MMC_CTYPE_1BIT  0 /* The card is in 1bit mode */

/* Block size register */
#define  BLKSIZE (0x1c)


/* Byte count register */
#define  BYTCNT (0x20) /* Number of bytes to be transferred; should be integer 
			  multiple of Block Size for block transfers. */

/* Interrupt mask register */
#define  INTMASK (0x24) 
#define  MINTSTS (0x40) /* Mask interrupt status register */
#define  RINTSTS (0x44) /* Raw interrupt status regiset */

#define MMC_INT_SDIO(n) (1 << (16 + n)) /* Mask SDIO interrupts */
#define MMC_INT_EBE (1 << 15) /* End-bit error(read)/write no CRC */
#define MMC_INT_ACD (1 << 14) /* Auto command done */
#define MMC_INT_SBE (1 << 13) /* Start bit error */
#define MMC_INT_HLE (1 << 12) /* Hardware locked writer error */
#define MMC_INT_FRUN (1 << 11) /* fifo underrun /overrun error */
#define MMC_INT_HTO (1 << 10) /* Data starvation by host timeout */
#define MMC_INT_DRTO (1 << 9) /* Data read timeout */
#define MMC_INT_RTO (1 << 8) /* Response timeout */
#define MMC_INT_DCRC (1 << 7) /* Data CRC error */
#define MMC_INT_RCRC (1 << 6) /* Response CRC error */
#define MMC_INT_RXDR (1 << 5) /* Receive FIFO data request */
#define MMC_INT_TXDR ( 1 << 4) /* Transmit FIFO data request */
#define MMC_INT_DTO (1 << 3) /* Data transfer over */
#define MMC_INT_CMD_DONE (1 << 2) /* command done */
#define MMC_INT_RE (1 << 1) /* Response error */
#define MMC_INT_CD (1 << 0) /* Card dected */
#define MMC_INT_MASK_A 0xffffffff

/* Command argument register */
#define  CMDARG (0x28) 


/* command register */
#define  CMD (0x2c)

#define MMC_CMD_INDEX(n) ( (n) & 0X3f) /* command index */
#define MMC_CMD_RESP_EXP (1 << 6) /* response expected from card */
#define MMC_CMD_RESP_LENGTH (1 << 7) /*  short/long expext form card */
#define MMC_CMD_RESP_CRC (1 << 8) /* check response crc */
#define MMC_CMD_DATA_EXP (1 << 9) /* data transfer except */
#define MMC_CMD_RW (1 << 10) /* read form or write to card */
#define MMC_CMD_TRANS_MODE (1 << 11) /* transfer mode */
#define MMC_CMD_SEND_STOP (1 << 12) /* stop command send at the end of data transfer */
#define MMC_CMD_WAIT_PRVDAT (1 << 13) /* wait previous data transfer complete */
#define MMC_CMD_STOP_ABORT_SEND (1 << 14) /* host issues stop abort command to stop data transfer */
#define MMC_CMD_SEND_INIT (1 << 15) /* bit should be set while sending first command to card */
#define MMC_CMD_CARD_NUM (1 << 16) /* card number in use */
#define MMC_CMD_UPD_CLK ( 1 << 21 ) /* change card clock */
#define MMC_CMD_CEATE_RD (1 << 22) /* Software should set this bit to indicate 
				      that CE-ATA device is being accessed for read transfer */
#define MMC_CMD_CCS_EXP (1 << 23 ) /* just for CE-ATA device */
/* 30~24 reserved */
#define MMC_CMD_USE_HOLD_REG (1 << 29)
#define MMC_CMD_START (1 << 31) /* start command */
#define CMD_ONLY_CLK ( MMC_CMD_WAIT_PRVDAT | MMC_CMD_UPD_CLK | MMC_CMD_START)


/* Response registers */
#define  RESP0   (0x30) 
#define  RESP1   (0x34)
#define  RESP2   (0x38)
#define  RESP3   (0x3c)


/* Status register */
#define  STATUS		(0x48)
#define MMC_STATUS_FIFO_RWAK (1 << 0) /* fifo reached received watermark */
#define MMC_STATUS_FIFO_TWAK (1 << 1) /* fifo reached  transfer watermark */
#define MMC_STATUS_FIFO_EMPTY (1 << 2) /* fifo is empty */
#define MMC_STATUS_FIFO_FULL (1 << 3) /* fifo is full */
#define MMC_STATUS_CMD_FSM_STA (1 << 4) /* command fsm status */
#define MMC_STATUS_DATA3  (1 << 8) /* check whether card is present */
#define MMC_STATUS_DBUSY (1 << 9) /* card data is or is no busy */
#define MMC_STATUS_DMC_BUSY (1 << 10) /* data transfer or revceive state-machine is busy */
#define MMC_STATUS_RESP_INDEX (1 << 11) /* index of previous */
#define MMC_STATUS_FIFO_COUNT (1 << 12) /* number of filed locations in fifo */
#define MMC_STATUS_DMA_ACK (1 << 30) /* dma acknowledge signal state */
#define MMC_STATUS_DMA_REQ (1 << 31) /* dma request signal state */
#define MMC_GET_FCNT(x)  (((x)>>17) & 0x1FFF)


/* fifo threshold watermark register */
#define  FIFOTH (0x4C)
#define MMC_FIFOTH_TXWATERWARK_MASK(x) ( (x >> 0) & 0xfff) /* Tx transfer watermark */
/* 15 ~ 12, reserved */
#define MMC_FIFOTH_RXWATEWARK_MASK(x) ( (x >> 12) & 0xfff) /* Rx transfer watermark */
#define MMC_FIFOTH_CONT_MASK(x) ( (x >> 16) & 0x03) /* Msize define */
#define MMC_FIFOTH_DMT_SIZE_MASK ((x >> 28) & 0x03) /* Burst size of multiple transaction for DMA mult trnasfer */


/* Card detect register */
#define  CDETECT (0x50) 


/* Card wtite protect register */
#define  WRTPRT (0x54)


/* General purpose input/output register */
#define  GPIO (0x58)


/*  Transferred CIU card byte count register */
#define TCBCNT (0x5c)


/* Transferred Host to BIU-FIFO Byte Count Register */
#define  TBBCNT (0x60)


/* Debounce count register */
#define  DEBNCE (0x64)


/* User ID register */
#define  USRID (0x68)


/* Version ID register */
#define  VERID (0x6c)


/* Hardware Configuration Registe */
#define  HCON (0x70)

#define UHS_REG (0x74)


/* Bus mode register */
#define  BMOD (0x80)
#define MMC_BMOD_IDMAC_EN (1 << 7)
#define MMC_BMOD_IDMAC_FB (1 << 1)
#define MMC_BMOD_IDMAC_SWR (1 << 0)

/* Poll demand register */
#define  PLDMND (0x84)


/* Descriptor List Base Address Register */
#define  DBADDR (0x88)


/* Internal DMAC Status Register */
#define  IDSTS (0x8c)

#define MMC_IDSTS_TI (1 << 0) /* Transmit interrupt */
#define MMC_IDSTS_RI (1 << 1) /* Receive interrupt */
#define	MMC_IDSTS_FBE (1 << 2) /* Fatal Bus Error Interrupt */
#define MMC_IDSTS_DU (1 << 4) /* Descriptor Unavailable Interrupt */
#define MMC_IDSTS_CES (1 << 5) /* Card Error Summary */
#define MMC_IDSTS_NIS (1 << 8) /* Normal Interrupt Summary */
#define MMC_IDSTS_AIS (1 << 9) /* Abnormal Interrupt Summary */
#define MMC_IDSTS_EB (1 << 10) /* Indicates the type of error that caused a Bus Error */
#define MMC_IDSTS_FSM (1 << 13) /* DMAC FSM present state */


/* Internal DMAC Interrupt Enable Register */
#define  IDINTEN (0x90)

#define MMC_IDINTEN_TI (1 << 0) /* Transmit Interrupt Enable */
#define MMC_IDINTEN_RI (1 << 1) /* Receive Interrupt Enable */
#define MMC_IDINTEN_FBE (1 << 2) /* Fatal Bus Error Enable */
#define MMC_IDINTEN_DU (1 << 4) /* Descriptor Unavailable Interrupt */
#define MMC_IDINTEN_CES (1 << 5) /* Card Error summary Interrupt Enable */
#define MMC_IDINTEN_NI (1 << 8) /* Normal Interrupt Summary Enable */
#define MMC_IDINTEN_AI (1 << 9) /* Abnormal Interrupt Summary Enable */


/* Current Host Descriptor Address Register */
#define  DSCADDR (0x94)


/* Current Buffer Descriptor Address Register */
#define  BUFADDR (0x98)

#define  DATA_OFFSET (0x200)

#define CLK_ENABLE  (0x1<<0)
#define CLK_DISABLE (0x0<<0)

#define MOD "[MMC]: "

#define MAX_SLOTS 8

struct mci_host {
	char *name;
	void *ioaddr;
	unsigned int quirks;
	unsigned int caps;
	u8 data_shift;
	u8 use_idmac:1;
	u8 slot[MAX_SLOTS];
	u8 slot_using;
	unsigned int version;
	unsigned int clock;
	int clksrc;
	unsigned int bus_hz;
	int dev_id;
	int buswidth; 
	u32 fifo_depth;
	struct mmc *mmc; 

	void (*clksel)(struct mci_host *host, int clksrc);
	unsigned int (*mmc_clk)(int dev_index);

	struct mmc_config cfg;
};      

static inline void mci_writel(struct mci_host *host, int reg, u32 val)
{
	writel(val, host->ioaddr + reg);
}

static inline void mci_writew(struct mci_host *host, int reg, u16 val)
{
	writew(val, host->ioaddr + reg);
}

static inline void mci_writeb(struct mci_host *host, int reg, u8 val)
{
	writeb(val, host->ioaddr + reg);
}
static inline u32 mci_readl(struct mci_host *host, int reg)
{
	return readl(host->ioaddr + reg);
}

static inline u16 mci_readw(struct mci_host *host, int reg)
{
	return readw(host->ioaddr + reg);
}

static inline u8 mci_readb(struct mci_host *host, int reg)
{
	return readb(host->ioaddr + reg);
}

#endif
