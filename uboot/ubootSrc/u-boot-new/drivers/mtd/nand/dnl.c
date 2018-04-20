/*
 * NAND Flash Controller Device Driver
 * Copyright © 2009-2010, Intel Corporation and its suppliers.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include <common.h>      
#include <nand.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <malloc.h>
#include <dnl.h>

/* we define a module parameter that allows the user to override
 * that hardware and device what timing mode shoule be used.
 */
#define NAND_DEFAULT_TIMINGS    -1

static int onfi_timing_mode = NAND_DEFAULT_TIMINGS;

/* We define a macro here thar combines all interrupts this driver uses into
 * a signle constant value, for convenience.
 */
#define DENALI_IRQ_ALL	(INTR_STATUS__DMA_CMD_COMP | \
			INTR_STATUS__ECC_ERR | \
			INTR_STATUS__PROGRAM_FAIL | \
			INTR_STATUS__LOAD_COMP | \
			INTR_STATUS__PROGRAM_COMP | \
			INTR_STATUS__TIME_OUT | \
			INTR_STATUS__ERASE_FAIL | \
			INTR_STATUS__RST_COMP | \
			INTR_STATUS__ERASE_COMP)

/* indicates whether or not the internal value for the flash bank is
 * valid or not
 */
#define CHIP_SELECT_INVALID -1

#define SUPPORT_8BITECC     1

/* This macro divides two integers and rounds fractional values up
 * to the nearest integer value
 */
#define CEIL_DIV(x, y) (((x)%(y)) ? ((x)/(y)+1) : ((x)/(y)))

/* These constants are defined by the driver to enable common driver  
 * configuration options. 
 */                                          
#define SPARE_ACCESS        0x41                                      
#define MAIN_ACCESS     0x42                                          
#define MAIN_SPARE_ACCESS   0x43                                      

#define DENALI_READ	0
#define DENALI_WRITE	0x100

/* this is a helper macro that allows us to
 * format the bank into the proper bits for the controller 
 */
#define BANK(x) ((x) << 24)

#define __virt_to_phys(x) ((x) & 0x1fffffff)

static int nand_debug_level = 0;

/* forward declarations */
static void clear_status(struct denali_nand_info *denali);
static uint32_t wait_for_status(struct denali_nand_info *denali, 
        uint32_t status_mask);
static uint32_t read_int_status(struct denali_nand_info *denali);

/* global denali nand */
static struct denali_nand_info *clouder;

#ifdef CONFIG_MTD_DEBUG
static void nand_dump_buf(const uint8_t *buf, int len, int mod)
{
    int i;

    if(!buf || !len)
        return;

    printf("denali nand dump buf[%p]:\n", buf);
    for(i = 0; i < len; i++)
    {
        if((i != 0) && !(i % mod))
            printf("\n");

        printf(" %02x", buf[i]);
    }
    
    printf("\n");
}
#else
static void nand_dump_buf(const uint8_t *buf, int len, int mod) {}
#endif

static inline void denali_writel(int32_t val, void __iomem *addr)
{
	writel(val, addr);
}

static inline unsigned int denali_readl(void __iomem *addr)
{
	return readl(addr);
}

/* Certain operations for the denali NAND controller use
 * an indexed mode to read/write data. The operation is
 * performed by writing the address value of the command
 * to the device memory followed by the data. This function
 * abstracts this common operation.
 */
static void index_addr(struct denali_nand_info *denali,
        uint32_t address, uint32_t data)
{
	denali_writel(address, denali->flash_mem);    
	denali_writel(data, denali->flash_mem + 0x10);
}

/* Perform an indexed read of the device */
static void index_addr_read_data(struct denali_nand_info *denali,
        uint32_t address, uint32_t *pdata)
{
	denali_writel(address, denali->flash_mem);
	*pdata = denali_readl(denali->flash_mem + 0x10);
}

/* We need to buffer some data for some of the NAND core routines.
 * The operations manage buffering that data. 
 */
static void reset_buf(struct denali_nand_info *denali)
{
	denali->buf.head = denali->buf.tail = 0;
}

static void write_byte_to_buf(struct denali_nand_info *denali, uint8_t byte)
{
	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	BUG_ON(denali->buf.tail >= sizeof(denali->buf.buf));
	denali->buf.buf[denali->buf.tail++] = byte;
}

/* reads the status of the device */
static void read_status(struct denali_nand_info *denali)
{
	uint32_t cmd = 0x0;

	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	/* initialize the data buffer to store status */
	reset_buf(denali);

	cmd = denali_readl(denali->flash_reg + WRITE_PROTECT);
	if (cmd)
		write_byte_to_buf(denali, NAND_STATUS_WP);
	else
		write_byte_to_buf(denali, 0);
}

/* resets a specific device connected to the core */
static void reset_bank(struct denali_nand_info *denali)
{
	uint32_t req_status = 0;
	uint32_t status_mask = INTR_STATUS__RST_COMP |
        INTR_STATUS__TIME_OUT;

	clear_status(denali);

	denali_writel(1 << denali->flash_bank, denali->flash_reg + DEVICE_RESET);

	req_status = wait_for_status(denali, status_mask);

	if (req_status & INTR_STATUS__TIME_OUT)
		printf("reset bank failed.\n");
}

/* Reset the flash controller */
static uint16_t denali_nand_reset(struct denali_nand_info *denali)
{
	uint32_t i;

	for (i = 0 ; i < denali->max_banks; i++)
		denali_writel(INTR_STATUS__RST_COMP | INTR_STATUS__TIME_OUT,
		denali->flash_reg + INTR_STATUS(i));

	for (i = 0 ; i < denali->max_banks; i++) {
		denali_writel(1 << i, denali->flash_reg + DEVICE_RESET);
		while (!(denali_readl(denali->flash_reg + INTR_STATUS(i)) &
			(INTR_STATUS__RST_COMP | INTR_STATUS__TIME_OUT)));
		if (denali_readl(denali->flash_reg + INTR_STATUS(i)) &
			INTR_STATUS__TIME_OUT)
			printf("NAND Reset operation timed out on bank %d\n", i);
	}

	for (i = 0; i < denali->max_banks; i++)
		denali_writel(INTR_STATUS__RST_COMP | INTR_STATUS__TIME_OUT,
			denali->flash_reg + INTR_STATUS(i));

	denali_nand_msg(NAND_DBG_TRACE, "%s completed\n", __func__);

	return PASS;
}

/* this routine calculates the ONFI timing values for a given mode and
 * programs the clocking register accordingly. The mode is determined by
 * the get_onfi_nand_para routine.
 */
static void nand_onfi_timing_set(struct denali_nand_info *denali,
        uint16_t mode)
{
	uint16_t Trea[6] = {40, 30, 25, 20, 20, 16};
	uint16_t Trp[6] = {50, 25, 17, 15, 12, 10};
	uint16_t Treh[6] = {30, 15, 15, 10, 10, 7};
	uint16_t Trc[6] = {100, 50, 35, 30, 25, 20};
	uint16_t Trhoh[6] = {0, 15, 15, 15, 15, 15};
	uint16_t Trloh[6] = {0, 0, 0, 0, 5, 5};
	uint16_t Tcea[6] = {100, 45, 30, 25, 25, 25};
	uint16_t Tadl[6] = {200, 100, 100, 100, 70, 70};
	uint16_t Trhw[6] = {200, 100, 100, 100, 100, 100};
	uint16_t Trhz[6] = {200, 100, 100, 100, 100, 100};
	uint16_t Twhr[6] = {120, 80, 80, 60, 60, 60};
	uint16_t Tcs[6] = {70, 35, 25, 25, 20, 15};

	uint16_t TclsRising = 1;
	uint16_t data_invalid_rhoh, data_invalid_rloh, data_invalid;
	uint16_t dv_window = 0;
	uint16_t en_lo, en_hi;
	uint16_t acc_clks;
	uint16_t addr_2_data, re_2_we, re_2_re, we_2_re, cs_cnt;

	en_lo = CEIL_DIV(Trp[mode], CLK_X);
	en_hi = CEIL_DIV(Treh[mode], CLK_X);
#if ONFI_BLOOM_TIME
	if ((en_hi * CLK_X) < (Treh[mode] + 2))
		en_hi++;
#endif

	if ((en_lo + en_hi) * CLK_X < Trc[mode])
		en_lo += CEIL_DIV((Trc[mode] - (en_lo + en_hi) * CLK_X), CLK_X);

	if ((en_lo + en_hi) < CLK_MULTI)
		en_lo += CLK_MULTI - en_lo - en_hi;

	while (dv_window < 8) {
		data_invalid_rhoh = en_lo * CLK_X + Trhoh[mode];

		data_invalid_rloh = (en_lo + en_hi) * CLK_X + Trloh[mode];

		data_invalid =
		    data_invalid_rhoh <
		    data_invalid_rloh ? data_invalid_rhoh : data_invalid_rloh;

		dv_window = data_invalid - Trea[mode];

		if (dv_window < 8)
			en_lo++;
	}

	acc_clks = CEIL_DIV(Trea[mode], CLK_X);

	while (((acc_clks * CLK_X) - Trea[mode]) < 3)
		acc_clks++;

	if ((data_invalid - acc_clks * CLK_X) < 2)
		printf("%s, Line %d: Warning!\n",
			__func__, __LINE__);

	addr_2_data = CEIL_DIV(Tadl[mode], CLK_X);
	re_2_we = CEIL_DIV(Trhw[mode], CLK_X);
	re_2_re = CEIL_DIV(Trhz[mode], CLK_X);
	we_2_re = CEIL_DIV(Twhr[mode], CLK_X);
	cs_cnt = CEIL_DIV((Tcs[mode] - Trp[mode]), CLK_X);
	if (!TclsRising)
		cs_cnt = CEIL_DIV(Tcs[mode], CLK_X);
	if (cs_cnt == 0)
		cs_cnt = 1;

	if (Tcea[mode]) {
		while (((cs_cnt * CLK_X) + Trea[mode]) < Tcea[mode])
			cs_cnt++;
	}

#if MODE5_WORKAROUND
	if (mode == 5)
		acc_clks = 5;
#endif

	/* Sighting 3462430: Temporary hack for MT29F128G08CJABAWP:B */
	if ((denali_readl(denali->flash_reg + MANUFACTURER_ID) == 0) &&
		(denali_readl(denali->flash_reg + DEVICE_ID) == 0x88))
		acc_clks = 6;

	denali_writel(acc_clks, denali->flash_reg + ACC_CLKS);
	denali_writel(re_2_we, denali->flash_reg + RE_2_WE);
	denali_writel(re_2_re, denali->flash_reg + RE_2_RE);
	denali_writel(we_2_re, denali->flash_reg + WE_2_RE);
	denali_writel(addr_2_data, denali->flash_reg + ADDR_2_DATA);
	denali_writel(en_lo, denali->flash_reg + RDWR_EN_LO_CNT);
	denali_writel(en_hi, denali->flash_reg + RDWR_EN_HI_CNT);
	denali_writel(cs_cnt, denali->flash_reg + CS_SETUP_CNT);
}

/* queries the NAND device to see what ONFI modes it supports. */
static uint16_t get_onfi_nand_para(struct denali_nand_info *denali)
{
	int i;

	/* we needn't to do a reset here because driver has already
	 * reset all the banks before
	 * */
	if (!(denali_readl(denali->flash_reg + ONFI_TIMING_MODE) &
		ONFI_TIMING_MODE__VALUE))
		return FAIL;

	for (i = 5; i > 0; i--) {
		if (denali_readl(denali->flash_reg + ONFI_TIMING_MODE) &
			(0x01 << i))
			break;
	}

	nand_onfi_timing_set(denali, i);

	/* By now, all the ONFI devices we know support the page cache */
	/* rw feature. So here we enable the pipeline_rw_ahead feature */
	/* denali_writel(1, denali->flash_reg + CACHE_WRITE_ENABLE); */
	/* denali_writel(1, denali->flash_reg + CACHE_READ_ENABLE);  */

	return PASS;
}

static void get_samsung_nand_para(struct denali_nand_info *denali,
        uint8_t device_id)
{
	if (device_id == 0xd3) { /* Samsung K9WAG08U1A */
		/* Set timing register values according to datasheet */
		denali_writel(5, denali->flash_reg + ACC_CLKS);
		denali_writel(20, denali->flash_reg + RE_2_WE);
		denali_writel(12, denali->flash_reg + WE_2_RE);
		denali_writel(14, denali->flash_reg + ADDR_2_DATA);
		denali_writel(3, denali->flash_reg + RDWR_EN_LO_CNT);
		denali_writel(2, denali->flash_reg + RDWR_EN_HI_CNT);
		denali_writel(2, denali->flash_reg + CS_SETUP_CNT);
	}
}

static void get_toshiba_nand_para(struct denali_nand_info *denali)
{
	uint32_t tmp;

	/* Workaround to fix a controller bug which reports a wrong */
	/* spare area size for some kind of Toshiba NAND device */
	if ((denali_readl(denali->flash_reg + DEVICE_MAIN_AREA_SIZE) == 4096) &&
		(denali_readl(denali->flash_reg + DEVICE_SPARE_AREA_SIZE) == 64)) {
		denali_writel(216, denali->flash_reg + DEVICE_SPARE_AREA_SIZE);
		tmp = denali_readl(denali->flash_reg + DEVICES_CONNECTED) *
			denali_readl(denali->flash_reg + DEVICE_SPARE_AREA_SIZE);
		denali_writel(tmp,
				denali->flash_reg + LOGICAL_PAGE_SPARE_SIZE);
#if SUPPORT_15BITECC
		denali_writel(15, denali->flash_reg + ECC_CORRECTION);
#elif SUPPORT_8BITECC
		denali_writel(8, denali->flash_reg + ECC_CORRECTION);
#endif
	}
}

static void get_hynix_nand_para(struct denali_nand_info *denali,
        uint8_t device_id)
{
	uint32_t main_size, spare_size;

	switch (device_id) {
	case 0xD5: /* Hynix H27UAG8T2A, H27UBG8U5A or H27UCG8VFA */
	case 0xD7: /* Hynix H27UDG8VEM, H27UCG8UDM or H27UCG8V5A */
		denali_writel(128, denali->flash_reg + PAGES_PER_BLOCK);
		denali_writel(4096, denali->flash_reg + DEVICE_MAIN_AREA_SIZE);
		denali_writel(224, denali->flash_reg + DEVICE_SPARE_AREA_SIZE);
		main_size = 4096 *
			denali_readl(denali->flash_reg + DEVICES_CONNECTED);
		spare_size = 224 *
			denali_readl(denali->flash_reg + DEVICES_CONNECTED);
		denali_writel(main_size,
				denali->flash_reg + LOGICAL_PAGE_DATA_SIZE);
		denali_writel(spare_size,
				denali->flash_reg + LOGICAL_PAGE_SPARE_SIZE);
		denali_writel(0, denali->flash_reg + DEVICE_WIDTH);
#if SUPPORT_15BITECC
		denali_writel(15, denali->flash_reg + ECC_CORRECTION);
#elif SUPPORT_8BITECC
		denali_writel(8, denali->flash_reg + ECC_CORRECTION);
#endif
		break;
	default:
        printf("Spectra: Unknown Hynix NAND (Device ID: 0x%x)\
                Will use default parameter values instead.\n",
			device_id);
	}
}

/* determines how many NAND chips are connected to the controller. Note for
 * Intel CE4100 devices we don't support more than one device.
 */
static void find_valid_banks(struct denali_nand_info *denali)
{
	uint32_t id[denali->max_banks];
	int i;

	denali->total_used_banks = 1;
	for (i = 0; i < denali->max_banks; i++) {
		index_addr(denali, (uint32_t)(MODE_11 | (i << 24) | 0), 0x90);
		index_addr(denali, (uint32_t)(MODE_11 | (i << 24) | 1), 0);
		index_addr_read_data(denali,
				(uint32_t)(MODE_11 | (i << 24) | 2), &id[i]);

		denali_nand_msg(NAND_DBG_TRACE,
				"Return 1st ID for bank[%d]: %x\n", i, id[i]);

		if (i == 0) {
			if (!(id[i] & 0x0ff))
				break; /* WTF? */
		} else {
			if ((id[i] & 0x0ff) == (id[0] & 0x0ff))
				denali->total_used_banks++;
			else
				break;
		}
	}

	denali_nand_msg(NAND_DBG_DEBUG, "denali->total_used_banks: %d\n", 
            denali->total_used_banks);
}


/*
 * Use the configuration feature register to determine the maximum number of
 * banks that the hardware supports.
 */
static void detect_max_banks(struct denali_nand_info *denali)
{
	uint32_t features = denali_readl(denali->flash_reg + FEATURES);

	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	if ((features & FEATURES__N_BANKS) > CONFIG_DNL_MAX_BANK)
		denali->max_banks = CONFIG_DNL_MAX_BANK;
	else
		denali->max_banks = features & FEATURES__N_BANKS;

	denali_nand_msg(NAND_DBG_DEBUG, "denali->max_banks %d\n", denali->max_banks);
}

static void set_samsung_nand_bbtskipbytes(struct denali_nand_info *denali,
		uint8_t device_id)
{
	uint8_t skip_bytes = 0;

	/* parm from datasheet */
	if(device_id == 0xDA) {
		skip_bytes = 0x2;
	}

	denali_writel(skip_bytes, denali->flash_reg + SPARE_AREA_SKIP_BYTES);
}

static void denali_nand_bbtskipbytes_set(struct denali_nand_info *denali)
{
	uint32_t id_bytes[5], addr;
	uint8_t i, maf_id, device_id;

	/* Use read id method to get device ID and other
	 * params. For some NAND chips, controller can't
	 * report the correct device ID by reading from
	 * DEVICE_ID register
	 * */
	addr = (uint32_t)MODE_11 | BANK(denali->flash_bank);
	index_addr(denali, (uint32_t)addr | 0, 0x90);
	index_addr(denali, (uint32_t)addr | 1, 0);
	for (i = 0; i < 5; i++)
		index_addr_read_data(denali, addr | 2, &id_bytes[i]);
	maf_id = id_bytes[0];
	device_id = id_bytes[1];

	if(maf_id == 0xEC) {
		set_samsung_nand_bbtskipbytes(denali, device_id);
	}
}

static uint16_t denali_nand_timing_set(struct denali_nand_info *denali)
{
	uint16_t status = PASS;
	uint32_t id_bytes[5], addr;
	uint8_t i, maf_id, device_id;

	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	/* Use read id method to get device ID and other
	 * params. For some NAND chips, controller can't
	 * report the correct device ID by reading from
	 * DEVICE_ID register
	 * */
	addr = (uint32_t)MODE_11 | BANK(denali->flash_bank);
	index_addr(denali, (uint32_t)addr | 0, 0x90);
	index_addr(denali, (uint32_t)addr | 1, 0);
	for (i = 0; i < 5; i++)
		index_addr_read_data(denali, addr | 2, &id_bytes[i]);
	maf_id = id_bytes[0];
	device_id = id_bytes[1];
	/* init cellinfo */
	denali->cellinfo = id_bytes[2];

	denali_nand_msg(NAND_DBG_DEBUG, "maf_id 0x%02x, device_id 0x%02x\n",
            maf_id, device_id);

	if (denali_readl(denali->flash_reg + ONFI_DEVICE_NO_OF_LUNS) &
		ONFI_DEVICE_NO_OF_LUNS__ONFI_DEVICE) { /* ONFI 1.0 NAND */
		if (FAIL == get_onfi_nand_para(denali))
			return FAIL;
	} else if (maf_id == 0xEC) { /* Samsung NAND */
		get_samsung_nand_para(denali, device_id);
	} else if (maf_id == 0x98) { /* Toshiba NAND */
		get_toshiba_nand_para(denali);
	} else if (maf_id == 0xAD) { /* Hynix NAND */
		get_hynix_nand_para(denali, device_id);
	}

	denali_nand_msg(NAND_DBG_TRACE, 
			"Dump timing register values:\n \
			acc_clks: %d, re_2_we: %d, re_2_re: %d\n \
			we_2_re: %d, addr_2_data: %d, rdwr_en_lo_cnt: %d\n \
			rdwr_en_hi_cnt: %d, cs_setup_cnt: %d\n",
			denali_readl(denali->flash_reg + ACC_CLKS),
			denali_readl(denali->flash_reg + RE_2_WE),
			denali_readl(denali->flash_reg + RE_2_RE),
			denali_readl(denali->flash_reg + WE_2_RE),
			denali_readl(denali->flash_reg + ADDR_2_DATA),
			denali_readl(denali->flash_reg + RDWR_EN_LO_CNT),
			denali_readl(denali->flash_reg + RDWR_EN_HI_CNT),
			denali_readl(denali->flash_reg + CS_SETUP_CNT));

	find_valid_banks(denali);

	/* If the user specified to override the default timings
	 * with a specific ONFI mode, we apply those changes here.
	 */
	if (onfi_timing_mode != NAND_DEFAULT_TIMINGS)
		nand_onfi_timing_set(denali, onfi_timing_mode);

	return status;
}

static void denali_set_intr_modes(struct denali_nand_info *denali,
        uint16_t int_en)
{
	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	if (int_en)
		denali_writel(1, denali->flash_reg + GLOBAL_INT_ENABLE);
	else
		denali_writel(0, denali->flash_reg + GLOBAL_INT_ENABLE);
}

/* validation function to verify that the controlling software is making
 * a valid request
 */
static inline int is_flash_bank_valid(int flash_bank)
{
	return (flash_bank >= 0 && flash_bank < 4);
}

static void denali_irq_init(struct denali_nand_info *denali)
{
	uint32_t int_mask = 0;
	int i;

	/* Disable global interrupts */
	denali_set_intr_modes(denali, 0);

	int_mask = DENALI_IRQ_ALL;

	/* Clear all status bits */
	for (i = 0; i < denali->max_banks; ++i)
		denali_writel(0xFFFF, denali->flash_reg + INTR_STATUS(i));
}

/* This function only returns when an interrupt that this driver cares about
 * occurs. This is to reduce the overhead of servicing interrupts
 */
static inline uint32_t denali_status_detected(struct denali_nand_info *denali)
{
	return read_int_status(denali) & DENALI_IRQ_ALL;
}

/* Interrupts are cleared by writing a 1 to the appropriate status bit */
static inline void clear_int_status(struct denali_nand_info *denali,
        uint32_t status_mask)
{
	uint32_t intr_status_reg = 0;

	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	intr_status_reg = INTR_STATUS(denali->flash_bank);

	denali_writel(status_mask, denali->flash_reg + intr_status_reg);
}

static void clear_status(struct denali_nand_info *denali)
{
	uint32_t status = 0x0;

	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	status = read_int_status(denali);
	clear_int_status(denali, status);

	denali->int_status = 0x0;
}

static uint32_t read_int_status(struct denali_nand_info *denali)
{
	uint32_t intr_status_reg = 0;

	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	intr_status_reg = INTR_STATUS(denali->flash_bank);

	return denali_readl(denali->flash_reg + intr_status_reg);
}

#define BANK(x) ((x) << 24)

static uint32_t wait_for_status(struct denali_nand_info *denali, uint32_t status_mask)
{
	uint32_t intr_status = 0;
	int retry = 0;
	unsigned long timeout = 1000;

	denali_nand_msg(NAND_DBG_TRACE, "%s\n", __func__);

	do {
		intr_status = read_int_status(denali);

		if (intr_status & status_mask) {
			/* our status was detected */
			break;
		} else {
			/* these are not the interrupts you are looking for -
			 * need to wait again */
			retry = 1;
		}
	} while (retry && timeout--);

	if (timeout == 0) {
		/* timeout */
		printf("timeout occurred, status = 0x%x, mask = 0x%x\n",
				intr_status, status_mask);

		intr_status = 0;
	}

	return intr_status;
}

/* This helper function setups the registers for ECC and whether or not
 * the spare area will be transferred. 
 */
static void setup_ecc_for_xfer(struct denali_nand_info *denali, int ecc_en,
        int transfer_spare)
{
	int ecc_en_flag = 0, transfer_spare_flag = 0;

	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	/* set ECC, transfer spare bits if needed */
	ecc_en_flag = ecc_en ? ECC_ENABLE__FLAG : 0;
	transfer_spare_flag = transfer_spare ? TRANSFER_SPARE_REG__FLAG : 0;

	/* Enable spare area/ECC per user's request. */
	denali_writel(ecc_en_flag, denali->flash_reg + ECC_ENABLE);
	denali_writel(transfer_spare_flag,
			denali->flash_reg + TRANSFER_SPARE_REG);
}

/* sends a pipeline command operation to the controller. See the Denali NAND
 * controller's user guide for more information (section 4.2.3.6).
 */
static int denali_send_pipeline_cmd(struct denali_nand_info *denali,
        int ecc_en, int transfer_spare,
        int access_type, int op)
{
	int status = PASS;
	uint32_t addr = 0x0, cmd = 0x0, page_count = 1, int_status = 0,
		 int_mask = 0;

	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	if (op == DENALI_READ) {
		int_mask = INTR_STATUS__LOAD_COMP;
	} else if (op == DENALI_WRITE) {
		int_mask = 0;
	} else {
		printf("denali oprate mismach\n");
		status = FAIL;
		return status;
	}

	setup_ecc_for_xfer(denali, ecc_en, transfer_spare);

	/* clear interrupts */
	clear_status(denali);

	addr = BANK(denali->flash_bank) | denali->page;

	if (op == DENALI_WRITE && access_type != SPARE_ACCESS) {
		cmd = MODE_01 | addr;
		denali_writel(cmd, denali->flash_mem);
	} else if (op == DENALI_WRITE && access_type == SPARE_ACCESS) {
		/* read spare area */
		cmd = MODE_10 | addr;
		index_addr(denali, (uint32_t)cmd, access_type);

		cmd = MODE_01 | addr;
		denali_writel(cmd, denali->flash_mem);
	} else if (op == DENALI_READ) {
		/* setup page read request for access type */
		cmd = MODE_10 | addr;
		index_addr(denali, (uint32_t)cmd, access_type);

		/* page 33 of the NAND controller spec indicates we should not
		   use the pipeline commands in Spare area only mode. So we
		   don't.
		 */
		if (access_type == SPARE_ACCESS) {
			cmd = MODE_01 | addr;
			denali_writel(cmd, denali->flash_mem);
		} else {
			index_addr(denali, (uint32_t)cmd,
					0x2000 | op | page_count);

			/* wait for command to be accepted
			 * can always use status0 bit as the
			 * mask is identical for each
			 * bank. */
			int_status = wait_for_status(denali, int_mask);

			if (int_status == 0) {
				printf("cmd, page, addr on timeout(0x%x, 0x%x, 0x%x)\n",
						cmd, denali->page, addr);
				status = FAIL;
			} else {
				cmd = MODE_01 | addr;
				denali_writel(cmd, denali->flash_mem);
			}
		}
	}
	return status;
}

/* helper function that simply writes a buffer to the flash */
static int write_data_to_flash_mem(struct denali_nand_info *denali,
        const uint8_t *buf,	int len)
{
	uint32_t i = 0, *buf32;

	/* verify that the len is a multiple of 4. see comment in
	 * read_data_from_flash_mem() */
	BUG_ON((len % 4) != 0);

	/* write the data to the flash memory */
	buf32 = (uint32_t *)buf;
	for (i = 0; i < len / 4; i++)
		denali_writel(*buf32++, denali->flash_mem + 0x10);
	return i*4; /* intent is to return the number of bytes read */
}

/* helper function that simply reads a buffer from the flash */
static int read_data_from_flash_mem(struct denali_nand_info *denali,
        uint8_t *buf, int len)
{
	uint32_t i = 0, *buf32;

	/* we assume that len will be a multiple of 4, if not
	 * it would be nice to know about it ASAP rather than
	 * have random failures...
	 * This assumption is based on the fact that this
	 * function is designed to be used to read flash pages,
	 * which are typically multiples of 4...
	 */

	BUG_ON((len % 4) != 0);

	/* transfer the data from the flash */
	buf32 = (uint32_t *)buf;
	for (i = 0; i < len / 4; i++)
		*buf32++ = denali_readl(denali->flash_mem + 0x10);
	return i*4; /* intent is to return the number of bytes read */
}

/* writes OOB data to the device */
static int write_oob_data(struct mtd_info *mtd, uint8_t *buf, int page)
{
	struct denali_nand_info *denali = clouder;
	uint32_t int_status = 0;
	uint32_t status_mask = INTR_STATUS__PROGRAM_COMP |
		INTR_STATUS__PROGRAM_FAIL;
	int status = 0;

	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	denali->page = page;

	if (denali_send_pipeline_cmd(denali, 0, 0, SPARE_ACCESS,
							DENALI_WRITE) == PASS) {
		write_data_to_flash_mem(denali, buf, mtd->oobsize);

		/* wait for operation to complete */
		int_status = wait_for_status(denali, status_mask);

		if (int_status == 0) {
			printf("OOB write failed\n");
			status = -EIO;
		}
	} else {
		printf("unable to send pipeline command\n");
		status = -EIO;
	}
	return status;
}

/* reads OOB data from the device */
static void read_oob_data(struct mtd_info *mtd, uint8_t *buf, int page)
{
	struct denali_nand_info *denali = clouder;
	uint32_t int_mask = INTR_STATUS__LOAD_COMP,
			 int_status = 0, addr = 0x0, cmd = 0x0;

	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	denali->page = page;

	if (denali_send_pipeline_cmd(denali, 0, 1, SPARE_ACCESS,
							DENALI_READ) == PASS) {
		read_data_from_flash_mem(denali, buf, mtd->oobsize);

		/* wait for command to be accepted
		 * can always use status0 bit as the mask is identical for each
		 * bank. */
		int_status = wait_for_status(denali, int_mask);

		if (int_status == 0)
			printf("page on OOB timeout %d\n", denali->page);

		/* We set the device back to MAIN_ACCESS here as I observed
		 * instability with the controller if you do a block erase
		 * and the last transaction was a SPARE_ACCESS. Block erase
		 * is reliable (according to the MTD test infrastructure)
		 * if you are in MAIN_ACCESS.
		 */
		addr = BANK(denali->flash_bank) | denali->page;
		cmd = MODE_10 | addr;
		index_addr(denali, (uint32_t)cmd, MAIN_ACCESS);
	}

	/* dump buffer, DBG */
	nand_dump_buf(buf, mtd->oobsize, 16);
}

/* this function examines buffers to see if they contain data that
 * indicate that the buffer is part of an erased region of flash.
 */
int is_erased(uint8_t *buf, int len)
{
	int i = 0;
	for (i = 0; i < len; i++)
		if (buf[i] != 0xFF)
			return 0;
	return 1;
}

#define ECC_SECTOR_SIZE 512

#define ECCCORINFO_MAX_ERR(x) ((x) & ECCCORINFO_MAX_ERR_MASK)

static int handle_ecc(struct denali_nand_info *denali, uint8_t *buf,
        uint32_t int_status, unsigned int *max_bitflips)
{
	int check_erased_page = 0;
	uint8_t max_errors = 0, uncor_errors = 0;
	uint16_t err_ecccor_info, bank_ecccorinfo;

	denali_nand_msg(NAND_DBG_TRACE, "%s: buf %x bank %d\n",
			__func__, (unsigned int)buf, denali->flash_bank);

	/* Controller updates this register when it completes a transaction,
	 * controller correct itself
	 */
	if (int_status & INTR_STATUS__ECC_ERR) {
		switch(denali->flash_bank)
		{
			case 0:
			case 1:
				err_ecccor_info = denali_readl(denali->flash_reg 
						+ ECCCORINFO_B01);
				if(denali->flash_bank) {
					bank_ecccorinfo = (err_ecccor_info >> 8) & 0xff;
				} else {
					bank_ecccorinfo = err_ecccor_info & 0xff;
				}
				max_errors = ECCCORINFO_MAX_ERR(bank_ecccorinfo);
				uncor_errors = (bank_ecccorinfo >> 7) & 0x1;
				break;
			case 2:
			case 3:
				err_ecccor_info = denali_readl(denali->flash_reg 
						+ ECCCORINFO_B23);
				if(denali->flash_bank == 3) {
					bank_ecccorinfo = (err_ecccor_info >> 8) & 0xff;
				} else {
					bank_ecccorinfo = err_ecccor_info & 0xff;
				}
				max_errors = ECCCORINFO_MAX_ERR(bank_ecccorinfo);
				uncor_errors = (bank_ecccorinfo >> 7) & 0x1;
				break;
			default:
				printf("flash bank unsupport\n");
				break;
		}
	}

	if(uncor_errors) {
		/* if the error is not correctable, need to
		 * look at the page to see if it is an erased
		 * page. if so, then it's not a real ECC error
		 * */
		check_erased_page = 1;
	}

	denali_nand_msg(NAND_DBG_DEBUG, "%s max_errors %d uncor_errors %d\n",
			__func__, max_errors, uncor_errors);

	*max_bitflips = max_errors;

	return check_erased_page;
}

/* programs the controller to either enable/disable DMA transfers */
static void denali_enable_dma(struct denali_nand_info *denali, int en)
{
	uint32_t reg_val = 0x0;

	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	if (en)
		reg_val = DMA_ENABLE__FLAG;

	denali_writel(reg_val, denali->flash_reg + DMA_ENABLE);
	denali_readl(denali->flash_reg + DMA_ENABLE);
}

/* setups the HW to perform the data DMA */
static void denali_setup_dma(struct denali_nand_info *denali, int op)
{
	uint32_t mode = 0x0;
	const int page_count = 1;
	uint32_t addr = denali->buf.dma_buf;

	denali_nand_msg(NAND_DBG_TRACE, "%s: dma buffer %08x, flash_bank %d\n", 
			__func__, addr, denali->flash_bank);

	mode = MODE_10 | BANK(denali->flash_bank);

	/* DMA is a four step process */

	/* 1. setup transfer type and # of pages */
	index_addr(denali, mode | denali->page, 0x2000 | op | page_count);

	/* 2. set memory high address bits 23:8 */
	index_addr(denali, mode | ((uint16_t)(addr >> 16) << 8), 0x2200);

	/* 3. set memory low address bits 23:8 */
	index_addr(denali, mode | ((uint16_t)addr << 8), 0x2300);

	/* 4.  interrupt when complete, burst len = 64 bytes*/
	index_addr(denali, mode | 0x11000, 0x2400);
}

/* writes a page. user specifies type, and this function handles the
 * configuration details. */
static int write_page(struct mtd_info *mtd, struct nand_chip *chip,
        const uint8_t *buf, int raw_xfer)
{
	struct denali_nand_info *denali = clouder;
	uint32_t int_status = 0;
	uint32_t int_mask = INTR_STATUS__DMA_CMD_COMP |
		INTR_STATUS__PROGRAM_FAIL;

	denali_nand_msg(NAND_DBG_DEBUG, "%s buf 0x%08x raw_xfer %s\n",
			__func__, (unsigned int)buf, raw_xfer ? "yes" : "no");

	/* if it is a raw xfer, we want to disable ecc, and send
	 * the spare area.
	 * !raw_xfer - enable ecc
	 * raw_xfer - transfer spare
	 */
	setup_ecc_for_xfer(denali, !raw_xfer, raw_xfer);

	/* copy buffer into DMA buffer */
	memcpy(denali->data_buf, buf, mtd->writesize);

	/* flush dcache */
	flush_dcache_range((ulong)denali->data_buf, mtd->writesize);

	if (raw_xfer) {
		/* transfer the data to the spare area */
		memcpy(denali->data_buf + mtd->writesize,
			chip->oob_poi,
			mtd->oobsize);
		flush_dcache_range((ulong)denali->data_buf + mtd->writesize, mtd->oobsize);
	}

	/* dump buffer, DBG */
	nand_dump_buf(denali->data_buf, mtd->writesize, 16);

	clear_status(denali);
	denali_enable_dma(denali, 1);

	denali_setup_dma(denali, DENALI_WRITE);

	/* wait for operation to complete */
	int_status = wait_for_status(denali, int_mask);

	if (int_status == 0) {
		printf("timeout on write_page (type = %d)\n",
				raw_xfer);
		denali->status =
			(int_status & INTR_STATUS__PROGRAM_FAIL) ?
			NAND_STATUS_FAIL : PASS;
	}

	denali_enable_dma(denali, 0);

	return 0;
}

/* NAND core entry points */

/* this is the callback that the NAND core calls to write a page. Since
 * writing a page with ECC or without is similar, all the work is done
 * by write_page above.
 * */
static int denali_write_page(struct mtd_info *mtd, struct nand_chip *chip,
        const uint8_t *buf, int oob_required)
{
	denali_nand_msg(NAND_DBG_DEBUG, "%s buf 0x%08x\n",
			__func__, (unsigned int)buf);

	/* for regular page writes, we let HW handle all the ECC
	 * data written to the device. */
	write_page(mtd, chip, buf, 0);

	return 0;
}

/* This is the callback that the NAND core calls to write a page without ECC.
 * raw access is similar to ECC page writes, so all the work is done in the
 * write_page() function above.
 */
static int denali_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
        const uint8_t *buf, int oob_required)
{
	denali_nand_msg(NAND_DBG_DEBUG, "%s buf 0x%08x\n",
			__func__, (unsigned int)buf);

	/* for raw page writes, we want to disable ECC and simply write
	   whatever data is in the buffer. */
	write_page(mtd, chip, buf, 1);

	return 0;
}

static int denali_write_oob(struct mtd_info *mtd, struct nand_chip *chip,
        int page)
{
	denali_nand_msg(NAND_DBG_DEBUG, "%s page %d\n",
			__func__, page);

	/* dump buffer, DBG */
	nand_dump_buf(chip->oob_poi, mtd->oobsize, 16);

	return write_oob_data(mtd, chip->oob_poi, page);
}

static int denali_read_oob(struct mtd_info *mtd, struct nand_chip *chip,
        int page)
{
	denali_nand_msg(NAND_DBG_DEBUG, "%s page %d\n",
			__func__, page);

	read_oob_data(mtd, chip->oob_poi, page);

	return 0;
}

static int denali_read_page(struct mtd_info *mtd, struct nand_chip *chip,
        uint8_t *buf, int oob_required, int page)
{
	unsigned int max_bitflips;
	struct denali_nand_info *denali = clouder;
	uint32_t int_status = 0;
	uint32_t int_mask = INTR_STATUS__DMA_CMD_COMP | INTR_STATUS__ECC_ERR;
	int check_erased_page = 0;

	denali_nand_msg(NAND_DBG_DEBUG, "%s buf 0x%08x  page %d\n",
			__func__, (unsigned int)buf, page);

	if (page != denali->page) {
		printf("IN %s: page %d is not equal to denali->page %d, investigate!!",
				__func__, page, denali->page);
		mtd->ecc_stats.failed++;
		return 0;
	}

	setup_ecc_for_xfer(denali, 1, 0);

	denali_enable_dma(denali, 1);

	clear_status(denali);
	denali_setup_dma(denali, DENALI_READ);

	/* wait for operation to complete */
	int_status = wait_for_status(denali, int_mask);

	nand_dump_buf(denali->data_buf, mtd->writesize, 16);

	memcpy(buf, denali->data_buf, mtd->writesize);

	/* flush dcache */
	flush_dcache_range((ulong)buf, mtd->writesize);

	nand_dump_buf(buf, mtd->writesize, 16);

	check_erased_page = handle_ecc(denali, buf, int_status, &max_bitflips);
	denali_enable_dma(denali, 0);

	if (check_erased_page) {
		read_oob_data(mtd, chip->oob_poi, denali->page);

		/* check ECC failures that may have occurred on erased pages */
		if (check_erased_page) {
			if (!is_erased(buf, mtd->writesize))
				mtd->ecc_stats.failed++;
			if (!is_erased(buf, mtd->oobsize))
				mtd->ecc_stats.failed++;
		}
	}

	/* dump buffer, DBG */
	nand_dump_buf(buf, mtd->writesize, 16);

	return max_bitflips;
}

static int denali_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
        uint8_t *buf, int oob_required, int page)
{
	struct denali_nand_info *denali = clouder;
	uint32_t int_status = 0;
	uint32_t int_mask = INTR_STATUS__DMA_CMD_COMP;

	denali_nand_msg(NAND_DBG_DEBUG, "%s buf 0x%08x page %d\n",
			__func__, (unsigned int)buf, page);

	if (page != denali->page) {
		printf("IN %s: page %d is not equal to denali->page %d, investigate\n",
				__func__, page, denali->page);
	}

	setup_ecc_for_xfer(denali, 0, 1);
	denali_enable_dma(denali, 1);

	clear_status(denali);
	denali_setup_dma(denali, DENALI_READ);

	/* wait for operation to complete */
	int_status = wait_for_status(denali, int_mask);

	denali_enable_dma(denali, 0);

	nand_dump_buf(denali->data_buf, mtd->writesize, 16);

	memcpy(buf, denali->data_buf, mtd->writesize);
	flush_dcache_range((ulong)buf, mtd->writesize);
	memcpy(chip->oob_poi, denali->data_buf + mtd->writesize, mtd->oobsize);

	/* flush dcache */
	flush_dcache_range((ulong)chip->oob_poi, mtd->oobsize);

	/* dump buffer, DBG */
	nand_dump_buf(buf, mtd->writesize, 16);
	nand_dump_buf(chip->oob_poi, mtd->oobsize, 16);

	return 0;
}

static uint8_t denali_read_byte(struct mtd_info *mtd)
{
	struct denali_nand_info *denali = clouder;
	uint8_t result = 0xff;

	denali_nand_msg(NAND_DBG_TRACE, "%s Enter\n", __func__);

	if (denali->buf.head < denali->buf.tail)
		result = denali->buf.buf[denali->buf.head++];

	return result;
}

static void denali_select_chip(struct mtd_info *mtd, int chip)
{
	struct denali_nand_info *denali = clouder;

	denali_nand_msg(NAND_DBG_TRACE, "%s chip num %d\n",
			__func__, chip);

	denali->flash_bank = chip;
}

static int denali_waitfunc(struct mtd_info *mtd, struct nand_chip *chip)
{
	struct denali_nand_info *denali = clouder;
	int status = denali->status;
	denali->status = 0;

	denali_nand_msg(NAND_DBG_TRACE, "%s Enter status %d\n", 
			__func__, status);

	return status;
}

static void denali_erase(struct mtd_info *mtd, int page)
{
	struct denali_nand_info *denali = clouder;
	uint32_t cmd = 0x0, int_status = 0;

	denali_nand_msg(NAND_DBG_TRACE, "%s page %d\n",
			__func__, page);

	/* clear interrupts */
	clear_status(denali);

	/* setup page read request for access type */
	cmd = MODE_10 | BANK(denali->flash_bank) | page;
	index_addr(denali, (uint32_t)cmd, 0x1);

	/* wait for erase to complete or failure to occur */
	int_status = wait_for_status(denali, INTR_STATUS__ERASE_COMP |
			INTR_STATUS__ERASE_FAIL);

	denali->status = (int_status & INTR_STATUS__ERASE_FAIL) ?
		NAND_STATUS_FAIL : PASS;
}

static void denali_cmdfunc(struct mtd_info *mtd, unsigned int cmd, int col,
        int page)
{
	struct denali_nand_info *denali = clouder;
	uint32_t addr, id;
	int i;

	denali_nand_msg(NAND_DBG_DEBUG, "%s cmd(0x%02x) page(%d)\n",
			__func__, cmd, page);

	switch (cmd) {
	case NAND_CMD_PAGEPROG:
		break;
	case NAND_CMD_STATUS:
		read_status(denali);
		break;
	case NAND_CMD_READID:
	case NAND_CMD_PARAM:
		reset_buf(denali);
		/*sometimes ManufactureId read from register is not right
		 * e.g. some of Micron MT29F32G08QAA MLC NAND chips
		 * So here we send READID cmd to NAND insteand
		 * */
		addr = (uint32_t)MODE_11 | BANK(denali->flash_bank);
		index_addr(denali, (uint32_t)addr | 0, 0x90);
		index_addr(denali, (uint32_t)addr | 1, 0);
		for (i = 0; i < 5; i++) {
			index_addr_read_data(denali, (uint32_t)addr | 2, &id);
			write_byte_to_buf(denali, id);
		}
		break;
	case NAND_CMD_READ0:
	case NAND_CMD_SEQIN:
		denali->page = page;
		break;
	case NAND_CMD_RESET:
		reset_bank(denali);
		break;
	case NAND_CMD_ERASE1:
		denali_erase(mtd, page);
		break;
	case NAND_CMD_READOOB:
		reset_buf(denali);
		/* sometimes read oob data just need two bytes through 
		 * denali_read_byte function, so we should manual read 
		 * OOB data to dma buffer.
		 */
		read_oob_data(mtd, denali->buf.buf, page);
		denali->buf.tail += mtd->oobsize;
		break;
	default:
		denali_nand_msg(NAND_DBG_TRACE,
				"unsupported command received 0x%x\n", cmd);
		break;
	}
}

/* stubs for ECC functions not used by the NAND core */
static int denali_ecc_calculate(struct mtd_info *mtd, const uint8_t *data,
        uint8_t *ecc_code)
{
	printf("denali_ecc_calculate called unexpectedly\n");

	return -EIO;
}

static int denali_ecc_correct(struct mtd_info *mtd, uint8_t *data,
        uint8_t *read_ecc, uint8_t *calc_ecc)
{
	printf("denali_ecc_correct called unexpectedly\n");

	return -EIO;
}

static void denali_ecc_hwctl(struct mtd_info *mtd, int mode)
{
	printf("denali_ecc_hwctl called unexpectedly\n");

    return;
}

/* Initialization code to bring the device up to a known good state */
static void denali_hw_init(struct denali_nand_info *denali)
{
	/* tell driver how many bit controller will skip before
	 * writing ECC code in OOB, this register may be already
	 * set by firmware. So we read this value out.
	 * if this value is 0, just let it be.
	 * */
	denali->bbtskipbytes = denali_readl(denali->flash_reg +
			SPARE_AREA_SKIP_BYTES);
	detect_max_banks(denali);
	denali_nand_reset(denali);
	denali_writel(0x0F, denali->flash_reg + RB_PIN_ENABLED);
	denali_writel(CHIP_EN_DONT_CARE__FLAG,
			denali->flash_reg + CHIP_ENABLE_DONT_CARE);

	/* nand bbt skip bytes setting */
	denali_nand_bbtskipbytes_set(denali);

	denali_writel(0xffff, denali->flash_reg + SPARE_AREA_MARKER);
	denali_nand_msg(NAND_DBG_TRACE, 
			"spare area skip bytes %d spare area maker 0x%08x\n",
			denali_readl(denali->flash_reg + SPARE_AREA_SKIP_BYTES), 
			denali_readl(denali->flash_reg + SPARE_AREA_MARKER));

	/* Should set value for these registers when init */
	denali_writel(0, denali->flash_reg + TWO_ROW_ADDR_CYCLES);
	denali_writel(1, denali->flash_reg + ECC_ENABLE);

	/* timing setting */
	denali_nand_timing_set(denali);

	/* disable irq */
	denali_irq_init(denali);

	denali->pg_size = denali_readl(denali->flash_reg + 
			DEVICE_MAIN_AREA_SIZE);
	denali->oob_size = denali_readl(denali->flash_reg + 
			DEVICE_SPARE_AREA_SIZE);
	denali_nand_msg(NAND_DBG_DEBUG, 
			"denali->pg_size 0x%x denali->oob_size 0x%0x\n", 
			denali->pg_size, denali->oob_size);
}

/* Althogh controller spec said SLC ECC is forceb to be 4bit,
 * but denali controller in MRST only support 15bit and 8bit ECC
 * correction
 * */
#define ECC_8BITS	14
static struct nand_ecclayout nand_8bit_oob = {
	.eccbytes = 14,
};

#define ECC_15BITS	26
static struct nand_ecclayout nand_15bit_oob = {
	.eccbytes = 26,
};

static uint8_t bbt_pattern[] = {'B', 'b', 't', '0' };
static uint8_t mirror_pattern[] = {'1', 't', 'b', 'B' };

static struct nand_bbt_descr bbt_main_descr = {
	.options = NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE
		| NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs =	8,
	.len = 4,
	.veroffs = 12,
	.maxblocks = 4,
	.pattern = bbt_pattern,
};

static struct nand_bbt_descr bbt_mirror_descr = {
	.options = NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE
		| NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs =	8,
	.len = 4,
	.veroffs = 12,
	.maxblocks = 4,
	.pattern = mirror_pattern,
};

/* initialize driver data structures */
void denali_drv_init(struct denali_nand_info *denali)
{
	denali->idx = 0;

	/* indicate that MTD has not selected a valid bank yet */
	denali->flash_bank = CHIP_SELECT_INVALID;

	/* initialize our irq_status variable to indicate no interrupts */
	denali->int_status = 0;
}

int denali_init(struct denali_nand_info *denali)
{
	struct nand_chip *nand = denali->nand;

	/* reg&mem address */
	denali->flash_reg = (void __iomem*)CONFIG_NAND_FLASH_REG;
	denali->flash_mem = (void __iomem*)CONFIG_NAND_FLASH_RAM;

	/* dma buffer prepare */
	denali->data_buf = (uint8_t *)CONFIG_NAND_DMA_BUFFER;
	denali->buf.dma_buf = (uint32_t)(__virt_to_phys((uint32_t)denali->data_buf));

	denali_nand_msg(NAND_DBG_TRACE, 
			"flash_reg 0x%08x flash_mem 0x%08x data buf 0x%08x dma buf 0x%08x\n",
			(unsigned int)denali->flash_reg, (unsigned int)denali->flash_mem, 
			(unsigned int)denali->data_buf, denali->buf.dma_buf);

	/* the Controller will always initialize and configure itself 
	 * from chip select 0, controller assumes a homogeneous system,
	 * flash bank 0 use default.
	 */
	denali->flash_bank = 0;

	denali_hw_init(denali);

	/* driver initial */
	denali_drv_init(denali);

	/* register the driver with the NAND core subsystem */
	nand->select_chip = denali_select_chip;
	nand->cmdfunc = denali_cmdfunc;
	nand->read_byte = denali_read_byte;
	nand->waitfunc = denali_waitfunc;

	if(denali->pg_size > NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE) {
		printf("Spectra: device size not supported by this version of MTD.");
		return 1;
	}

	/* support for single nand, multi unsupport */
	denali->devnum = denali_readl(denali->flash_reg + DEVICES_CONNECTED);
	if(denali->devnum != CONFIG_SYS_NAND_MAX_CHIPS) {
		printf("denali multi nand unsupport\n");
		return 1;
	}

	/* second stage of the NAND scan
	 * this stage requires information regarding ECC and
	 * bad block management. */

	/* Bad block management */
	nand->bbt_td = &bbt_main_descr;
	nand->bbt_md = &bbt_mirror_descr;

	/* skip the scan for now until we have OOB read and write support */
	nand->options = NAND_SKIP_BBTSCAN;
	nand->ecc.mode = NAND_ECC_HW_SYNDROME;

	/* Denali Controller only support 15bit and 8bit ECC in MRST,
	 * so just let controller do 15bit ECC for MLC and 8bit ECC for
	 * SLC if possible.
	 * */
	if (denali->cellinfo & 0xc &&
		(denali->oob_size > (denali->bbtskipbytes +
			ECC_15BITS * (denali->pg_size /
				     ECC_SECTOR_SIZE)))) {
		/* if MLC OOB size is large enough, use 15bit ECC*/
		nand->ecc.layout = &nand_15bit_oob;
		nand->ecc.bytes = ECC_15BITS;
		denali_writel(15, denali->flash_reg + ECC_CORRECTION);
	} else if (denali->oob_size < (denali->bbtskipbytes +
				ECC_8BITS * (denali->pg_size /
					ECC_SECTOR_SIZE))) {
		printf("Your NAND chip OOB is not large enough to \
				contain 8bit ECC correction codes");
		return 1;
	} else {
		nand->ecc.layout = &nand_8bit_oob;
		nand->ecc.bytes = ECC_8BITS;
		denali_writel(8, denali->flash_reg + ECC_CORRECTION);
	}

	nand->ecc.layout->eccbytes *=
		denali->pg_size / ECC_SECTOR_SIZE;
	nand->ecc.layout->oobfree[0].offset =
		denali->bbtskipbytes + nand->ecc.layout->eccbytes;
	nand->ecc.layout->oobfree[0].length =
		denali->oob_size - nand->ecc.layout->eccbytes -
		denali->bbtskipbytes;

	/* These functions are required by the NAND core framework, otherwise,
	 * the NAND core will assert. However, we don't need them, so we'll stub
	 * them out. */
	nand->ecc.calculate = denali_ecc_calculate;
	nand->ecc.correct = denali_ecc_correct;
	nand->ecc.hwctl = denali_ecc_hwctl;

	/* override the default read operations */
	nand->ecc.size = ECC_SECTOR_SIZE;
	nand->ecc.read_page = denali_read_page;
	nand->ecc.read_page_raw = denali_read_page_raw;
	nand->ecc.write_page = denali_write_page;
	nand->ecc.write_page_raw = denali_write_page_raw;
	nand->ecc.read_oob = denali_read_oob;
	nand->ecc.write_oob = denali_write_oob;

	return 0;
}

int board_nand_init(struct nand_chip *chip)
{
	static int chip_n;

	if(chip_n > CONFIG_SYS_NAND_MAX_CHIPS)
		return -ENODEV;

	clouder = (struct denali_nand_info *)malloc(sizeof(struct denali_nand_info));
	if(!clouder) {
		printf("Alloc denali nand structure failed.\n");
		return -ENOMEM;
	}

	clouder->nand = chip;
	denali_nand_msg(NAND_DBG_DEBUG, "Denali nand controller init\n");
	if(denali_init(clouder))
		return -EIO;

	chip->priv = clouder;
	chip_n++;

	return 0;
}
