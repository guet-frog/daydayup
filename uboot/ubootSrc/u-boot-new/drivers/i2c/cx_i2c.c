/*
 * (C) Copyright 2009
 * Vipin Kumar, ST Micoelectronics, vipin.kumar@st.com.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <linux/err.h>
#include <linux/errno.h>
#include <linux/bitops.h>
#include <common.h>
#include <asm/io.h>
#include <linux/list.h>
#include <malloc.h>
#include <cx_i2c.h>

/* bus */
static unsigned int current_bus = 0;

/* Active adapter */
static struct cx_i2c_dev *i2c_adapter;

/* adapter list */
LIST_HEAD(adapters);

static char *abort_sources[] = {
	[ABRT_7B_ADDR_NOACK] =
		"slave address not acknowledged (7bit mode)",
	[ABRT_10ADDR1_NOACK] =
		"first address byte not acknowledged (10bit mode)",
	[ABRT_10ADDR2_NOACK] =
		"second address byte not acknowledged (10bit mode)",
	[ABRT_TXDATA_NOACK] =
		"data not acknowledged",
	[ABRT_GCALL_NOACK] =
		"no acknowledgement for a general call",
	[ABRT_GCALL_READ] =
		"read after general call",
	[ABRT_SBYTE_ACKDET] =
		"start byte acknowledged",
	[ABRT_SBYTE_NORSTRT] =
		"trying to send start byte when restart is disabled",
	[ABRT_10B_RD_NORSTRT] =
		"trying to read when restart is disabled (10bit mode)",
	[ABRT_MASTER_DIS] =
		"trying to use disabled adapter",
	[ARB_LOST] =
		"lost arbitration",
};

struct i2c_msg {
	u16 addr;		 /* slave address                        */
	u16 flags;
#define I2C_M_TEN               0x0010  /* this is a ten bit chip address */
#define I2C_M_RD                0x0001  /* read data, from slave to master */
#define I2C_M_NOSTART           0x4000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_REV_DIR_ADDR      0x2000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK        0x1000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NO_RD_ACK         0x0800  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_RECV_LEN          0x0400  /* length will be first received byte */
	u16 len;                /* msg length                           */
	u8 *buf;                /* pointer to msg data                  */

};

struct cx_i2c_dev *bus_to_adapter(u8 bus)
{
	struct cx_i2c_dev *dev = NULL;

	list_for_each_entry(dev, &adapters, adapter) {
		if (dev->bus == bus)
			return dev;
	}

	return dev;
}

static u32 cx_readl(struct cx_i2c_dev *dev, int offset)
{
	return readl(dev->base + offset);
}

static void cx_writel(struct cx_i2c_dev *dev, u32 b, int offset)
{
	writel(b, dev->base + offset);
}

static u32 i2c_cx_scl_hcnt(u32 ic_clk, u32 tSYMBOL, u32 tf, int cond, int offset)
{
	if (cond)
		return (ic_clk * tSYMBOL + 5000) / 10000000 - 8 + offset;
	else
		return (ic_clk * (tSYMBOL + tf) + 5000) / 10000000 - 3 + offset;
}

static u32 i2c_cx_scl_lcnt(u32 ic_clk, u32 tLOW, u32 tf, int offset)
{
	return ((ic_clk * (tLOW + tf) + 5000) / 10000000) - 1 + offset;
}

static int i2c_cx_bus_probe(struct cx_i2c_dev *dev)
{
	u32 hcnt, lcnt;

	cx_writel(dev, 0, CX_IC_ENABLE);

	hcnt = i2c_cx_scl_hcnt(dev->clock,
				40,	/* tHD;STA = tHIGH = 4.0 us */
				3,	/* tf = 0.3 us */
				0,	/* 0: DW default, 1: Ideal */
				0);	/* No offset */
	lcnt = i2c_cx_scl_lcnt(dev->clock,
				47,	/* tLOW = 4.7 us */
				3,	/* tf = 0.3 us */
				0);	/* No offset */
	cx_writel(dev, hcnt, CX_IC_SS_SCL_HCNT);
	cx_writel(dev, lcnt, CX_IC_SS_SCL_LCNT);

	hcnt = i2c_cx_scl_hcnt(dev->clock,
				6,	/* tHD;STA = tHIGH = 0.6 us */
				3,	/* tf = 0.3 us */
				0,	/* 0: DW default, 1: Ideal */
				0);	/* No offset */
	lcnt = i2c_cx_scl_lcnt(dev->clock,
				13,	/* tLOW = 1.3 us */
				3,	/* tf = 0.3 us */
				0);	/* No offset */
	cx_writel(dev, hcnt, CX_IC_FS_SCL_HCNT);
	cx_writel(dev, lcnt, CX_IC_FS_SCL_LCNT);

	cx_writel(dev, dev->tx_fifo_depth - 1, CX_IC_TX_TL);
	cx_writel(dev, 0, CX_IC_RX_TL);

	cx_writel(dev, dev->master_cfg , CX_IC_CON);

	return 0;
}

static int i2c_cx_wait_bus_not_busy(struct cx_i2c_dev *dev)
{
	int timeout = TIMEOUT;

	while (cx_readl(dev, CX_IC_STATUS) & CX_IC_STATUS_ACTIVITY) {
		if (timeout <= 0) {
			printf("timeout waiting for bus ready\n");
			return -ETIMEDOUT;
		}

		timeout--;
	}

	return 0;
}

static void i2c_cx_xfer_init(struct cx_i2c_dev *dev)
{
        struct i2c_msg *msgs = dev->msgs;
        u32 ic_con;

        cx_writel(dev, 0, CX_IC_ENABLE);

        cx_writel(dev, msgs[dev->msg_write_idx].addr, CX_IC_TAR);

        ic_con = cx_readl(dev, CX_IC_CON);
        if (msgs[dev->msg_write_idx].flags & I2C_M_TEN)
                ic_con |= CX_IC_CON_10BITADDR_MASTER;
        else
                ic_con &= ~CX_IC_CON_10BITADDR_MASTER;
        cx_writel(dev, ic_con, CX_IC_CON);

        cx_writel(dev, 1, CX_IC_ENABLE);
}

void i2c_cx_xfer_msg(struct cx_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	int tx_limit, rx_limit;
	u32 addr = msgs[dev->msg_write_idx].addr;
	u32 buf_len = dev->tx_buf_len;
	u8 *buf = dev->tx_buf;

	for (; dev->msg_write_idx < dev->msgs_num; dev->msg_write_idx++) {
		if (msgs[dev->msg_write_idx].addr != addr) {
			printf("%s: invalid target address\n", __func__);
			dev->msg_err = -EINVAL;
			break;
		}

		if (msgs[dev->msg_write_idx].len == 0) {
			printf("%s: invalid message length\n", __func__);
			dev->msg_err = -EINVAL;
			break;
		}

		if (!(dev->status & STATUS_WRITE_IN_PROGRESS)) {
			buf = msgs[dev->msg_write_idx].buf;
			buf_len = msgs[dev->msg_write_idx].len;
		}

		tx_limit = dev->tx_fifo_depth - cx_readl(dev, CX_IC_TXFLR);
		rx_limit = dev->rx_fifo_depth - cx_readl(dev, CX_IC_RXFLR);

		while (buf_len > 0 && tx_limit > 0 && rx_limit > 0) {
			u32 cmd = 0;

			if (dev->msg_write_idx == dev->msgs_num -1 && buf_len == 1)
				cmd |= (1 << 9);

			if (msgs[dev->msg_write_idx].flags & I2C_M_RD) {
				cx_writel(dev, cmd | 0x100, CX_IC_DATA_CMD);
				rx_limit--;
			} else {
				cx_writel(dev, cmd | *buf++ , CX_IC_DATA_CMD);
			}
			tx_limit--; buf_len--;
		}

		dev->tx_buf = buf;
		dev->tx_buf_len = buf_len;

		if (buf_len > 0) {
			dev->status |= STATUS_WRITE_IN_PROGRESS;
			break;
		} else
			dev->status &= ~STATUS_WRITE_IN_PROGRESS;
	}
}

static void i2c_cx_read(struct cx_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	int rx_valid;

	for (; dev->msg_read_idx < dev->msgs_num; dev->msg_read_idx++) {
		u32 len;
		u8 *buf;

		if (!(msgs[dev->msg_read_idx].flags & I2C_M_RD))
			continue;

		if (!(dev->status & STATUS_READ_IN_PROGRESS)) {
			len = msgs[dev->msg_read_idx].len;
			buf = msgs[dev->msg_read_idx].buf;
		} else {
			len = dev->rx_buf_len;
			buf = dev->rx_buf;
		}

		rx_valid = cx_readl(dev, CX_IC_RXFLR);

		for (; len > 0 && rx_valid > 0; len--, rx_valid--) {
			*buf++ = cx_readl(dev, CX_IC_DATA_CMD);
		}

		if (len > 0) {
			dev->status |= STATUS_READ_IN_PROGRESS;
			dev->rx_buf_len = len;
			dev->rx_buf = buf;
			return;
		} else
			dev->status &= ~STATUS_READ_IN_PROGRESS;
	}
}

static int i2c_cx_handle_tx_abort(struct cx_i2c_dev *dev)
{
	unsigned long abort_source = dev->abort_source;
	int i;

	if (abort_source & CX_IC_TX_ABRT_NOACK) {
		for_each_set_bit(i, &abort_source, ARRAY_SIZE(abort_sources))
			printf("%s: %s\n", __func__, abort_sources[i]);

		return -EREMOTEIO;
	}

	for_each_set_bit(i, &abort_source, ARRAY_SIZE(abort_sources))
		printf("%s: %s\n", __func__, abort_sources[i]);

	if (abort_source & CX_IC_TX_ARB_LOST)
		return -EAGAIN;
	else if (abort_source & CX_IC_TX_ABRT_GCALL_READ)
		return -EINVAL; /* wrong msgs[] data */
	else
		return -EIO;
}

void i2c_cx_poll(struct cx_i2c_dev *dev)
{
	u32 stat, enabled;

	if (!dev)
		return;

	while(1) {
		enabled = cx_readl(dev, CX_IC_ENABLE);
		stat = cx_readl(dev, CX_IC_RAW_INTR_STAT);
		
		if (!enabled || !(stat & ~CX_IC_INTR_ACTIVITY)) {
			break;
		}

		if (stat & CX_IC_INTR_TX_ABRT) {
			dev->cmd_err |= CX_IC_ERR_TX_ABRT;
			dev->status = STATUS_IDLE;

			goto tx_aborted;
		}

		if (stat & CX_IC_INTR_RX_FULL)
			i2c_cx_read(dev);

		if (stat & CX_IC_INTR_TX_EMPTY)
			i2c_cx_xfer_msg(dev);

tx_aborted:
		if ((stat & (CX_IC_INTR_TX_ABRT | CX_IC_INTR_STOP_DET)) || dev->msg_err) {
			break;
		}
	}
}

static int i2c_cx_xfer(struct cx_i2c_dev *dev, struct i2c_msg msgs[], int num)
{
	int ret = 0;

	dev->msgs = msgs;
	dev->msgs_num = num;
	dev->cmd_err = 0;
	dev->msg_write_idx = 0;
	dev->msg_read_idx = 0;
	dev->msg_err = 0;
	dev->status = STATUS_IDLE;
	dev->abort_source = 0;

	ret = i2c_cx_wait_bus_not_busy(dev);
	if (ret < 0)
		goto done;

	i2c_cx_xfer_init(dev);

	/* xmit */
	i2c_cx_poll(dev);

	if (dev->msg_err) {
		ret = dev->msg_err;
		goto done;
	}

	if (!dev->cmd_err) {
		cx_writel(dev, 0, CX_IC_ENABLE);
		ret = num;
		goto done;
	}

	if (dev->cmd_err == CX_IC_ERR_TX_ABRT) {
		ret = i2c_cx_handle_tx_abort(dev);
		goto done;
	}

done:
	return ret;
}

static void i2c_cx_clear_int(struct cx_i2c_dev *dev)
{
	cx_readl(dev, CX_IC_CLR_INTR);
}

static void i2c_cx_disable_int(struct cx_i2c_dev *dev)
{
	cx_writel(dev, 0, CX_IC_INTR_MASK);
}

static u32 i2c_cx_read_comp_param(struct cx_i2c_dev *dev)
{
	return cx_readl(dev, CX_IC_COMP_PARAM_1);
}

void i2c_cx_bus_init(struct i2c_board_info *info)
{
	struct cx_i2c_dev *dev;
	u32 param1;
	int ret;

	dev = (struct cx_i2c_dev *)malloc(sizeof(*dev));
	if (!dev)
		return;

	memset(dev, 0x0, sizeof(*dev));

	INIT_LIST_HEAD(&dev->adapter);

	dev->base = (void*)CKSEG1ADDR(info->mmap);
	dev->bus = info->bus;
	dev->clock = info->clock;
	dev->master_cfg = CX_IC_CON_MASTER | CX_IC_CON_SLAVE_DISABLE | 
			  CX_IC_CON_RESTART_EN | CX_IC_CON_SPEED_STD;

	/* fifo level */
	param1 = i2c_cx_read_comp_param(dev);
	dev->tx_fifo_depth = ((param1 >> 16) & 0xff) + 1;
	dev->rx_fifo_depth = ((param1 >> 8)  & 0xff) + 1;

	ret = i2c_cx_bus_probe(dev);
	if (ret)
		goto unreg;

	i2c_cx_disable_int(dev);
	i2c_cx_clear_int(dev);
	list_add_tail(&dev->adapter, &adapters);

	debug("i2c bus %d registered\n", info->bus);

	return;

unreg:
	free(dev);
}

/*
 * i2c_read - Read from i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Read from i2c memory.
 */
int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	struct i2c_msg msg[2] = { 
		{ chip, 0, 1, &addr},
		{ chip, I2C_M_RD, len, buffer}
	};

	i2c_cx_xfer(i2c_adapter, msg, 2);

	return len;
}

/*
 * i2c_write - Write to i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Write to i2c memory.
 */
int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	struct i2c_msg msg = { chip, 0, len, buffer};

	i2c_cx_xfer(i2c_adapter, &msg, 1);

	return len;
}

int i2c_set_bus_num(unsigned int bus)
{
	/* Active bus */
	current_bus = bus;

	/* setting active adapter */
	i2c_adapter = bus_to_adapter(bus);

	return 0;
}

int i2c_get_bus_num(void)
{
	return current_bus;
}
