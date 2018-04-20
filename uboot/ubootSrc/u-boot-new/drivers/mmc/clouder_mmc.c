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

#include <common.h>
#include <malloc.h>
#include <mmc.h>
#include <errno.h>
#include <asm-generic/errno.h>
#include <mci.h>

#define MMC_CMD_RETRIES  20

enum {
	RESP_T_OKAY = 0,
	RESP_T_RE,
	RESP_T_CRC,
	RESP_T_RTO
};

char *response[] = {
	"Response",
	"Response error",
	"Response CRC error",
	"Response timeout",
	NULL
};

#ifdef DEBUG_MSHC
#define dbg(x...)       printf(x)
#else 
#define dbg(x...)       do { } while (0)
#endif

#define PAGE_SIZE 4096
#define __tbl_th(x, blksize) ( (x) / blksize)

#define __virt_to_phys(x) ((x) & 0x1fffffff)

#define MMC_DATA_ERR  (MMC_INT_EBE | MMC_INT_SBE | MMC_INT_HLE | \
        MMC_INT_FRUN | MMC_INT_EBE | MMC_INT_DCRC) 
#define MMC_DATA_TMO (MMC_INT_HTO | MMC_INT_DRTO)                     

struct mci_idmac {
    unsigned int flags; /* desc0 */
#define MMC_IDMAC_OWN     (1 << 31)
#define MMC_IDMAC_ER      (1 << 5)
#define MMC_IDMAC_CH      (1 << 4)
#define MMC_IDMAC_FS      (1 << 3)
#define MMC_IDMAC_LD      (1 << 2)
#define MMC_IDMAC_DIC     (1 << 1)

    unsigned int cnt; /* desc1 */
    unsigned int addr; /* desc2 */
    unsigned int next_addr; /* desc3 */
};

#ifdef CONFIG_MMC_TRACE
static void mci_dumpregs(struct mci_host *host)
{
	dbg( "mci: ============== REGISTER DUMP ==============\n");
	dbg( "mci:  CTRL:      0x%08x\n",
		mci_readl(host,  CTRL));
	dbg( "mci:  PWREN:     0x%08x\n",
		mci_readl(host,  PWREN));
	dbg( "mci:  CLKDIV:    0x%08x\n",
		mci_readl(host,  CLKDIV));
	dbg( "mci:  CLKSRC:    0x%08x\n",
		mci_readl(host,  CLKSRC));
	dbg( "mci:  CLKENA:    0x%08x\n",
		mci_readl(host,  CLKENA));
	dbg( "mci:  TMOUT:     0x%08x\n",
		mci_readl(host,  TMOUT));
	dbg( "mci:  CTYPE:     0x%08x\n",
		mci_readl(host,  CTYPE));
	dbg( "mci:  BLKSIZ:    0x%08x\n",
		mci_readl(host,  BLKSIZE));
	dbg( "mci:  BYTCNT:    0x%08x\n",
		mci_readl(host,  BYTCNT));
	dbg( "mci:  INTMSK:    0x%08x\n",
		mci_readl(host,  INTMASK));
	dbg( "mci:  CMDARG:    0x%08x\n",
		mci_readl(host,  CMDARG));
	dbg( "mci:  CMD:       0x%08x\n",
		mci_readl(host,  CMD));
	dbg( "mci:  MINTSTS:   0x%08x\n",
		mci_readl(host,  MINTSTS));
	dbg( "mci:  RINTSTS:   0x%08x\n",
		mci_readl(host,  RINTSTS));
	dbg( "mci:  STATUS:    0x%08x\n",
		mci_readl(host,  STATUS));
	dbg( "mci:  FIFOTH:    0x%08x\n",
		mci_readl(host,  FIFOTH));
	dbg( "mci:  CDETECT:   0x%08x\n",
		mci_readl(host,  CDETECT));
	dbg( "mci:  WRTPRT:    0x%08x\n",
		mci_readl(host,  WRTPRT));
	dbg( "mci:  GPIO:      0x%08x\n",
		mci_readl(host,  GPIO));
	dbg( "mci:  TCBCNT:    0x%08x\n",
		mci_readl(host,  TCBCNT));
	dbg( "mci:  TBBCNT:    0x%08x\n",
		mci_readl(host,  TBBCNT));
	dbg( "mci:  DEBNCE:    0x%08x\n",
		mci_readl(host,  DEBNCE));
	dbg( "mci:  USRID:     0x%08x\n",
		mci_readl(host,  USRID));
	dbg( "mci:  VERID:     0x%08x\n",
		mci_readl(host,  VERID));
	dbg( "mci:  HCON:      0x%08x\n",
		mci_readl(host,  HCON));
	dbg( "mci:  UHS_REG:   0x%08x\n",
		mci_readl(host,  UHS_REG));
	dbg( "mci:  BMOD:      0x%08x\n",
		mci_readl(host,  BMOD));
	dbg( "mci:  PLDMND:    0x%08x\n",
		mci_readl(host,  PLDMND));
	dbg( "mci:  DBADDR:    0x%08x\n",
		mci_readl(host,  DBADDR));
	dbg( "mci:  IDSTS:     0x%08x\n",
		mci_readl(host,  IDSTS));
	dbg( "mci:  IDINTEN:   0x%08x\n",
		mci_readl(host,  IDINTEN));
	dbg( "mci:  DSCADDR:   0x%08x\n",
        mci_readl(host,  DSCADDR));
	dbg( "mci:  BUFADDR:   0x%08x\n",
		mci_readl(host,  BUFADDR));
	dbg( "mci: ===========================================\n");
}
#else
static void mci_dumpregs(struct mci_host *host) {}
#endif

static void mci_clkdiv_sel(struct mci_host *host, int clksrc)
{
	/* current select clkdiv 0, implement future */
	unsigned int tmp;

	tmp = mci_readl(host, CLKSRC);
	tmp &= (~0xff);  /* clear */
	tmp = clksrc;
	mci_writel(host, CLKSRC, tmp);
	host->clksrc = clksrc;
}

static void mci_reset_ciu(struct mci_host *host)
{
	u32 timeout = 100;
	u32 ier;

	ier = mci_readl(host, CTRL);
	ier |= MMC_CTRL_CTL_RESET;

	mci_writel(host, CTRL, ier);
	while (mci_readl(host, CTRL) & MMC_CTRL_CTL_RESET) {
		if (timeout == 0) {
			printf("Reset CTRL never completed.\n");
			return;
		}
		timeout--;
		udelay(1);
	}
}

static void mci_reset_fifo(struct mci_host *host)
{
	u32 timeout = 100;
	u32 ier;

	ier = mci_readl(host, CTRL);
	ier |= MMC_CTRL_FIFO_RESET;

	mci_writel(host, CTRL, ier);
	while (mci_readl(host, CTRL) & MMC_CTRL_FIFO_RESET) {
		if (timeout == 0) {
			printf("Reset FIFO never completed.\n");
			return;
		}
		timeout--;
		udelay(1);
	}
}

static void mci_reset_dma(struct mci_host *host)
{
	u32 timeout = 100;
	u32 ier;

	ier = mci_readl(host, CTRL);
	ier |= MMC_CTRL_DMA_RESET;

	mci_writel(host, CTRL, ier);
	while (mci_readl(host, CTRL) & MMC_CTRL_DMA_RESET) {
		if (timeout == 0) {
			printf("Reset DMA never completed.\n");
			return;
		}
		timeout--;
		udelay(1);
	}
}

static void mci_reset_all(struct mci_host *host)
{
	mci_reset_ciu(host);
	mci_reset_fifo(host);
	mci_reset_dma(host);
}

static void mci_idmac_reset_dma(struct mci_host *host)
{   
	u32 temp;   

	temp = mci_readl(host, BMOD);
	/* Software reset of DMA, the DMA Controller resets all 
	 * its internal registers. automatically cleared after 1 clock cycle
	 */
	temp |= MMC_BMOD_IDMAC_SWR;
	mci_writel(host, BMOD, temp);
}       

static void mci_idmac_stop_dma(struct mci_host *host)
{
	u32 ctrl, temp;

	/* Disable and reset the IDMAC interface */
	ctrl = mci_readl(host, CTRL);
	ctrl &= ~(MMC_CTRL_USE_IDMAC & MMC_CTRL_USE_IDMAC);
	mci_writel(host, CTRL, ctrl);

	/* Stop the IDMAC running, idmac block */
	temp = mci_readl(host, BMOD);
	temp &= ~(MMC_BMOD_IDMAC_EN| MMC_BMOD_IDMAC_FB);
	mci_writel(host, BMOD, temp);

	mci_idmac_reset_dma(host);
}

static void mci_clock_onoff(struct mci_host *host, int val)
{
	volatile u32 loop_count = 0x100000;

	if(val) {
		mci_writel(host, CLKENA, 1 << host->slot_using);
		mci_writel(host, CMDARG, 0);
		mci_writel(host, CMD, CMD_ONLY_CLK);
		do {
			if(!(mci_readl(host, CMD) & MMC_CMD_START))
				break;
			loop_count--;
		} while (loop_count);
	} else {
		mci_writel(host, CLKENA, 0 << host->slot_using);    
		mci_writel(host, CMDARG, 0);                      
		mci_writel(host, CMD, CMD_ONLY_CLK);           
		do {                                           
			if(!(mci_readl(host, CMD) & MMC_CMD_START))
				break;                                 
			loop_count--;                              
		} while (loop_count);                          
	}

	if(loop_count == 0) {  
		printf("%s Clock %s has been failed.\n ", MOD, val ? "ON":"OFF");
	}

}

static int mci_clock_change(struct mci_host *host, u32 freq)
{
	u32 div, status;
	int loop_count = 1000000;
	unsigned long sclk;

	if(freq == host->clock)
		return 0;
    
	if(freq == 0) {
		host->clock = freq;
		return 0;
	}

	dbg("%s UPC: slot(%d) using, hclock(%d), frep(%d)\n", \
			MOD, host->slot_using, host->clock, freq);

	if(host->mmc_clk) {
		sclk = host->mmc_clk(host->dev_id);
	} else if(host->bus_hz) {
		sclk = host->bus_hz;
	} else {
		printf("Get source clock failed.\n");
		return -EINVAL;
	}

	/* calculate clock divider value */
	if(sclk % freq)
		div = (sclk / freq >> 1) + 1;
	else 
		div = sclk / freq >> 1;

	if (div == 0 && !(mci_readl(host, UHS_REG) & (1 << host->slot_using)))
		div = 1;

	dbg("%s UPC: sclk %ld, set divider(%d)\n", MOD, sclk, div);

	/* befor changing clock. clock needs to be off. */ 
	mci_clock_onoff(host, CLK_DISABLE); 

	mci_writel(host, CLKDIV, div);
	mci_writel(host, CMDARG, 0);
	mci_writel(host, CMD, CMD_ONLY_CLK);
	for(; ;)
	{
		status = mci_readl(host, CMD);
		if(!(status & MMC_CMD_START))
			break;

		if(!loop_count--) {
			printf("%s UPC set divider timeout.\n", MOD);
			return TIMEOUT;
		}
	}

	mci_writel(host, CMD, mci_readl(host, CMD) & (~MMC_CMD_UPD_CLK));

	mci_clock_onoff(host, CLK_ENABLE);

	host->clock = freq;

	return 0;
}

static void mci_set_ios(struct mmc *mmc)
{
	struct mci_host *host = mmc->priv;
	unsigned int val = 0;

	dbg("%s Buswidth = %d, clock:%d\n", MOD, mmc->bus_width, mmc->clock);

	mci_clock_change(host, mmc->clock);

	/* setup bus width, 1bit, 4bits*/
	val = mci_readl(host, CTYPE);
	switch(mmc->bus_width) {
		case 8:                          
			val |= 1 << (host->slot_using + 16);
			val &= ~(1 << host->slot_using);
			break;                       
		case 4:                          
			val &= ~(1 << (host->slot_using + 16));
			val |= 1 << host->slot_using;
			break;                       
		default:                         
			val &= ~(1 << (host->slot_using + 16));
			val &= ~(1 << host->slot_using);
			break;                       
	}

	mci_writel(host, CTYPE, val);

	/* select divider */
	if(host->clksel)                      
		host->clksel(host, host->clksrc);
}

static int mci_set_trans_mode(struct mci_host *host, struct mmc_data *data)
{
	u32 mode;

	mode = MMC_CMD_DATA_EXP;
	if(data->flags & MMC_DATA_WRITE)
		mode |= MMC_CMD_RW;

	dbg("%s %s:", MOD, (data->flags & MMC_DATA_WRITE) ? "write" : "read");

	return mode;
}

static void mci_set_idma_desc(struct mci_idmac *idmac,
		u32 desc0, u32 desc1, u32 desc2)
{
	struct mci_idmac *desc = idmac;

	desc->flags = desc0;
	desc->cnt = desc1;
	desc->addr = __virt_to_phys(desc2);
	desc->next_addr = __virt_to_phys((unsigned int)desc) + sizeof(struct mci_idmac);
#ifdef CONFIG_MMC_TRACE
	printf("%s: CH(0x%08x), desc0(0x%08x), desc1(0x%08x), \ndesc2(0x%08x), desc3(0x%08x)\n", \
			__func__, (unsigned int)desc, desc->flags,\
			desc->cnt, desc->addr, desc->next_addr);
#endif
}

static int mci_pre_data(struct mci_host *host, struct mmc_data *data)
{
	u32 blksize = 0, blocks, bytecnt;
	unsigned int buf_ptr, flags, cnt;
	unsigned int i = 0, ctrl;
	ulong list_start, list_end, ptr_start, ptr_end;
	ALLOC_CACHE_ALIGN_BUFFER(struct mci_idmac, cur_idmac, data->blocks);

	mci_reset_fifo(host);

	blksize = data->blocksize;
	bytecnt = blksize * data->blocks;
	dbg("blocks %d, blksize %d, bytecnt %d.\n", \
			data->blocks, data->blocksize, bytecnt);
	blocks = data->blocks;
	if(host->use_idmac) {
		unsigned int step = __tbl_th(PAGE_SIZE, data->blocksize);

		list_start = (ulong)cur_idmac;
		mci_writel(host, DBADDR, __virt_to_phys((unsigned int)cur_idmac));

		if (data->flags == MMC_DATA_READ)
			buf_ptr = (unsigned int)data->dest;
		else
			buf_ptr = (unsigned int)data->src;
		ptr_start = (ulong)buf_ptr;

		do {
			flags = MMC_IDMAC_OWN | MMC_IDMAC_CH ;
			flags |= (i == 0) ? MMC_IDMAC_FS : 0;
			if (blocks <= step) {
				flags |= MMC_IDMAC_LD;
				flags &= ~MMC_IDMAC_CH;
				cnt = data->blocksize * blocks;
			} else
				cnt = data->blocksize * step;

			mci_set_idma_desc(cur_idmac, flags, cnt,
					buf_ptr + (i * PAGE_SIZE));

			if(blocks <= step)
				break;

			blocks -= step;
			cur_idmac++;
			i++;
		} while(1);

		list_end = (ulong)cur_idmac;
#ifdef HAVE_L2
		flush_dcache_all();
#else
		flush_dcache_range(list_start, list_end + ARCH_DMA_MINALIGN);
#endif

		ctrl = mci_readl(host, CTRL);
		ctrl |= (MMC_CTRL_USE_IDMAC | MMC_CTRL_DMA_ENABLE);
		mci_writel(host, CTRL, ctrl);

		ctrl = mci_readl(host, BMOD);
		ctrl |= MMC_BMOD_IDMAC_EN | MMC_BMOD_IDMAC_FB;
		mci_writel(host, BMOD, ctrl);

		/* Start it running */
		mci_writel(host, PLDMND, 1);

		/* flush dcache, idmac using */
		ptr_end = (ulong)(buf_ptr + bytecnt);
#ifdef HAVE_L2
		flush_dcache_all();
#else
		flush_dcache_range(ptr_start, ptr_end + ARCH_DMA_MINALIGN);
#endif
	}

	mci_writel(host, BYTCNT, bytecnt); 
	mci_writel(host, BLKSIZE, blksize);

	return 0;
}

static int mci_pre_cmd(struct mci_host *host, struct mmc_cmd *cmd)
{
	int flags = 0;

	if ((cmd->resp_type & MMC_RSP_136) && (cmd->resp_type & MMC_RSP_BUSY))
		return -EINVAL;

	if (cmd->cmdidx == MMC_CMD_STOP_TRANSMISSION)
		flags |= MMC_CMD_STOP_ABORT_SEND;
	else
		flags |= MMC_CMD_WAIT_PRVDAT;

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		flags |= MMC_CMD_RESP_EXP;
		if (cmd->resp_type & MMC_RSP_136)
			flags |= MMC_CMD_RESP_LENGTH;
	}

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= MMC_CMD_RESP_CRC;

	flags |= (cmd->cmdidx | MMC_CMD_START | MMC_CMD_USE_HOLD_REG);

	return flags;
}

unsigned int wait_for_cmd_done(struct mci_host *host, unsigned int flags)
{                                                                             
	unsigned int done = 0;  

	while(!((done = mci_readl(host, RINTSTS)) & MMC_INT_CMD_DONE)) {                                                                         
		if(done & MMC_INT_RE) {                                                                     
			mci_writel(host, RINTSTS, MMC_INT_RE);                                        
			return RESP_T_RE;                                                 
		}                                                                     

		if(done & MMC_INT_RCRC) {                                                                     
			mci_writel(host, RINTSTS, MMC_INT_RCRC);                                        
			return RESP_T_CRC;                                                
		}                                                                     

	} /* wait cmd done */                                                     

	dbg("%s CMD Done\n", MOD);

	mci_writel(host, RINTSTS, MMC_INT_CMD_DONE);                                        

	/* no resp cmd done */                                                    
	if(!(flags & MMC_CMD_RESP_EXP))                                             
		return RESP_T_OKAY;                                                   

	if(done & MMC_INT_RTO) {                                                                         
		mci_writel(host, RINTSTS, MMC_INT_RTO);                                        
		return RESP_T_RTO;                                                    
	}

	return RESP_T_OKAY;                                                       
}       

static void mci_pull_data(struct mci_host *host, unsigned int *buf, unsigned int len)
{
	unsigned int *pdata = (unsigned int *)buf;

	for (; len; len -= 4)
		*pdata++ = mci_readl(host, DATA_OFFSET);
}

static int mci_recv_req(struct mci_host *host, char *buf,
		u32 *offset, size_t *remain)
{
	unsigned int fcnt, len = 0;

	/* Read bytes in fifo */
	fcnt = MMC_GET_FCNT(mci_readl(host, STATUS)) << host->data_shift;
	len = min(*remain, fcnt);
	if(!len)
		return 0;

	mci_pull_data(host, (unsigned int *)(buf + *offset), len); /* bytes */
	*offset += len;
	*remain -= len;

	return 1;
}

static void msdc_read(struct mci_host *host, struct mmc_data *data)
{
	unsigned int pending;
	char *buf = (char *)data->dest;
	u32 offset = 0;
	unsigned int remain = data->blocks * data->blocksize;

	/* polling data receive */
	for(; ;)
	{
		pending = mci_readl(host, RINTSTS);

		if(pending & MMC_INT_DTO) {/* receiver data request */
			dbg("%s Data RECV Completed\n", MOD);
			pending |= MMC_INT_DTO;
			mci_writel(host, RINTSTS, pending);

			/* Case: init, read data less than a block */
			if (remain) {
				mci_recv_req(host, buf, &offset, &remain);
				if (remain)
					printf("Data Recv Fatal\n");
			}

			break;
		}

		if(pending & MMC_INT_RXDR) { /* receiver data request */
			dbg("%s Data RECV Request\n", MOD);
			if(host->data_shift == 2) { /* push 4 bytes once time, 16bit align */
				if (!(mci_recv_req(host, buf, &offset, &remain)))
					break;
			} else { /* 2 bytes or 8bytes once time */
				printf("%s Data shift not support currently.\n", MOD);
				break;
			}

			pending |= MMC_INT_RXDR;
			mci_writel(host, RINTSTS, pending);
		}

		if(pending & (MMC_DATA_ERR | MMC_DATA_TMO)) {
			printf("%s Date Read Err, pending 0x%08x\n", MOD, pending);

			pending |= (MMC_DATA_ERR | MMC_DATA_TMO);
			mci_writel(host, RINTSTS, pending);

			break;
		}
	}
}

static void mci_push_data(struct mci_host *host, unsigned int *buf, unsigned int len)
{
	unsigned int *pdata = (unsigned int *)buf;

	for (; len; len -= 4)
		mci_writel(host, DATA_OFFSET, *pdata++);
}

static void msdc_write(struct mci_host *host, struct mmc_data *data)
{
	unsigned int pending;
	char *buf = (char *)data->src;
	u32 offset = 0;
	unsigned int remain = data->blocks * data->blocksize;

	/* polling data receive */
	for(; ;)
	{
		pending = mci_readl(host, RINTSTS);

		if(pending & MMC_INT_DTO) {/* receiver data request */
			dbg("%s Data SEND Completed\n", MOD);
			pending |= MMC_INT_DTO;
			mci_writel(host, RINTSTS, pending);

			break;
		}

		if(pending & MMC_INT_TXDR) {
			dbg("%s Transmit FIFO Data Request.\n", MOD);
			/* write fifo */
			if(host->data_shift == 2) {/* push 4 bytes once time */
				unsigned int fcnt, len = 0;

				/* Write bytes */
				fcnt = (host->fifo_depth - \
						MMC_GET_FCNT(mci_readl(host, STATUS))) << host->data_shift;
				len = min(remain, fcnt);
				if(!len)
					continue;

				mci_push_data(host, (unsigned int *)(buf + offset), len); /* bytes */
				offset += len;
				remain -= len;

			} else { /* 2 bytes or 8bytes once time */
				printf("%s Data shift not support currently.\n", MOD);
				break;
			}

			pending |= MMC_INT_RXDR;
			mci_writel(host, RINTSTS, pending);
		}

		if(pending & (MMC_DATA_ERR | MMC_DATA_TMO)) {
			printf("%s Data Write ERR, pending 0x%08x\n", MOD, pending);
			pending |= (MMC_DATA_ERR | MMC_DATA_TMO);
			mci_writel(host, RINTSTS, pending);

			break;
		}

	}
}

static int mci_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	struct mci_host *host = (struct mci_host *)mmc->priv;
	int flags = 0, i;
	unsigned int tmo = 1000000;
	unsigned int status = 0;
	u32 retries = MMC_CMD_RETRIES;
	u32 cmds, rsp = 0;

	/* wait last data transfer end */
	for(; ;)
	{
		status = mci_readl(host, STATUS);
		if(!(status & MMC_STATUS_DBUSY))
			break;
		if(!tmo-- && data) {
			printf("Card data busy.\n");
			return TIMEOUT;
		}
	}

	/* mask all irq */
	mci_writel(host, RINTSTS, MMC_INT_MASK_A);
	if(data) {
		flags = mci_set_trans_mode(host, data);
		if(mci_pre_data(host, data))
			return -EIO;
	}

	flags |= mci_pre_cmd(host, cmd);
	mci_writel(host, CMDARG, cmd->cmdarg); /*  write arg */
	mci_writel(host, CMD, flags); /* write CMD */

	dbg("%s Send CMD%d(0x%08x),ARG(0x%08x)\n", MOD, cmd->cmdidx, \
			flags, cmd->cmdarg);

	for(i = 0; i < retries; i++)                                                            
	{                                                                                           
		cmds = mci_readl(host, CMD);
		if(!(cmds & MMC_CMD_START)) {
			rsp = wait_for_cmd_done(host, flags);

			break;
		}
	}

	if(rsp) {
		dbg("%s CMD%d(%x),ARG(%x), %s.\n", MOD, cmd->cmdidx, \
				flags, cmd->cmdarg, response[rsp]);

		return TIMEOUT;
	}

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		if (cmd->resp_type & MMC_RSP_136) {
			cmd->response[0] = mci_readl(host, RESP3);
			cmd->response[1] = mci_readl(host, RESP2);
			cmd->response[2] = mci_readl(host, RESP1);
			cmd->response[3] = mci_readl(host, RESP0);
		} else {
			cmd->response[0] = mci_readl(host, RESP0);
		}
	}

	/* data handle */
	if(data) {
		if(host->use_idmac) {
			unsigned int mask;

			do {
				mask = mci_readl(host, RINTSTS);
				if (mask & (MMC_DATA_ERR | MMC_DATA_TMO)) {
					printf("%s error during transfer: 0x%08x\n", MOD, mask);
					mci_dumpregs(host);
					mci_idmac_stop_dma(host);

					return -EIO;
				}
			} while (!(mask & MMC_INT_DTO));

			mci_writel(host, RINTSTS, mask);
			mci_idmac_stop_dma(host);
		} else {
			if(flags & MMC_CMD_RW) {
				msdc_write(host, data);
			} else {
				msdc_read(host, data);
			}
		}
	}

	udelay(100); /* wait for a moment */

	return 0;
}

void mci_power_en(struct mci_host *host, int en)
{                                                                                                
	unsigned int pwren;
	unsigned int tmp;

	if(host->slot_using > MAX_SLOTS)
		return;

	pwren = mci_readl(host, PWREN); 
	tmp = pwren;
	pwren = pwren & (1 << host->slot_using);

	if(en) {
		if(pwren) {
			/* In this case, nothing to do */
			return; 
		} else {
			/* In this case power on */
			pwren = tmp | (1 << host->slot_using);
			mci_writel(host, PWREN, pwren);
		}
	} else {
		if(pwren) {
			/* In this case, power off */
			pwren = tmp & (~(1 << host->slot_using));
			mci_writel(host, PWREN, pwren);
		} else {
			/* In this case, nothing to do */
			return;
		}
	}

	dbg("%s Power status: 0x%08x\n", MOD, mci_readl(host, PWREN)); 
}

static void mci_fifo_init(struct mci_host *host)
{
	u32 fval;
#define MSIZE(x)        ((x) << 28)
#define RX_WMARK(x)     ((x) << 16) 
#define TX_WMARK(x)     (x)

	if(host->use_idmac) {
		/* Setting fifo */
		fval = MSIZE(0x2) | RX_WMARK(host->fifo_depth / 2 - 1) | TX_WMARK(host->fifo_depth / 2);
		mci_writel(host, FIFOTH, fval);

		dbg("%sFIFO WMARK FOR RX 0x%x WX 0x%x.\n", MOD,
				host->fifo_depth / 2 -1, host->fifo_depth / 2);
	}

}

static void mci_init(struct mci_host *host)
{
	/* power on */
	mci_power_en(host, 1);

	mci_reset_all(host);
	mci_fifo_init(host);

	/* It clears all pending interrupts */
	mci_writel(host, RINTSTS, 0xFFFFFFFF); 
	/* It dose not use Interrupt. Disable all */
	mci_writel(host, INTMASK, 0);
}

static int clouder_mci_init(struct mmc *mmc) 
{
	struct mci_host *host = (struct mci_host *)mmc->priv;

	mci_init(host);

	/* 400KHZ */
	mci_clock_change(host, 400000);

	/* CTRL clear */
	mci_writel(host, CTRL, 0);

	/* set debounce filter value*/
	mci_writel(host, DEBNCE, 0xfffff);

	/* clear card type. set 1bit mode */
	mci_writel(host, CTYPE, 0x0);

	/* set bus mode register for IDMAC */
	mci_writel(host, BMOD, MMC_BMOD_IDMAC_SWR);

	/* disable all interrupt source of IDMAC */
	mci_writel(host, IDINTEN, 0);

	/* set max timeout */
	mci_writel(host, TMOUT, 0xFFFFFFFF);

	return 0; 
}

static int mci_getcd(struct mmc *mmc)
{
#ifdef CONFIG_DW_MMC_HW_DETECT
	struct mci_host *host = (struct mci_host *)mmc->priv;
	unsigned int cd, tmp;                                       
	int cdbit, cnum = 0;  

	cd = mci_readl(host, CDETECT);                                    
	if(!cd) /* 31~30 reserved */                   
		return cnum;                                            
	tmp = cd;                                                   
	for(cdbit = 0; cdbit < MAX_SLOTS; cdbit++)                         
	{                                                           
		cd &= 0x1;                                              
		if(cd) {                                                       
			host->slot[cdbit] = 1; /* card detect */
			cnum++;                                             
			host->slot_using = cdbit; /* card detect */
		}                                                       

		cd = tmp >> (cdbit + 1);                                
	}                                                           

	if(--cnum > 0 && host->slot[0]) {
		dbg("\n%s %d cards detected.\n", MOD, ++cnum);  
		host->slot_using = 0; /* 0 slot in using */
	}

	return cnum;
#else

	return 1;
#endif
}

static struct mmc_ops mci_ops = {
	.send_cmd = mci_send_cmd,
	.set_ios  = mci_set_ios,
	.init	  = clouder_mci_init,
	.getcd    = mci_getcd
};

struct mmc *clouder_mmc_initialize(struct mci_host *host)
{
	struct mmc_config *conf;
	int hcon;
	unsigned int fsize;

	dbg("%s mmc%d register\n", MOD, host->dev_id);

	conf = &host->cfg;
	conf->ops = &mci_ops;

	host->clock = 0;
	host->clksel = mci_clkdiv_sel;

	conf->name = host->name;
	conf->f_max = host->bus_hz; /* 25MHZ */
	conf->f_min = DIV_ROUND_UP(host->bus_hz, 510); /* 100KHZ */
	conf->voltages = MMC_VDD_165_195 | MMC_VDD_31_32 |
		MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_34_35;

	if (host->buswidth == MMC_MODE_4BIT) {
		conf->host_caps &= ~MMC_MODE_8BIT;
		conf->host_caps |= MMC_MODE_4BIT;
	} else if (host->buswidth == MMC_MODE_8BIT) {
		conf->host_caps &= ~MMC_MODE_4BIT;
		conf->host_caps |= MMC_MODE_8BIT;
	} else {
		conf->host_caps &= ~(MMC_MODE_4BIT | MMC_MODE_8BIT);
	}

	conf->host_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS | MMC_MODE_HC;
	conf->b_max = 65536;

	/* FIFO Width */
	hcon = mci_readl(host, HCON);
	hcon = ((hcon >> 7) & 0x7);
	host->data_shift = 1 << hcon;

	fsize = mci_readl(host, FIFOTH);     
	fsize = 1 + ((fsize >> 16) & 0xfff); 
	host->fifo_depth = fsize;            

	return mmc_create(conf, host);
}

int clouder_mmc_init(struct mci_host **host, int mmax)
{
	struct mmc *mmc;
	int i;
	u32 iomux = 0;   

	for (i = 0; i < mmax; i++) {
		struct mci_host *mci = host[i];

		if (!mci) {
			printf("mci device %d initialize, no host\n", i);
			continue;
		}

		mmc = clouder_mmc_initialize(mci);
		if (!mmc)
			printf("mci device %d unregistered\n", i);
		else 
			mci->mmc = mmc;
	}

	return 0;                            
}
