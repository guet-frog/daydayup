/*+ * @file clouder_i80.h
+ *
+ * @brief I80 system initialization and file operation definition
+ *
+ * @ingroup I80
+ */

#ifndef __CLOUDER_I80_H__
#define __CLOUDER_I80_H__

//#include <linux/fs.h>

#include <common.h>
#include <asm/errno.h>
#include <linux/list.h>
#include <malloc.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <asm/byteorder.h>
#include <asm/unaligned.h>
#include <asm/types.h>
#include <asm/io.h>
#include <usb/lin_gadget_compat.h>
#include <cx_gpio_map.h>
//#include "cx-regs-otg.h"
#define I80_REG_MARGIN			0xFF

#define LCD_RESET	GPIO15_A7

#ifdef	PRODUCT_CLOUDER_MINI2
#define SSD2805C_RESET	GPIO15_A5
#endif

#ifdef	PRODUCT_CLOUDER_GC100
#define SSD2805C_RESET	GPIO15_A6
#endif

#ifdef	PRODUCT_CLOUDER_M93
#define SSD2805C_RESET	GPIO15_A6
#endif
//------------------------------------------------------------------------------
// I80 IF CONTROL REGISTER
//------------------------------------------------------------------------------
#define I80_IF_TIME_CTRL0		0x00
#define I80_IF_TIME_CTRL1		0x04
#define I80_IF_LOOP_CTRL		0x08
#define I80_IF_POLARITY_CTRL		0x0C
#define I80_IF_CMD_ENABLE		0x40
#define I80_IF_CMD_DCX			0x44
#define I80_IF_CMD_WRX			0x48
#define I80_IF_CMD_START		0x4C
#define I80_IF_CMD_IDX00		0x50
#define I80_IF_CMD_IDX01		0x54
#define I80_IF_CMD_IDX02		0x58
#define I80_IF_CMD_IDX03		0x5C
#define I80_IF_CMD_IDX04		0x60
#define I80_IF_CMD_IDX05		0x64
#define I80_IF_CMD_IDX06		0x68
#define I80_IF_CMD_IDX07		0x6C
#define I80_IF_CMD_IDX08		0x70
#define I80_IF_CMD_IDX09		0x74
#define I80_IF_CMD_IDX10		0x78
#define I80_IF_CMD_IDX11		0x7C
#define I80_IF_CMD_STATUS		0x80

//------------------------------------------------------------------------------
// FRAME CONTROL REGISTER
//------------------------------------------------------------------------------
#define I80_FRM_SIZE			0x20
#define I80_FRM_BASE_ADDR0		0x24
#define I80_FRM_BASE_ADDR1		0x28
#define I80_FRM_LINE_OFF		0x2C
#define I80_FRM_INDEX_SEL		0x30
#define I80_FRM_NUM_ONELOOP		0x34
#define I80_FRM_CYCLE_NUM		0x38
#define I80_FRM_BUS_CYCLE		0x3C
//------------------------------------------------------------------------------
// INTERRUPT REGISTER
//------------------------------------------------------------------------------
#define I80_INT_MASK			0x10
#define I80_INT_CLR			0x14
#define I80_INT_RAW			0x18
#define I80_INT_STATUS			0x1C

#define I80_INTR_FRM_DONE	(1 << 0)

#define I80_STATUS_CUR_FRM	(1 << 2)

#define RM67160_ID	1
#define ST7796S_ID	2
#define ILI9488_ID	3


struct i80_timing_info {
	u16 hp;
	u16 hpw;
	u16 hbp;
	u16 hfp;
	u16 hv;
	u16 vp;
	u16 vpw;
	u16 vbp;
	u16 vfp;
	u16 vv;

	u8 if_bits;
	u8 pixel_format;
	u8 timing_period;
	u8 reserved;	
};

/* sync polarization level */
struct plevel {
	u8 bl:1;
	u8 pl:1;
	u8 hsl:1;
	u8 vsl:1;
};      
        
struct i80_ctrl_info {
	u8 cd:2; 
	u8 pc:1;
	struct plevel pl;
};

struct logo_info {
	u8 bpp;
	char *logo_buf;
};


extern unsigned long i80WriteReg(unsigned long addr, unsigned int data);
extern unsigned long i80ReadReg(unsigned long addr);
extern int I80_WriteCmd(u8 cmd);
extern int I80_WriteData(unsigned char data);
extern int I80_WritePattern(u8* pattern, u16 DCpattern, u16 patternLen);
extern int I80_ReadPattern(u8* pattern, u16 patternLen);
extern int I80_ReadWritePattern(u8* pattern, u16 DCpattern, u16 RWpattern, u16 patternLen);
extern int I80_SetFrameAddress(u32 baseAddr0, u32 baseAddr1);
extern int I80_GetFreeFrameIndex(int *index);

extern struct i80_timing_info *i80_timing_info(void);
extern struct i80_ctrl_info *i80_ctrl_info(void);
extern int SSD2805C_init(void);
extern int i80_lcd_test(int r, int g, int b);
extern int read_lcd_id(void);
extern int init_ILI9488_register(void);
extern int init_RM67160_register(void);
extern int init_ST7796S_register(void);
extern int lcd_id_state; 
extern int lcd_exit_sleep(void);
extern int lcd_enter_sleep(void);
extern void init_lcd_driver(void);
extern void exit_sleep(void);
extern void enter_sleep(void);

#endif  //__CLOUDER_I80_H__

