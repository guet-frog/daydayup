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

#ifndef __CX_I2C_H_
#define __CX_I2C_H_

#define CX_IC_CON_MASTER		0x1
#define CX_IC_CON_SPEED_STD		0x2
#define CX_IC_CON_SPEED_FAST		0x4
#define CX_IC_CON_10BITADDR_MASTER	0x10
#define CX_IC_CON_RESTART_EN		0x20
#define CX_IC_CON_SLAVE_DISABLE		0x40

/*
 * Registers offset
 */
#define CX_IC_CON       0x0
#define CX_IC_TAR       0x4
#define CX_IC_DATA_CMD      0x10
#define CX_IC_SS_SCL_HCNT   0x14
#define CX_IC_SS_SCL_LCNT   0x18
#define CX_IC_FS_SCL_HCNT   0x1c
#define CX_IC_FS_SCL_LCNT   0x20
#define CX_IC_INTR_STAT     0x2c
#define CX_IC_INTR_MASK     0x30
#define CX_IC_RAW_INTR_STAT 0x34
#define CX_IC_RX_TL     0x38
#define CX_IC_TX_TL     0x3c
#define CX_IC_CLR_INTR      0x40
#define CX_IC_CLR_RX_UNDER  0x44
#define CX_IC_CLR_RX_OVER   0x48
#define CX_IC_CLR_TX_OVER   0x4c
#define CX_IC_CLR_RD_REQ    0x50
#define CX_IC_CLR_TX_ABRT   0x54
#define CX_IC_CLR_RX_DONE   0x58
#define CX_IC_CLR_ACTIVITY  0x5c
#define CX_IC_CLR_STOP_DET  0x60
#define CX_IC_CLR_START_DET 0x64
#define CX_IC_CLR_GEN_CALL  0x68
#define CX_IC_ENABLE        0x6c
#define CX_IC_STATUS        0x70
#define CX_IC_TXFLR     0x74
#define CX_IC_RXFLR     0x78
#define CX_IC_TX_ABRT_SOURCE    0x80
#define CX_IC_COMP_PARAM_1  0xf4
#define CX_IC_COMP_TYPE     0xfc
#define CX_IC_COMP_TYPE_VALUE   0x44570140

#define CX_IC_INTR_RX_UNDER 0x001
#define CX_IC_INTR_RX_OVER  0x002
#define CX_IC_INTR_RX_FULL  0x004
#define CX_IC_INTR_TX_OVER  0x008
#define CX_IC_INTR_TX_EMPTY 0x010
#define CX_IC_INTR_RD_REQ   0x020
#define CX_IC_INTR_TX_ABRT  0x040
#define CX_IC_INTR_RX_DONE  0x080
#define CX_IC_INTR_ACTIVITY 0x100
#define CX_IC_INTR_STOP_DET 0x200
#define CX_IC_INTR_START_DET    0x400
#define CX_IC_INTR_GEN_CALL 0x800

#define CX_IC_INTR_DEFAULT_MASK     (CX_IC_INTR_RX_FULL | \
                     CX_IC_INTR_TX_EMPTY | \
                     CX_IC_INTR_TX_ABRT | \
                     CX_IC_INTR_STOP_DET)

#define CX_IC_STATUS_ACTIVITY   0x1

#define CX_IC_ERR_TX_ABRT   0x1

/*
 * status codes
 */
#define STATUS_IDLE         0x0
#define STATUS_WRITE_IN_PROGRESS    0x1
#define STATUS_READ_IN_PROGRESS     0x2

#define TIMEOUT         20

/*
 * hardware abort codes from the CX_IC_TX_ABRT_SOURCE register
 *
 * only expected abort codes are listed here
 * refer to the datasheet for the full list
 */
#define ABRT_7B_ADDR_NOACK  0
#define ABRT_10ADDR1_NOACK  1
#define ABRT_10ADDR2_NOACK  2
#define ABRT_TXDATA_NOACK   3
#define ABRT_GCALL_NOACK    4
#define ABRT_GCALL_READ     5
#define ABRT_SBYTE_ACKDET   7
#define ABRT_SBYTE_NORSTRT  9
#define ABRT_10B_RD_NORSTRT 10
#define ABRT_MASTER_DIS     11
#define ARB_LOST        12

#define CX_IC_TX_ABRT_7B_ADDR_NOACK (1UL << ABRT_7B_ADDR_NOACK)
#define CX_IC_TX_ABRT_10ADDR1_NOACK (1UL << ABRT_10ADDR1_NOACK)
#define CX_IC_TX_ABRT_10ADDR2_NOACK (1UL << ABRT_10ADDR2_NOACK)
#define CX_IC_TX_ABRT_TXDATA_NOACK  (1UL << ABRT_TXDATA_NOACK)
#define CX_IC_TX_ABRT_GCALL_NOACK   (1UL << ABRT_GCALL_NOACK)
#define CX_IC_TX_ABRT_GCALL_READ    (1UL << ABRT_GCALL_READ)
#define CX_IC_TX_ABRT_SBYTE_ACKDET  (1UL << ABRT_SBYTE_ACKDET)
#define CX_IC_TX_ABRT_SBYTE_NORSTRT (1UL << ABRT_SBYTE_NORSTRT)
#define CX_IC_TX_ABRT_10B_RD_NORSTRT    (1UL << ABRT_10B_RD_NORSTRT)
#define CX_IC_TX_ABRT_MASTER_DIS    (1UL << ABRT_MASTER_DIS)
#define CX_IC_TX_ARB_LOST       (1UL << ARB_LOST)

#define CX_IC_TX_ABRT_NOACK     (CX_IC_TX_ABRT_7B_ADDR_NOACK | \
                     CX_IC_TX_ABRT_10ADDR1_NOACK | \
                     CX_IC_TX_ABRT_10ADDR2_NOACK | \
                     CX_IC_TX_ABRT_TXDATA_NOACK | \
                     CX_IC_TX_ABRT_GCALL_NOACK)

/**
 * struct cx_i2c_dev - private i2c-designware data
 * @cmd_err: run time hadware error code
 * @msgs: points to an array of messages currently being transfered
 * @msgs_num: the number of elements in msgs
 * @msg_write_idx: the element index of the current tx message in the msgs
 *	array
 * @tx_buf_len: the length of the current tx buffer
 * @tx_buf: the current tx buffer
 * @msg_read_idx: the element index of the current rx message in the msgs
 *	array
 * @rx_buf_len: the length of the current rx buffer
 * @rx_buf: the current rx buffer
 * @msg_err: error status of the current transfer
 * @status: i2c master status, one of STATUS_*
 * @abort_source: copy of the TX_ABRT_SOURCE register
 * @irq: interrupt number for the i2c master
 * @tx_fifo_depth: depth of the hardware tx fifo
 * @rx_fifo_depth: depth of the hardware rx fifo
 */
struct cx_i2c_dev {
	struct	list_head	adapter;
	u8			bus;
	unsigned int		clock;
	void			*base;
	int			cmd_err;
	struct i2c_msg		*msgs;
	int			msgs_num;
	int			msg_write_idx;
	int			cmd_complete;
	u32			tx_buf_len;
	u8			*tx_buf;
	int			msg_read_idx;
	u32			rx_buf_len;
	u8			*rx_buf;
	int			msg_err;
	unsigned int		status;
	u32			abort_source;
	int			irq;
	int			swab;
	u32			functionality;
	u32			master_cfg;
	unsigned int		tx_fifo_depth;
	unsigned int		rx_fifo_depth;
};

struct i2c_board_info {
	u32			mmap;
	u8			bus;
	u32			clock;
};

#endif /* __CX_I2C_H_ */

