/* drivers/video/clouder/i80/clouder_fb_i80.c
 *
 * Core ClouderSemi I80 framebuffer driver.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <configs/clouder800.h>
#include <common.h>
#include <clouder_i80.h>
#include <linux/fb.h>
#include <cx_gpio_map.h>
#include "./logo.h"
#define CFB_NAME "I80/LCD"
#define CFB_PROMPT "I80/LCD: "
#define CFB_VSYNC_IRQ_EN 0 
#define VSYNC_TIMEOUT_MSEC 100 
#define FRAMEBUF_ADDR  0x90000000
#define __virt_to_phys(x) ((x)-0x80000000)
#define CLOUDER_VFRAME_FREQ 60
static unsigned long I80_REG_BASE = CONFIG_CLOUDER_LCDC_BASE;
unsigned int fbsize = 0;
#define PALETTE_SIZE   256
/* irq_flags bits */ 

static struct fb_var_screeninfo clouder_i80_var;
static struct fb_fix_screeninfo clouder_i80_fix;
static int i80_check_var(struct fb_var_screeninfo *var);
int lcd_id_state = 0;

#undef DEBUG
#ifdef DEBUG
#define i80_dbg(x...) printf(CFB_PROMPT x)
#else
#define i80_dbg(x...)
#endif

#ifndef FBIOGET_VBARINFO
#define FBIOGET_VBARINFO _IOR('F', 0x22, __u32)
#endif
#define FBIOSET_IMAGE    _IOW('F', 0x23, __u32)
enum {
	VGA_CTRL_CD8 = 0,
	VGA_CTRL_CD16,
	VGA_CTRL_CD24,
	VGA_CTRL_CD32,
};

enum vmp {
	VGA_AVMP_VBA = 0,
	VGA_AVMP_VBB,
	VGA_AVMP_NONE,
};

struct i80_dev {
	/* Physical and virtual addresses of control regs */ 
	u32/*phys_addr_t*/    regs_phys;
	int            regs_phys_size;
	void __iomem  *regs;

	/* Physical and virtual addresses of framebuffer */
	u32/*phys_addr_t*/  fb_phys;
	void __iomem  *fb_virt;
//	u32 pseudo_palette[PALETTE_SIZE];

	int irq;
	unsigned long vsync_count;
	unsigned long irq_flags;
	enum vmp cur;
	enum vmp prev;

	int has_acct;
	
	struct fb_var_screeninfo var;
	struct fb_fix_screeninfo fix;
	struct i80_ctrl_info *cinfo;
	struct i80_timing_info *uinfo;
	struct logo_info *klogo;
};



/* make consideration for both register and physical mem access */
inline unsigned long *reg_map(unsigned long offset)
{
	return (unsigned long*)(offset + (unsigned long)I80_REG_BASE);
}

unsigned long i80WriteReg(unsigned long addr, unsigned int data)
{
	unsigned long *reg_addr = reg_map(addr);
	*(volatile unsigned long*)reg_addr = data;

	return 0;
}

unsigned long i80ReadReg(unsigned long addr)
{
	unsigned long *reg_addr = reg_map(addr);
	unsigned long data = (*(volatile unsigned long*)reg_addr);

	return data;
}


static struct logo_info cfb_klogo = {
	.bpp = 32,
	.logo_buf = clouder_logo,
};

struct logo_info *clouder_user_klogo(void)
{
	return &cfb_klogo;
}

void copy_logo_data(char *pdst, char *psrc, int width, int height)
{
	int i = 0, row = 0, datlen = 0;
	char *pfb;
	int startx, starty;

	if ((pdst == NULL) || (psrc == NULL))
	{
		return;
	}
	if ((width == 0) || (height == 0))
	{
		return;
	}
	startx = (width - LOGO_WIDTH) / 2;
	if (startx < 0)
	{
		startx = 0;
	}
	starty = (height - LOGO_HEIGHT) / 2;
	if (starty < 0)
	{
		starty = 0;
	}
	pfb = pdst + (starty * width + startx) * 4;
	row = LOGO_HEIGHT;
	if (row > height)
	{
		row = height;
	}
	for (i = 0; i < row; i++)
	{
		memcpy_toio((void __iomem *)pfb, &psrc[datlen], WIDTH_BYTES);
		pfb += width * 4;
		datlen += WIDTH_BYTES;
	}
}

int I80_WriteCmd(u8 cmd)
{

	i80WriteReg(I80_IF_LOOP_CTRL, 0x00);		//Disable Loop function
	i80WriteReg(I80_INT_MASK, 0x00);			//Disable Interrupt

	i80WriteReg(I80_IF_CMD_IDX00, cmd);			//Write command to the cmd register
	i80WriteReg(I80_IF_CMD_ENABLE, 0x01);  		//Enable cmd index0
	i80WriteReg(I80_IF_CMD_DCX, 0x00);			//Write command
	i80WriteReg(I80_IF_CMD_WRX, 0x01);			//Write Operation

	i80WriteReg(I80_IF_CMD_START, 0x01);		//Start the Write Operation

	while (!(i80ReadReg(I80_INT_RAW) & 0x02)) {
		;	
	}
	i80WriteReg(I80_INT_CLR, 0x02); 			//Clear the Interrupt Flag

	return 0;
}

int I80_WriteData(unsigned char data)
{

	i80WriteReg(I80_IF_LOOP_CTRL, 0x00);		//Disable Loop function
	i80WriteReg(I80_INT_MASK, 0x00);			//Disable Interrupt

	i80WriteReg(I80_IF_CMD_IDX00, data);		//Write data to the cmd register
	i80WriteReg(I80_IF_CMD_ENABLE, 0x01);  		//Enable cmd index0
	i80WriteReg(I80_IF_CMD_DCX, 0x01);			//Write Data
	i80WriteReg(I80_IF_CMD_WRX, 0x01);			//Write Operation

	i80WriteReg(I80_IF_CMD_START, 0x01);		//Start the Read Operation

	while (!(i80ReadReg(I80_INT_RAW) & 0x02)) {
		;
	}
	i80WriteReg(I80_INT_CLR, 0x02);				//Clear the Interrupt Flag
	return 0;
}

int I80_WritePattern(u8* pattern, u16 DCpattern, u16 patternLen)
{
	int loop = 0;
	u32 WRFLAG = 0;

	if (12 < patternLen) {
		printf("ERROR: Pattern length is more than the Maximun length of 12.\n");
		return -1;
	}

	i80WriteReg(I80_IF_LOOP_CTRL, 0x00);		//Disable Loop function
	i80WriteReg(I80_INT_MASK, 0x00);			//Disable Interrupt
	for (loop = 0; loop < patternLen; loop++) {
		//Write patten to the cmd register
		i80WriteReg((I80_IF_CMD_IDX00 + (loop << 2)), *(pattern + loop));
		WRFLAG |= (1 << loop);
	}
	i80WriteReg(I80_IF_CMD_ENABLE, WRFLAG);  	//Write pattern enable flag
	i80WriteReg(I80_IF_CMD_DCX, DCpattern);		//Write DC pattern
	i80WriteReg(I80_IF_CMD_WRX, WRFLAG);		//Write Operation

	i80WriteReg(I80_IF_CMD_START, 0x01);		//Start the Write Operation

	while (!(i80ReadReg(I80_INT_RAW) & 0x02)) {
		;
	}
	i80WriteReg(I80_INT_CLR, 0x02);				//Clear the Interrupt Flag

	return 0;
}

int I80_ReadPattern(u8* pattern, u16 patternLen)
{
	int loop = 0;
	u32 RDFLAG = 0;

	if (12 < patternLen) {
		printf("ERROR: Pattern length is more than the Maximun length of 12.\n");
		return -1;
	}

	i80WriteReg(I80_IF_LOOP_CTRL, 0x00);		//Disable Loop function
	i80WriteReg(I80_INT_MASK, 0x00);			//Disable Interrupt
	for (loop = 0; loop < patternLen; loop++) {
		//Read patten to the cmd register
		RDFLAG |= (1 << loop);
	}
	i80WriteReg(I80_IF_CMD_ENABLE, RDFLAG);  	//Read pattern enable flag
	i80WriteReg(I80_IF_CMD_DCX, RDFLAG);		//Write DC pattern
	i80WriteReg(I80_IF_CMD_WRX, 0x00);			//Read Operation

	i80WriteReg(I80_IF_CMD_START, 0x01);		//Start the Read Operation

	while (!(i80ReadReg(I80_INT_RAW) & 0x02)) {
		;
	}
	i80WriteReg(I80_INT_CLR, 0x02);				//Clear the Interrupt Flag

	for (loop = 0; loop < patternLen; loop++) {
		pattern[loop] = (u8)i80ReadReg((I80_IF_CMD_IDX00 + (loop << 2)));
	}	

	return 0;
}

int I80_ReadWritePattern(u8* pattern, u16 DCpattern, u16 RWpattern, u16 patternLen)
{
	int loop = 0;
	u32 enFLAG = 0;

	if (12 < patternLen) {
		printf("ERROR: Pattern length is more than the Maximun length of 12.\n");
		return -1;
	}

	i80WriteReg(I80_IF_LOOP_CTRL, 0x00);		//Disable Loop function
	i80WriteReg(I80_INT_MASK, 0x00);			//Disable Interrupt
	for (loop = 0; loop < patternLen; loop++) {
		//Write patten to the cmd register
		i80WriteReg((I80_IF_CMD_IDX00 + (loop << 2)), *(pattern + loop));
		enFLAG |= (1 << loop);
	}
	i80WriteReg(I80_IF_CMD_ENABLE, enFLAG);  	//Cmd pattern enable flag
	i80WriteReg(I80_IF_CMD_DCX, DCpattern);		//Cmd DC pattern
	i80WriteReg(I80_IF_CMD_WRX, RWpattern);		//Read/Write Operation

	i80WriteReg(I80_IF_CMD_START, 0x01);		//Start the Read/Write Operation

	while (!(i80ReadReg(I80_INT_RAW) & 0x02)) {
		;
	}
	i80WriteReg(I80_INT_CLR, 0x02);				//Clear the Interrupt Flag

	return 0;
}

int I80_SetFrameAddress(u32 baseAddr0, u32 baseAddr1)
{
	u32 ba0 = baseAddr0;
	u32 ba1 = baseAddr1;

	if ((0 == ba0) || (0 == ba1)) {
		printf("Dump Physical Address Error.\n");
		return -1;
	}
	i80WriteReg(I80_FRM_BASE_ADDR0, ba0);
	i80WriteReg(I80_FRM_BASE_ADDR1, ba1);

	return 0;
}

int I80_GetFreeFrameIndex(int *index)
{
	unsigned int frameIndex = 0;

	frameIndex = i80ReadReg(I80_IF_CMD_STATUS);
	*index = ((frameIndex & 0x04) == 0);

	return 0;
}

static inline u32 i80_readl(void *base, int offset)
{
	return readl(base + offset);
}
 
static inline void i80_writel(void *base,  int offset, u32  data)
{
	writel(data, base + offset);
}

static void i80_enable_irq(struct i80_dev *fbdev)
{
	if (!test_and_set_bit(CFB_VSYNC_IRQ_EN, &fbdev->irq_flags)) {
		i80_writel(fbdev->regs, I80_INT_MASK, 0x01);
	}
}

static void i80_disable_irq(struct i80_dev *fbdev)
{
	if (test_and_clear_bit(CFB_VSYNC_IRQ_EN, &fbdev->irq_flags)) {
		i80_writel(fbdev->regs, I80_INT_MASK, 0x00);
	}
}


#define CLOUDER_PA_SCTL  	0x1FD03000
#define SYSC_BASE (CLOUDER_PA_SCTL | 0xA0000000)

void i80_interfaceEnable(void)
{
	//0x00: Enable I80 Interface; 0x01: Enable CLCDC Interface
	writel(0x00000000, (void*)(SYSC_BASE + 0x09C));
}

static void i80_setup_if(struct i80_dev *fbdev)
{
	unsigned int bus_data_num = 0;
	unsigned int frm_cyc_num_m1 = 0;
	unsigned int frm_num_loop_m1 = 10;
	struct i80_timing_info *i80_uinfo = fbdev->uinfo;
	i80_writel(fbdev->regs, I80_IF_LOOP_CTRL, 0x00);
	i80_writel(fbdev->regs, I80_FRM_SIZE, ((i80_uinfo->vv << 16) | (i80_uinfo->hv)));

	i80_writel(fbdev->regs, I80_FRM_LINE_OFF, (i80_uinfo->hv << 2));
	i80_writel(fbdev->regs, I80_FRM_INDEX_SEL, 0x00);
	i80_writel(fbdev->regs, I80_FRM_NUM_ONELOOP, frm_num_loop_m1);
	
	bus_data_num = i80_uinfo->vv * i80_uinfo->hv;
	i80_writel(fbdev->regs, I80_FRM_BUS_CYCLE, bus_data_num); //bus_data_num = Frame_VSize*Frame_HSize
	if (0 == i80_uinfo->pixel_format) {
		frm_cyc_num_m1 = bus_data_num - 1; 
	}
	else if (1 == i80_uinfo->pixel_format) {
		frm_cyc_num_m1 = (bus_data_num << 1) - 1; 
	}
	else if (2 == i80_uinfo->pixel_format) {
		frm_cyc_num_m1 = ((bus_data_num * 3) >> 1) - 1; 
	}
	else if (3 == i80_uinfo->pixel_format) {
		frm_cyc_num_m1 = (bus_data_num << 1) - 1; 
	}
	else {
		frm_cyc_num_m1 = bus_data_num * 3 - 1; 
	}
	i80_writel(fbdev->regs, I80_FRM_CYCLE_NUM, frm_cyc_num_m1);
	i80_writel(fbdev->regs, I80_IF_LOOP_CTRL, 0x00);
	/* Register framebuffer address */
	i80_writel(fbdev->regs, I80_FRM_BASE_ADDR0, fbdev->fb_phys);
	i80_writel(fbdev->regs, I80_FRM_BASE_ADDR1,
				fbdev->fb_phys + (fbdev->fix.smem_len>>1) );
	/* After init, VBA */
	fbdev->prev = VGA_AVMP_VBA;
}


static void i80_start_if(struct i80_dev *fbdev)
{
	i80_writel(fbdev->regs, I80_IF_CMD_IDX00, 0x2C);
	i80_writel(fbdev->regs, I80_IF_CMD_ENABLE, 0x1001);	//Write pattern enable flag
	i80_writel(fbdev->regs, I80_IF_CMD_DCX, 0x1000); 	//Write DC pattern
	i80_writel(fbdev->regs, I80_IF_CMD_WRX, 0x1001);	//Write Operation
	i80_writel(fbdev->regs, I80_IF_CMD_START, 0x01);
}


#ifdef CONFIG_CLOUDER_LCDC_DEBUG
static void i80_video_switch(struct i80_dev *fbdev)
{
	u32 ctrl = 0;
	u32 status = 0;
	static int is_firsr_switch = 1;
	void __iomem  *regs = fbdev->regs;
	struct timeval sys_time;
	__kernel_long_t interval = 0;

	status = i80_readl(regs, I80_IF_CMD_STATUS);
	fbdev->cur = (status & I80_STATUS_CUR_FRM) ? VGA_AVMP_VBB : VGA_AVMP_VBA;
	for (; fbdev->cur != fbdev->prev; ) {
		printf("wait for prev switch completed\n");
		status = i80_readl(regs, I80_IF_CMD_STATUS);
		fbdev->cur = (status & I80_STATUS_CUR_FRM) ? VGA_AVMP_VBB : VGA_AVMP_VBA;
	}

	if (!is_firsr_switch) {
		do_gettimeofday(&sys_time);
	}
	else {
		is_firsr_switch = 0;
	}
	interval = (sys_time.tv_sec - fbdev->lsw.tv_sec) * 1000 + 
				(sys_time.tv_usec - fbdev->lsw.tv_usec) / 1000;
	printf("%s: switch interval %lums\n", __func__, interval);

	if (fbdev->cur) {
		printf("%s: Active Video Mem Page A Switch\n", __func__);
		fbdev->prev = VGA_AVMP_VBA;
	}
	else {
		printf("%s: Active Video Mem Page B Switch\n", __func__);
		fbdev->prev = VGA_AVMP_VBB;
	}

	i80_writel(fbdev->regs, I80_FRM_INDEX_SEL, fbdev->prev);
	i80_writel(fbdev->regs, I80_IF_CMD_START, 0x01);

	do_gettimeofday(&fbdev->lsw);
}

static void i80_fill_fmem(u32 h, u32 w, u32 mem_base)
{
	u16 _w = 0;
	u16 _h = 0;
	u8 aptr = fill_cnt % 4;
	static u32 fill_cnt = 0;
	struct rgb {
		u8 r;
		u8 g;
		u8 b;
	} color_info[8] = {
		{255,  0,   0   }, 
		{0,    255, 0   },
		{0,    0,   255 },
		{0,    0,   0   },
		{255,  0,   0   }, 
		{0,    255, 0   },
		{0,    0,   255 },
		{0,    0,   0   },
	};

	printf("ClouderSemi FB Filling\n");

	/* fill vbar */ 
	for (_h = 0; _h < h; _h++) {
		u32 pixel = 0;

		for (_w = 0; _w < w; _w++) {
			if (_h < h / 4 && _w < w / 2) {
				pixel = (color_info[aptr].r << 16 |
						color_info[aptr].g << 8 | color_info[aptr].b); 
				writel(pixel, (void*)mem_base);
			}
			else if (_h < h / 4 && _w < w) {
				pixel = (color_info[aptr + 1].r << 16 |
						color_info[aptr + 1].g << 8 | color_info[aptr + 1].b); 
				writel(pixel, (void*)mem_base);
			}
			else if (_h < h / 2 && _w < w / 2) {
				pixel = (color_info[aptr + 1].r << 16 |
						color_info[aptr + 1].g << 8 | color_info[aptr + 1].b); 
				writel(pixel, (void*)mem_base);
			}
			else if (_h < h / 2 && _w < w) {
				pixel = (color_info[aptr + 2].r << 16 |
						color_info[aptr + 2].g << 8 | color_info[aptr + 2].b); 
				writel(pixel, (void*)mem_base);
			}
			else if (_h < h / 4 * 3 && _w < w / 2) {
				pixel = (color_info[aptr + 2].r << 16 |
						color_info[aptr + 2].g << 8 | color_info[aptr + 2].b); 
				writel(pixel, (void*)mem_base);
			}
			else if (_h < h / 4 * 3 && _w < w) {
				pixel = (color_info[aptr + 3].r << 16 |
						color_info[aptr + 3].g << 8 | color_info[aptr + 3].b); 
				writel(pixel, (void*)mem_base);
			}
			else if (_h < h && _w < w / 2) {
				pixel = (color_info[aptr + 3].r << 16 |
						color_info[aptr + 3].g << 8 | color_info[aptr + 3].b); 
				writel(pixel, (void*)mem_base);
			}
			else if (_h < h && _w < w){
				pixel = (color_info[aptr + 4].r << 16 |
						color_info[aptr + 4].g << 8 | color_info[aptr + 4].b); 
				writel(pixel, (void*)mem_base);
			}
			else {
				printf("mem base area unknow\n");
			}

			mem_base += 4;
		}
	}

	fill_cnt++;

}

/* Just A Simple Test */
static void i80_drv_t(struct i80_dev *fbdev)
{
	u16 w = 0;
	u16 h = 0;
	u32 ctrl = 0;
	unsigned long cs = 0;
	u32 vbara = (u32)fbdev->fb_virt;
	u32 vbarb = vbara + fix->smem_len / 2; 
	struct fb_info *fb = fbdev->info;
	struct fb_var_screeninfo *var = &fb->var;
	struct fb_fix_screeninfo *fix = &fb->fix;

	w = var->xres;
	h = var->yres;
	cs = w * h * 4;
	/* Clear Screen */
	memset((void*)vbara, 0x0, cs);
	if (fbdev->tmd) {
		memset((void*)vbarb, 0x0, cs);
	}

	i80_writel(fbdev->regs, LCDC_IF_START, 0x00);

	fbdev->prev = VGA_AVMP_VBA;
	i80_fill_fmem(h, w, vbara);
	if (fbdev->tmd) {
		i80_fill_fmem(h, w, vbarb);
	}

	i80_writel(fbdev->regs, I80_FRM_INDEX_SEL, fbdev->prev);
	i80_writel(fbdev->regs, LCDC_IF_START, 0x01);
}
#endif

static u8 i80_get_cd(struct i80_ctrl_info *cinfo)
{
	u8 cd = 0;

	switch(cinfo->cd) {
		case 3:
			cd = 32;
			break;
		case 2:
			cd = 24;
			break;
		case 1:
			cd = 16;
			break;
		case 0:
			cd = 8;
			break;
		default:
			printf("Color depth unSupport\n");
			break;
	}

	return cd;
}

/* clouder_cal_pixclock: calculating pixclock.
 * For a dotclock of e.g. 28.37516 MHz (millions of cycles per second), 
 * each pixel is 35242 ps (picoseconds) long:
 *      1/(28.37516E6 Hz) = 35.242E-9 s
 * If the screen resolution is 640x480, it will take
 *      640*35.242E-9 s = 22.555E-6 s
 * to paint the 640 (xres) pixels on one scanline. But the horizontal 
 * retrace  also takes time (e.g. 272 `pixels'), so a full scanline takes
 *      (640+272)*35.242E-9 s = 32.141E-6 s
 * We'll say that the horizontal scanrate is about 31 kHz:
 *      1/(32.141E-6 s) = 31.113E3 Hz
 * A full screen counts 480 (yres) lines, but we have to consider the vertical
 * retrace too (e.g. 49 `lines'). So a full screen will take
        (480+49)*32.141E-6 s = 17.002E-3 s
 * The vertical scanrate is about 59 Hz: 
        1/(17.002E-3 s) = 58.815 Hz 
 * This means the screen data is refreshed about 59 times per second.
 */
static unsigned int clouder_i80_cal_pixclock(struct i80_timing_info *ui)
{
	u32 result = 0;
	unsigned long sys_hz = 0;
	unsigned int pix2khz = 0;

	/* depends on Hardware pixclock, suppose it's 33MHZ, actual not */
	sys_hz = ui->hp * ui->vp * (CLOUDER_VFRAME_FREQ - 1);
	/* calculating pixclock */
	pix2khz = PICOS2KHZ(sys_hz) / 1000;
	result = KHZ2PICOS(pix2khz);

	return result;
}

static unsigned int i80_parse_set(struct i80_timing_info *ui, struct i80_dev *fbdev)
{
	int ret = 0;
	u32 smem_len = 0;
	struct fb_var_screeninfo *var = &fbdev->var;
	struct fb_fix_screeninfo *fix = &fbdev->fix;
	strcpy(fix->id, CFB_NAME);

	var->accel_flags = FB_ACCEL_NONE;
	var->activate = FB_ACTIVATE_NOW;
	var->nonstd = 0;
	var->vmode = FB_VMODE_NONINTERLACED;
	var->bits_per_pixel = i80_get_cd(fbdev->cinfo);
	var->hsync_len = ui->hpw;
	var->left_margin = ui->hbp;
	var->right_margin = ui->hfp;
	var->xres = ui->hv;
	var->xres_virtual = ui->hv;
	var->xoffset = 0;

	var->vsync_len = ui->vpw;
	var->upper_margin = ui->vbp;
	var->lower_margin = ui->vfp;
	var->yres = ui->vv;
	var->yres_virtual = (ui->vv << 1);
	var->yoffset = 0;

	var->grayscale = 0;
	var->sync = 0;
	/* Future modify, Read gralloc/framebuffer.cpp, android*/
//	var->pixclock = clouder_i80_cal_pixclock(ui);

	/* Hardward fixed ARG */
	fix->type = FB_TYPE_PACKED_PIXELS;
	if (var->bits_per_pixel == 8 && !var->grayscale) {
		fix->visual = FB_VISUAL_PSEUDOCOLOR;
	} 
	else {
		fix->visual = FB_VISUAL_TRUECOLOR;
	}

	ret = i80_check_var(var);
	if (ret < 0) {
		printf("check_var failed on initial video params\n");
		return ret;
	}

	fix->xpanstep    = 0;
	fix->ypanstep    = 1;
	fix->ywrapstep   = 0;
	fix->accel       = FB_ACCEL_NONE;
	fix->line_length = (var->bits_per_pixel * var->xres) >> 3;
	smem_len = (var->xres * var->yres * var->bits_per_pixel) >> 2;
	fix->smem_len = smem_len;
	return smem_len;
}

static int i80_setcolreg(unsigned regno, unsigned red, unsigned green,
							unsigned blue, unsigned transp, struct fb_info *info)
{

    return 0;
}

/* Impletment future */
static int i80_blank(int blank_mode, struct fb_info *info)
{
	i80_dbg("%s\n", __func__);

	switch (blank_mode) {
		/* lcd on, bl on */
		case FB_BLANK_UNBLANK:
			break;

		/* Unsupport, in this case, lcd on but bl off */
		case FB_BLANK_VSYNC_SUSPEND:
		case FB_BLANK_HSYNC_SUSPEND:
			break;

		/* lcd and bl off */
		case FB_BLANK_POWERDOWN:
			break;

		default:
			printf("input blank mode %d invalid\n", blank_mode); 
			return -EINVAL;
	}

	return 0;
} 

static int i80_check_var( struct fb_var_screeninfo *var/*,struct fb_info *info*/)
{
	if (var->yres > var->yres_virtual) {
		var->yres = var->yres_virtual;
	}
	if (var->xres > var->xres_virtual) {
		var->xres = var->xres_virtual;
	}

	switch (var->bits_per_pixel) {
		case 8:	/* PSEUDOCOLOUR, 256 */
			var->transp.offset = 0;
			var->transp.length = 0;
			var->red.offset    = 0;
			var->red.length    = 8;
			var->green.offset  = 0;
			var->green.length  = 8;
			var->blue.offset   = 0;
			var->blue.length   = 8;
			break;

		case 16:
			var->transp.offset = 0;
			var->transp.length = 0;
			var->red.offset    = 11;
			var->red.length    = 5;
			var->green.offset  = 5;
			var->green.length  = 6;
			var->blue.offset   = 0;
			var->blue.length   = 5;
			break;

		case 24:
			var->transp.offset = 0;
			var->transp.length = 0;
			var->red.offset    = 16;
			var->red.length    = 8;
			var->green.offset  = 8;
			var->green.length  = 8;
			var->blue.offset   = 0;
			var->blue.length   = 8;
			break;

		case 32:
			var->transp.offset = 24;
			var->transp.length = 8;
			var->red.offset    = 16;
			var->red.length    = 8;
			var->green.offset  = 8;
			var->green.length  = 8;
			var->blue.offset   = 0;
			var->blue.length   = 8;
		break;

		default:
			printf("invalid bpp\n");
			return -EINVAL;
	}
	return 0;
}
#if 0
static int i80_set_par(struct fb_info *info)
{
	int ret = 0;
	struct fb_var_screeninfo *var = NULL;
	struct i80_dev *fbdev = (struct i80_dev *)info;

	i80_dbg("I80 setting framebuffer parameters\n");

	if (!fbdev || !info) {
		printk(KERN_ERR CFB_PROMPT "Input ARG NULL\n");
		return -EINVAL;
	}

	var = &info->var;
	ret = i80_check_var(var);
	if (ret < 0) {
		printk(KERN_ERR CFB_PROMPT "I80 setting framebuffer failed\n");
		return ret;
	}

	var = NULL;

    return ret;
}

static int i80_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
	enum vmp avmp = VGA_AVMP_VBA;
	uint32_t status = 0;
	unsigned long flags = 0;
	struct i80_dev *fbdev = (struct i80_dev*)info->par;

	i80_dbg("%s: y_offset %d\n", __func__, var->yoffset);

	/* Calculate next fb */
	if (var->yoffset == var->yres) {
		avmp = VGA_AVMP_VBB;
	}

	if (avmp == fbdev->prev) {
		i80_dbg("%s switch overlap.\n", __func__);
		return 0;
	}
	/* Last switch completed? */
	status = i80_readl(fbdev->regs, I80_IF_CMD_STATUS);
	fbdev->cur = (status & I80_STATUS_CUR_FRM) ? VGA_AVMP_VBB : VGA_AVMP_VBA;
	//i80_dbg("%s: status:0x%x; fbdev->prev:%d; fbdev->cur:%d.\n", __func__, status, fbdev->prev, fbdev->cur);

	//for (; fbdev->prev != fbdev->cur; ) {
	//	i80_dbg("wait for prev switch completed\n");
	//	status = i80_readl(fbdev->regs, I80_INT_RAW);
	//	if (1 == (status & 0x01)) {
	//		i80_writel(fbdev->regs, I80_FRM_INDEX_SEL, fbdev->prev);
	//		i80_writel(fbdev->regs, I80_IF_CMD_START, 0x01);
	//	}
	//	status = i80_readl(fbdev->regs, I80_IF_CMD_STATUS);
	//	fbdev->cur = (status & I80_STATUS_CUR_FRM) ? VGA_AVMP_VBB : VGA_AVMP_VBA;
	//}

	spin_lock_irqsave(&fbdev->lock, flags);
	/* SWITCH */
	i80_writel(fbdev->regs, I80_FRM_INDEX_SEL, avmp);
	fbdev->prev = avmp;
	//i80_writel(fbdev->regs, I80_INT_CLR, 0x0F);
	fbdev->vsync_count = 0;
	i80_writel(fbdev->regs, I80_IF_CMD_START, 0x01);
	spin_unlock_irqrestore(&fbdev->lock, flags);

	i80_dbg("NEXT fbmem %s, PREV fbmem%s, switch\n",\
			avmp ? "VBARB" : "VBARA", fbdev->cur ? "VBARB" : "VBARA");

	return 0;
}


static int i80_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
	enum vmp avmp = VGA_AVMP_VBA;
	uint32_t status = 0, intr_state = 1;
	unsigned long flags = 0;
	struct i80_dev *fbdev = (struct i80_dev*)info->par;

	i80_dbg("%s: y_offset %d\n", __func__, var->yoffset);

	/* Calculate next fb */
	if (var->yoffset == var->yres) {
		avmp = VGA_AVMP_VBB;
	}
	if(first_boot == 1){
		first_boot = 0 ;
		i80_writel(fbdev->regs, I80_FRM_INDEX_SEL, avmp);
		i80_writel(fbdev->regs, I80_IF_CMD_START, 0x01);
		return 0;
	}
	status = i80_readl(fbdev->regs, I80_IF_CMD_STATUS);
	intr_state = i80_readl(fbdev->regs, I80_INT_RAW);
	fbdev->cur = (status & I80_STATUS_CUR_FRM) ? VGA_AVMP_VBB : VGA_AVMP_VBA;

	if(avmp == VGA_AVMP_VBA) {
		if (1 == (status & 0x01)) {
			i80_writel(fbdev->regs, I80_FRM_INDEX_SEL, avmp);
			i80_writel(fbdev->regs, I80_IF_CMD_START, 0x01);
			i80_writel(fbdev->regs, I80_INT_CLR, 0x0F);
			return 0;
		}else { 
			return 0;
		}
	}


	if(avmp == VGA_AVMP_VBB) {
		if (1 == (status & 0x01)) {
			i80_writel(fbdev->regs, I80_FRM_INDEX_SEL, avmp);
			i80_writel(fbdev->regs, I80_IF_CMD_START, 0x01);
			i80_writel(fbdev->regs, I80_INT_CLR, 0x0F);
			return 0;
		}else { 
			return 0;
		}
	}

	return 0;
}
#endif

static struct i80_dev gfbdev = {
	.regs = CONFIG_CLOUDER_LCDC_BASE,
};

static int lcd_power_enable(void) {
#if 0
#if defined(PRODUCT_CLOUDER_MINI)||defined(PRODUCT_CLOUDER_MINI2)
	unsigned char buf_2V8[2] = {0x50,0x71};
	unsigned char buf_1V8[2] = {0x53,0x24};
	unsigned char buf_en[2] = {0x44,0x90};
#endif
#if defined(PRODUCT_CLOUDER_M93)
	unsigned char buf_2V8[2] = {0x4E,0x65};
	unsigned char buf_1V8[2] = {0x4F,0x24};
	unsigned char buf_en[2] = {0x44,0x0c};
#endif
	unsigned char temp = 0;
	unsigned char buffer = 0;
	i2c_set_bus_num(CONFIG_PMU_BUS);
	i2c_write(CONFIG_SYS_I2C_SLAVE,buf_2V8[0],1,&buf_2V8,2);
	i2c_write(CONFIG_SYS_I2C_SLAVE,buf_1V8[0],1,&buf_1V8,2);
	i2c_read(CONFIG_SYS_I2C_SLAVE,0x44,1, &temp,1);
	buf_en[1] |= temp;
	i2c_write(CONFIG_SYS_I2C_SLAVE,0x44,1,&buf_en,2);
#endif
	return 0;
}

int lcd_power_disable(void) {
	unsigned char buf_en[2] = {0x44,0xf3};
	
	unsigned char temp = 0;
	i2c_read(CONFIG_SYS_I2C_SLAVE,0x44,1, &temp,1);
	buf_en[1] &= temp;
	i2c_write(CONFIG_SYS_I2C_SLAVE,0x44,1,&buf_en,2);
	return 0;
} 
void blacklight_open(void)
{
	int enable = 3;
	int disable = 0;
        writel(disable, (0xbFD36000 + 0x08));
	writel(512, (0xbFD36000 + 0xD0));
	writel(512, (0xbFD36000 + 0x00));
        writel(enable, (0xbFD36000 + 0x08));

}
int clouder_i80_init(void)
{
	int ret = 0;
	struct logo_info *klogo = NULL;
	int w, h;
	struct i80_dev* fbdev = &gfbdev;

	i80_interfaceEnable();
#if  defined(PRODUCT_CLOUDER_MINI2)||defined(PRODUCT_CLOUDER_GC100)
	ret = gpio_request(SSD2805C_RESET, "ssd2805c_reset");
	if(ret < 0) {
		printf("%s; gpio %d request failed (%d).\n", __func__, SSD2805C_RESET, ret);
		return ret;
	}
	gpio_direction_output(SSD2805C_RESET, 1);
#endif

#if  defined(PRODUCT_CLOUDER_MINI2)||defined(PRODUCT_CLOUDER_M93)||defined(PRODUCT_CLOUDER_GC100)
	ret = gpio_request(LCD_RESET, "lcd_reset");
	if(ret < 0) {
		printf("%s; gpio %d request failed (%d).\n", __func__, LCD_RESET, ret);
		return ret;
	}
	gpio_direction_output(LCD_RESET, 1);
#endif
	i80_writel(fbdev->regs, I80_INT_MASK, 0x00);
	i80_writel(fbdev->regs, I80_IF_TIME_CTRL0, 0x08080808);
	i80_writel(fbdev->regs, I80_IF_TIME_CTRL1, 0x013);
	i80_writel(fbdev->regs, I80_IF_POLARITY_CTRL, (0x04 << 12));
	SSD2805C_init();
	read_lcd_id();
	fbdev->uinfo = i80_timing_info();
	if (!fbdev->uinfo) {
		printf("Get LCDC panel ARG NULL.\n");
		return -1;
	}
	fbdev->cinfo = i80_ctrl_info();
	fbdev->vsync_count = 1;

	fbsize = i80_parse_set(fbdev->uinfo, fbdev);
	if (fbsize < 0) {
		printf("Clouder Sparse ARG Failed.\n");
		return -1;
	}
	klogo = clouder_user_klogo();
	fbdev->klogo = klogo;
	fbdev->fb_virt = klogo->logo_buf;
	fbdev->fb_phys = __virt_to_phys((unsigned int)FRAMEBUF_ADDR);
	if (!fbdev->fb_virt) {
		printf("Frame buffer memory allocation failed.\n");
		return  -1;
	}
	/* Setup and enable the framebuffer */
	i80_setup_if(fbdev);
	if (lcd_id_state == RM67160_ID) {
		init_RM67160_register();
	}
	if (lcd_id_state == ST7796S_ID) {
		init_ST7796S_register();
	}
	if (lcd_id_state == ILI9488_ID) {
		init_ILI9488_register();
	}
	memset_io((void __iomem *)FRAMEBUF_ADDR, 0, fbsize);
	switch (klogo->bpp) {
		case 32:
		//	printf("%s[%d],fix.smem_len=%d\n",__func__,__LINE__, fb->fix.smem_len);
			#if 0
			memcpy_toio((void __iomem *)fbdev->fb_virt,
						klogo->logo_buf, fb->fix.smem_len / 2);
			#endif

			/* match lcd: 320 * 320, 420 * 420 */

#ifdef  PRODUCT_CLOUDER_M93
			w = 320;
			h = 320;
#else
			w = 320;
			h = 320;
#endif
			if (lcd_id_state == RM67160_ID) {
				w = 400;
				h = 400;
			}
			copy_logo_data((char*)FRAMEBUF_ADDR, klogo->logo_buf, w, h);
			break;

		default:
			printf("logo bit depth unsupport\n");
	}

	i80_writel(fbdev->regs, I80_INT_MASK, 0x01);
	i80_writel(fbdev->regs, I80_IF_TIME_CTRL0, 0x00010303);
	i80_writel(fbdev->regs, I80_IF_TIME_CTRL1, 0x1);
	i80_start_if(fbdev);

	if (lcd_id_state != RM67160_ID)
		blacklight_open();

	return ret;
}


