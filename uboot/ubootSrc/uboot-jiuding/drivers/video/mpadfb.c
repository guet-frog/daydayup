
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#include <common.h>
#include <regs.h>
#include <asm/io.h>
#include <fb/fb.h>
#include <fb/reg-lcd.h>
#include <fb/s5pv210-fb.h>
#include <fb/fbsoft.h>
#include <fb/reg-clk.h>

#include <div64.h> //lqm.
#include <config.h>//lqm.

#ifndef mdelay
#define mdelay(x)	udelay(1000*x)
#endif

#if(DISP_MODE == AT070TN92)
static struct s5pv210fb_lcd lcd = {
	.width			= 800,
	.height			= 480,
	.bpp			= 32,
	.freq			= 60,

	.output			= S5PV210FB_OUTPUT_RGB,
	.rgb_mode		= S5PV210FB_MODE_BGR_P,
	.bpp_mode		= S5PV210FB_BPP_MODE_32BPP,
	.swap			= S5PV210FB_SWAP_WORD,

	.rgba = {
		.r_mask		= 8,
		.r_field	= 0,
		.g_mask		= 8,
		.g_field	= 8,
		.b_mask		= 8,
		.b_field	= 16,
		.a_mask		= 8,
		.a_field	= 24,
	},

	.timing = {
		.h_fp		= 210,
		.h_bp		= 38,
		.h_sw		= 10,
		.v_fp		= 22,
		.v_fpe		= 1,
		.v_bp		= 18,
		.v_bpe		= 1,
		.v_sw		= 7,
	},

	.polarity = {
		.rise_vclk	= 0,
		.inv_hsync	= 1,
		.inv_vsync	= 1,
		.inv_vden	= 0,
	},

	.vram			= CFG_LCD_FBUFFER,
};
#elif(DISP_MODE == TRULY043)
static struct s5pv210fb_lcd lcd = {
	.width			= 480,
	.height			= 272,
	.bpp			= 32,
	.freq			= 60,

	.output			= S5PV210FB_OUTPUT_RGB,
	.rgb_mode		= S5PV210FB_MODE_BGR_P,
	.bpp_mode		= S5PV210FB_BPP_MODE_32BPP,
	.swap			= S5PV210FB_SWAP_WORD,

	.rgba = {
		.r_mask		= 8,
		.r_field	= 0,
		.g_mask		= 8,
		.g_field	= 8,
		.b_mask		= 8,
		.b_field	= 16,
		.a_mask		= 8,
		.a_field	= 24,
	},

	.timing = {
		.h_fp		= 4,
		.h_bp		= 22,
		.h_sw		= 1,
		.v_fp		= 2,
		.v_fpe		= 1,
		.v_bp		= 2,
		.v_bpe		= 1,
		.v_sw		= 10,
	},

	.polarity = {
		.rise_vclk	= 0,
		.inv_hsync	= 1,
		.inv_vsync	= 1,
		.inv_vden	= 0,
	},

	.vram			= CFG_LCD_FBUFFER,
};
#elif(DISP_MODE==VGA_1024X768)
static struct s5pv210fb_lcd lcd = {
	.width			= 1024,
	.height			= 768,
	.bpp			= 32,
	.freq			= 60,

	.output			= S5PV210FB_OUTPUT_RGB,
	.rgb_mode		= S5PV210FB_MODE_BGR_P,
	.bpp_mode		= S5PV210FB_BPP_MODE_32BPP,
	.swap			= S5PV210FB_SWAP_WORD,

	.rgba = {
		.r_mask		= 8,
		.r_field	= 0,
		.g_mask		= 8,
		.g_field	= 8,
		.b_mask		= 8,
		.b_field	= 16,
		.a_mask		= 8,
		.a_field	= 24,
	},

	.timing = {
		.h_fp		= 24,
		.h_bp		= 160,
		.h_sw		= 136,
		.v_fp		= 3,
		.v_fpe		= 1,
		.v_bp		= 29,
		.v_bpe		= 1,
		.v_sw		= 6,
	},

	.polarity = {
		.rise_vclk	= 1,
		.inv_hsync	= 1,
		.inv_vsync	= 1,
		.inv_vden	= 0,
	},

	.vram			= CFG_LCD_FBUFFER,
};
#elif(DISP_MODE==VGA_800X600)
static struct s5pv210fb_lcd lcd = {
	.width			= 800,
	.height			= 600,
	.bpp			= 32,
	.freq			= 60,

	.output			= S5PV210FB_OUTPUT_RGB,
	.rgb_mode		= S5PV210FB_MODE_BGR_P,
	.bpp_mode		= S5PV210FB_BPP_MODE_32BPP,
	.swap			= S5PV210FB_SWAP_WORD,

	.rgba = {
		.r_mask		= 8,
		.r_field	= 0,
		.g_mask		= 8,
		.g_field	= 8,
		.b_mask		= 8,
		.b_field	= 16,
		.a_mask		= 8,
		.a_field	= 24,
	},

	.timing = {
		.h_fp		= 40,
		.h_bp		= 88,
		.h_sw		= 128,
		.v_fp		= 1,
		.v_fpe		= 1,
		.v_bp		= 23,
		.v_bpe		= /*1*/23,
		.v_sw		= 4,
	},

	.polarity = {
		.rise_vclk	= 1,
		.inv_hsync	= /*1*/0,
		.inv_vsync	= /*1*/0,
		.inv_vden	= 0,
	},

	.vram			= CFG_LCD_FBUFFER,
};
#elif(DISP_MODE==VGA_1440X900)
static struct s5pv210fb_lcd lcd = {
	.width			= 1440,
	.height			= 900,
	.bpp			= 32,
	.freq			= 60,

	.output			= S5PV210FB_OUTPUT_RGB,
	.rgb_mode		= S5PV210FB_MODE_BGR_P,
	.bpp_mode		= S5PV210FB_BPP_MODE_32BPP,
	.swap			= S5PV210FB_SWAP_WORD,

	.rgba = {
		.r_mask		= 8,
		.r_field	= 0,
		.g_mask		= 8,
		.g_field	= 8,
		.b_mask		= 8,
		.b_field	= 16,
		.a_mask		= 8,
		.a_field	= 24,
	},
	.timing = {
		.h_fp		= 48,
		.h_bp		= 80,
		.h_sw		= 32,
		.v_fp		= 3,
		.v_fpe		= 1,
		.v_bp		= 17,
		.v_bpe		= 1,
		.v_sw		= 6,
	},
	.polarity = {
		.rise_vclk	= 1,
		.inv_hsync	= 1,
		.inv_vsync	= /*1*/0,
		.inv_vden	= /*1*/0,
	},

	.vram			= CFG_LCD_FBUFFER,
};
#elif(DISP_MODE==VGA_1280X1024)
static struct s5pv210fb_lcd lcd = {
	.width			= 1280,
	.height			= 1024,
	.bpp			= 32,
	.freq			= 85,

	.output			= S5PV210FB_OUTPUT_RGB,
	.rgb_mode		= S5PV210FB_MODE_BGR_P,
	.bpp_mode		= S5PV210FB_BPP_MODE_32BPP,
	.swap			= S5PV210FB_SWAP_WORD,

	.rgba = {
		.r_mask		= 8,
		.r_field	= 0,
		.g_mask		= 8,
		.g_field	= 8,
		.b_mask		= 8,
		.b_field	= 16,
		.a_mask		= 8,
		.a_field	= 24,
	},

	.timing = {
		.h_fp		= 64,
		.h_bp		= 224,
		.h_sw		= 160,
		.v_fp		= 1,
		.v_fpe		= 1,
		.v_bp		= 44,
		.v_bpe		= 1,
		.v_sw		= 3,
	},

	.polarity = {
		.rise_vclk	= 1,
		.inv_hsync	= 0,
		.inv_vsync	= 0,
		.inv_vden	= 1,
	},

	.vram			= CFG_LCD_FBUFFER,
};
#endif

static struct fb_info info = {
	.name			= "fb",
};

static struct fb s5pv210_fb = {
	.info			= &info,
	.map_color		= fb_soft_map_color,
	.unmap_color	= fb_soft_unmap_color,
	.fill_rect		= fb_soft_fill_rect,
	.blit_bitmap	= fb_soft_blit_bitmap,
	.priv			= NULL,
};

static int s5pv210fb_set_output(struct s5pv210fb_lcd * lcd)
{
	u32 cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~S5PV210_VIDCON0_VIDOUT_MASK;

	if(lcd->output == S5PV210FB_OUTPUT_RGB)
		cfg |= S5PV210_VIDCON0_VIDOUT_RGB;
	else if(lcd->output == S5PV210FB_OUTPUT_ITU)
		cfg |= S5PV210_VIDCON0_VIDOUT_ITU;
	else if(lcd->output == S5PV210FB_OUTPUT_I80LDI0)
		cfg |= S5PV210_VIDCON0_VIDOUT_I80LDI0;
	else if(lcd->output == S5PV210FB_OUTPUT_I80LDI1)
		cfg |= S5PV210_VIDCON0_VIDOUT_I80LDI1;
	else if(lcd->output == S5PV210FB_OUTPUT_WB_RGB)
		cfg |= S5PV210_VIDCON0_VIDOUT_WB_RGB;
	else if(lcd->output == S5PV210FB_OUTPUT_WB_I80LDI0)
		cfg |= S5PV210_VIDCON0_VIDOUT_WB_I80LDI0;
	else if(lcd->output == S5PV210FB_OUTPUT_WB_I80LDI1)
		cfg |= S5PV210_VIDCON0_VIDOUT_WB_I80LDI1;
	else 
		return 0;
	writel(cfg, S5PV210_VIDCON0);

	cfg = readl(S5PV210_VIDCON2);
	cfg &= ~(S5PV210_VIDCON2_WB_MASK | S5PV210_VIDCON2_TVFORMATSEL_MASK | S5PV210_VIDCON2_TVFORMATSEL_YUV_MASK);

	if(lcd->output == S5PV210FB_OUTPUT_RGB)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(lcd->output == S5PV210FB_OUTPUT_ITU)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(lcd->output == S5PV210FB_OUTPUT_I80LDI0)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(lcd->output == S5PV210FB_OUTPUT_I80LDI1)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(lcd->output == S5PV210FB_OUTPUT_WB_RGB)
		cfg |= (S5PV210_VIDCON2_WB_ENABLE | S5PV210_VIDCON2_TVFORMATSEL_SW | S5PV210_VIDCON2_TVFORMATSEL_YUV444);
	else if(lcd->output == S5PV210FB_OUTPUT_WB_I80LDI0)
		cfg |= (S5PV210_VIDCON2_WB_ENABLE | S5PV210_VIDCON2_TVFORMATSEL_SW | S5PV210_VIDCON2_TVFORMATSEL_YUV444);
	else if(lcd->output == S5PV210FB_OUTPUT_WB_I80LDI1)
		cfg |= (S5PV210_VIDCON2_WB_ENABLE | S5PV210_VIDCON2_TVFORMATSEL_SW | S5PV210_VIDCON2_TVFORMATSEL_YUV444);
	else
		return 0;
	writel(cfg, S5PV210_VIDCON2);
	return 1;
}

static int s5pv210fb_set_display_mode(struct s5pv210fb_lcd * lcd)
{
	u32 cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~S5PV210_VIDCON0_PNRMODE_MASK;
	cfg |= (lcd->rgb_mode << S5PV210_VIDCON0_PNRMODE_SHIFT);
	writel(cfg, S5PV210_VIDCON0);
	return 1;
}

static int s5pv210fb_display_on(void)
{
	u32 cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg |= (S5PV210_VIDCON0_ENVID_ENABLE | S5PV210_VIDCON0_ENVID_F_ENABLE);
	writel(cfg, S5PV210_VIDCON0);
	return 1;
}

static int s5pv210fb_display_off(struct s5pv210fb_lcd * lcd)
{
	u32 cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~S5PV210_VIDCON0_ENVID_ENABLE;
	writel(cfg, S5PV210_VIDCON0);

	cfg &= ~S5PV210_VIDCON0_ENVID_F_ENABLE;
	writel(cfg, S5PV210_VIDCON0);
	return 1;
}

static int s5pv210fb_set_clock(struct s5pv210fb_lcd * lcd)
{
	u64 hclk, pixel_clock;
	u32 div;
	u32 cfg;

	//get hclk for lcd
	hclk = get_HCLKD();
	pixel_clock = ( lcd->freq * (lcd->timing.h_fp + lcd->timing.h_bp + lcd->timing.h_sw + lcd->width) *
			(lcd->timing.v_fp + lcd->timing.v_bp + lcd->timing.v_sw + lcd->height) );

	div = (u32)div64(hclk, pixel_clock);
	if(mod64(hclk, pixel_clock) > 0)
		div++;
	
	 //fixed clock source: hclk
	
	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~(S5PV210_VIDCON0_CLKSEL_MASK | S5PV210_VIDCON0_CLKVALUP_MASK | S5PV210_VIDCON0_VCLKEN_MASK | S5PV210_VIDCON0_CLKDIR_MASK);
	cfg |= (S5PV210_VIDCON0_CLKSEL_HCLK | S5PV210_VIDCON0_CLKVALUP_ALWAYS | S5PV210_VIDCON0_VCLKEN_NORMAL | S5PV210_VIDCON0_CLKDIR_DIVIDED);
	cfg |= S5PV210_VIDCON0_CLKVAL_F(div - 1);

	writel(cfg, S5PV210_VIDCON0);
	return 1; 
}

static int s5pv210fb_set_polarity(struct s5pv210fb_lcd * lcd)
{
	u32 cfg = 0;

	if(lcd->polarity.rise_vclk)
		cfg |= S5PV210_VIDCON1_IVCLK_RISING_EDGE;

	if(lcd->polarity.inv_hsync)
		cfg |= S5PV210_VIDCON1_IHSYNC_INVERT;

	if(lcd->polarity.inv_vsync)
		cfg |= S5PV210_VIDCON1_IVSYNC_INVERT;

	if(lcd->polarity.inv_vden)
		cfg |= S5PV210_VIDCON1_IVDEN_INVERT;

	cfg = 0x28c060;
	writel(cfg, S5PV210_VIDCON1);
	return 1;
}

static int s5pv210fb_set_timing(struct s5pv210fb_lcd * lcd)
{
	u32 cfg;

	cfg = 0;
	cfg |= S5PV210_VIDTCON0_VBPDE(lcd->timing.v_bpe - 1);
	cfg |= S5PV210_VIDTCON0_VBPD(lcd->timing.v_bp - 1);
	cfg |= S5PV210_VIDTCON0_VFPD(lcd->timing.v_fp - 1);
	cfg |= S5PV210_VIDTCON0_VSPW(lcd->timing.v_sw - 1);
	writel(cfg, S5PV210_VIDTCON0);

	cfg = 0;
	cfg |= S5PV210_VIDTCON1_VFPDE(lcd->timing.v_fpe - 1);
	cfg |= S5PV210_VIDTCON1_HBPD(lcd->timing.h_bp - 1);
	cfg |= S5PV210_VIDTCON1_HFPD(lcd->timing.h_fp - 1);
	cfg |= S5PV210_VIDTCON1_HSPW(lcd->timing.h_sw - 1);
	writel(cfg, S5PV210_VIDTCON1);
	return 1;
}

static int s5pv210fb_set_lcd_size(struct s5pv210fb_lcd * lcd)
{
	u32 cfg = 0;

	cfg |= S5PV210_VIDTCON2_HOZVAL(lcd->width - 1);
	cfg |= S5PV210_VIDTCON2_LINEVAL(lcd->height - 1);
	writel(cfg, S5PV210_VIDTCON2);
	return 1;
}

static int s5pv210fb_set_buffer_address(struct s5pv210fb_lcd * lcd, s32 id)
{
	u32 start, end;
	u32 shw;

	start = (u32)(lcd->vram);
	end = (u32)((start + lcd->width * (lcd->height * lcd->bpp / 8)) & 0x00ffffff);

	shw = readl(S5PV210_SHADOWCON);
	shw |= S5PV210_SHADOWCON_PROTECT(id);
	writel(shw, S5PV210_SHADOWCON);

	switch(id)
	{
	case 0:
		writel(start, S5PV210_VIDW00ADD0B0);
		writel(end, S5PV210_VIDW00ADD1B0);
		break;

	case 1:
		writel(start, S5PV210_VIDW01ADD0B0);
		writel(end, S5PV210_VIDW01ADD1B0);
		break;

	case 2:
		writel(start, S5PV210_VIDW02ADD0B0);
		writel(end, S5PV210_VIDW02ADD1B0);
		break;

	case 3:
		writel(start, S5PV210_VIDW03ADD0B0);
		writel(end, S5PV210_VIDW03ADD1B0);
		break;

	case 4:
		writel(start, S5PV210_VIDW04ADD0B0);
		writel(end, S5PV210_VIDW04ADD1B0);
		break;

	default:
		break;
	}

	shw = readl(S5PV210_SHADOWCON);
	shw &= ~(S5PV210_SHADOWCON_PROTECT(id));
	writel(shw, S5PV210_SHADOWCON);
	return 1;
}

static int s5pv210fb_set_buffer_size(struct s5pv210fb_lcd * lcd, s32 id)
{
	u32 cfg = 0;

	cfg = S5PV210_VIDADDR_PAGEWIDTH(lcd->width * lcd->bpp / 8);
	cfg |= S5PV210_VIDADDR_OFFSIZE(0);

	switch(id)
	{
	case 0:
		writel(cfg, S5PV210_VIDW00ADD2);
		break;

	case 1:
		writel(cfg, S5PV210_VIDW01ADD2);
		break;

	case 2:
		writel(cfg, S5PV210_VIDW02ADD2);
		break;

	case 3:
		writel(cfg, S5PV210_VIDW03ADD2);
		break;

	case 4:
		writel(cfg, S5PV210_VIDW04ADD2);
		break;

	default:
		return 0;
	}
	return 1;
}

static int s5pv210fb_set_window_position(struct s5pv210fb_lcd * lcd, s32 id)
{
	u32 cfg, shw;

	shw = readl(S5PV210_SHADOWCON);
	shw |= S5PV210_SHADOWCON_PROTECT(id);

	writel(shw, S5PV210_SHADOWCON);

	switch(id)
	{
	case 0:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(cfg, S5PV210_VIDOSD0A);
		cfg = S5PV210_VIDOSD_RIGHT_X(lcd->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(lcd->height - 1);
		writel(cfg, S5PV210_VIDOSD0B);
		break;

	case 1:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(cfg, S5PV210_VIDOSD1A);
		cfg = S5PV210_VIDOSD_RIGHT_X(lcd->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(lcd->height - 1);
		writel(cfg, S5PV210_VIDOSD1B);
		break;

	case 2:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(cfg, S5PV210_VIDOSD2A);
		cfg = S5PV210_VIDOSD_RIGHT_X(lcd->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(lcd->height - 1);
		writel(cfg, S5PV210_VIDOSD2B);
		break;

	case 3:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(cfg, S5PV210_VIDOSD3A);
		cfg = S5PV210_VIDOSD_RIGHT_X(lcd->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(lcd->height - 1);
		writel(cfg, S5PV210_VIDOSD3B);
		break;

	case 4:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(cfg, S5PV210_VIDOSD4A);
		cfg = S5PV210_VIDOSD_RIGHT_X(lcd->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(lcd->height - 1);
		writel(cfg, S5PV210_VIDOSD4B);
		break;

	default:
		break;
	}

	shw = readl(S5PV210_SHADOWCON);
	shw &= ~(S5PV210_SHADOWCON_PROTECT(id));
	writel(shw, S5PV210_SHADOWCON);
	return 1;
}

static int s5pv210fb_set_window_size(struct s5pv210fb_lcd * lcd, s32 id)
{
	u32 cfg;

	cfg = S5PV210_VIDOSD_SIZE(lcd->width * lcd->height);

	switch(id)
	{
	case 0:
		writel(cfg, S5PV210_VIDOSD0C);
		break;

	case 1:
		writel(cfg, S5PV210_VIDOSD1D);
		break;

	case 2:
		writel(cfg, S5PV210_VIDOSD2D);
		break;

	default:
		return 0;
	}
	return 1;
}

static int s5pv210fb_window2_enable(struct s5pv210fb_lcd * lcd)
{
	u32 cfg;

	//window control
	 
	cfg = readl(S5PV210_WINCON2);
	cfg &= ~(S5PV210_WINCON_BITSWP_ENABLE | S5PV210_WINCON_BYTESWP_ENABLE |
				S5PV210_WINCON_HAWSWP_ENABLE | S5PV210_WINCON_WSWP_ENABLE |
				S5PV210_WINCON_BURSTLEN_MASK | S5PV210_WINCON_BPPMODE_MASK |
				S5PV210_WINCON_INRGB_MASK | S5PV210_WINCON_DATAPATH_MASK |
				S5PV210_WINCON_ALPHA_SEL_MASK);

	cfg |= S5PV210_WINCON_ALPHA0_SEL;
	cfg |= S5PV210_WINCON_INRGB_RGB;
	cfg |= S5PV210_WINCON_DATAPATH_DMA;
	cfg |= S5PV210_WINCON_ENWIN_ENABLE;

	if(lcd->swap & S5PV210FB_SWAP_WORD)
		cfg |= S5PV210_WINCON_WSWP_ENABLE;
	else
		cfg |= S5PV210_WINCON_WSWP_DISABLE;

	if(lcd->swap & S5PV210FB_SWAP_HWORD)
		cfg |= S5PV210_WINCON_HAWSWP_ENABLE;
	else
		cfg |= S5PV210_WINCON_HAWSWP_DISABLE;

	if(lcd->swap & S5PV210FB_SWAP_BYTE)
		cfg |= S5PV210_WINCON_BYTESWP_ENABLE;
	else
		cfg |= S5PV210_WINCON_BYTESWP_DISABLE;

	if(lcd->swap & S5PV210FB_SWAP_BIT)
		cfg |= S5PV210_WINCON_BITSWP_ENABLE;
	else
		cfg |= S5PV210_WINCON_BITSWP_DISABLE;

	cfg |= (lcd->bpp_mode << S5PV210_WINCON_BPPMODE_SHIFT);

	writel(cfg, S5PV210_WINCON2);
	 
	cfg = readl(S5PV210_SHADOWCON);
	cfg |= 1 << 2;
	writel(cfg, S5PV210_SHADOWCON);
	return 1;
}

///* //lqm test
void fb_init()
{
	get_HCLKD();
	info.bitmap.info.width = lcd.width;
	info.bitmap.info.height = lcd.height;
	info.bitmap.info.bpp = lcd.bpp;
	info.bitmap.info.bytes_per_pixel = lcd.bpp / 8;
	info.bitmap.info.pitch = lcd.width * lcd.bpp / 8;

	info.bitmap.info.red_mask_size = lcd.rgba.r_mask;
	info.bitmap.info.red_field_pos = lcd.rgba.r_field;
	info.bitmap.info.green_mask_size = lcd.rgba.g_mask;
	info.bitmap.info.green_field_pos = lcd.rgba.g_field;
	info.bitmap.info.blue_mask_size = lcd.rgba.b_mask;
	info.bitmap.info.blue_field_pos = lcd.rgba.b_field;
	info.bitmap.info.alpha_mask_size = lcd.rgba.a_mask;
	info.bitmap.info.alpha_field_pos = lcd.rgba.a_field;

	info.bitmap.info.fmt = get_bitmap_format(&(info.bitmap.info));

	info.bitmap.info.fg_r = 0xff;
	info.bitmap.info.fg_g = 0xff;
	info.bitmap.info.fg_b = 0xff;
	info.bitmap.info.fg_a = 0xff;

	info.bitmap.info.bg_r = 0x00;
	info.bitmap.info.bg_g = 0x00;
	info.bitmap.info.bg_b = 0x00;
	info.bitmap.info.bg_a = 0x00;

	info.bitmap.viewport.left = 0;
	info.bitmap.viewport.top = 0;
	info.bitmap.viewport.right = lcd.width;
	info.bitmap.viewport.bottom = lcd.height;

	info.bitmap.allocated = 0;
	info.bitmap.data = lcd.vram;
}
//*/

void lcd_port_init()
{
	writel(0x22222222, GPF0CON);
	writel(0xffffffff, GPF0DRV);
	writel(0x0, GPF0PUD);
	writel(0x22222222, GPF1CON);
	writel(0xffffffff, GPF1DRV);
	writel(0x0, GPF1PUD);
	writel(0x22222222, GPF2CON);
	writel(0xffffffff, GPF2DRV);
	writel(0x0, GPF2PUD);
	writel((readl(GPF3CON) & ~(0xffff<<0)) | (0x2222<<0), GPF3CON);
	writel((readl(GPF3DRV) & ~(0xff<<0)) | (0xff<<0), GPF3DRV);
	writel((readl(GPF3PUD) & ~(0xff<<0)) | (0x00<<0), GPF3PUD);

	
	 //set gpd0_0 (backlight pwm pin) output and pull up and high level for disabled
	writel((readl(GPD0CON) & ~(0xf<<0)) | (0x1<<0), GPD0CON);
	writel((readl(GPD0PUD) & ~(0x3<<0)) | (0x2<<0), GPD0PUD);
	writel((readl(GPD0DAT) & ~(0x1<<0)) | (0x1<<0), GPD0DAT);

	 //set gpf3_5 (backlight enable pin) output and pull up and low level for disabled
	writel((readl(GPF3CON) & ~(0xf<<20)) | (0x1<<20), GPF3CON);
	writel((readl(GPF3PUD) & ~(0x3<<10)) | (0x2<<10), GPF3PUD);
	writel((readl(GPF3DAT) & ~(0x1<<5)) | (0x0<<5), GPF3DAT);
	
	 //set gph1_6 (LCD_5V) output and pull up and high level for enabled

	writel((readl(GPH1CON) & ~(0xf<<24)) | (0x1<<24), GPH1CON);
	writel((readl(GPH1PUD) & ~(0x3<<12)) | (0x2<<12), GPH1PUD);
	writel((readl(GPH1DAT) & ~(0x1<<6)) | (0x1<<6), GPH1DAT);

	
	 //set gph1_7 (LCD_33) output and pull up and low level for enabled
	 
	writel((readl(GPH1CON) & ~(0xf<<28)) | (0x1<<28), GPH1CON);
	writel((readl(GPH1PUD) & ~(0x3<<14)) | (0x2<<14), GPH1PUD);
	writel((readl(GPH1DAT) & ~(0x1<<7)) | (0x0<<7), GPH1DAT);

	 //wait a moment
	 
	mdelay(100);
}

void lcd_reg_init()
{
	 //display path selection
	 
	writel((readl(S5PV210_DISPLAY_CONTROL) & ~(0x3<<0)) | (0x2<<0), S5PV210_DISPLAY_CONTROL);

	 //turn all windows off
	 
	writel((readl(S5PV210_WINCON0) & ~0x1), S5PV210_WINCON0);
	writel((readl(S5PV210_WINCON1) & ~0x1), S5PV210_WINCON1);
	writel((readl(S5PV210_WINCON2) & ~0x1), S5PV210_WINCON2);
	writel((readl(S5PV210_WINCON3) & ~0x1), S5PV210_WINCON3);
	writel((readl(S5PV210_WINCON4) & ~0x1), S5PV210_WINCON4);

	
	 //turn all windows color map off
	 
	writel((readl(S5PV210_WIN0MAP) & ~(1<<24)), S5PV210_WIN0MAP);
	writel((readl(S5PV210_WIN1MAP) & ~(1<<24)), S5PV210_WIN1MAP);
	writel((readl(S5PV210_WIN2MAP) & ~(1<<24)), S5PV210_WIN2MAP);
	writel((readl(S5PV210_WIN3MAP) & ~(1<<24)), S5PV210_WIN3MAP);
	writel((readl(S5PV210_WIN4MAP) & ~(1<<24)), S5PV210_WIN4MAP);

	
	 //turn all windows color key off
	 
	writel((readl(S5PV210_W1KEYCON0) & ~(3<<25)), S5PV210_W1KEYCON0);
	writel((readl(S5PV210_W2KEYCON0) & ~(3<<25)), S5PV210_W2KEYCON0);
	writel((readl(S5PV210_W3KEYCON0) & ~(3<<25)), S5PV210_W3KEYCON0);
	writel((readl(S5PV210_W4KEYCON0) & ~(3<<25)), S5PV210_W4KEYCON0);
	

	 //initial lcd controller

	s5pv210fb_set_output(&lcd);
	s5pv210fb_set_display_mode(&lcd);
	s5pv210fb_display_off(&lcd);
	s5pv210fb_set_polarity(&lcd);
	s5pv210fb_set_timing(&lcd);
	s5pv210fb_set_lcd_size(&lcd);
	s5pv210fb_set_clock(&lcd);


	 //set lcd video buffer

	s5pv210fb_set_buffer_address(&lcd, 2);
	s5pv210fb_set_buffer_size(&lcd, 2);
	s5pv210fb_set_window_position(&lcd, 2);
	s5pv210fb_set_window_size(&lcd, 2);
	

	 //enable window 2 for main display area
	 
	s5pv210fb_window2_enable(&lcd);

	 //display on
	s5pv210fb_display_on();

	 //wait a moment
	 
	mdelay(100);
}


void backlight_brigness_init(u8 brightness)
{
	if(brightness)
	{
		writel((readl(GPF3DAT) & ~(0x1<<5)) | (0x1<<5), GPF3DAT);
		writel((readl(GPD0DAT) & ~(0x1<<0)) | (0x0<<0), GPD0DAT);
	}
	else
	{
		writel((readl(GPF3DAT) & ~(0x1<<5)) | (0x0<<5), GPF3DAT);
		writel((readl(GPD0DAT) & ~(0x1<<0)) | (0x1<<0), GPD0DAT);
	}
}




void mpadfb_init()
{
//	unsigned short int *pFB;//这里一定要用short类型!
//	int i;
	fb_init();//lqm masked for test
	lcd_port_init();
	lcd_reg_init();
#ifdef CONFIG_CHECK_X210CV3
	init_logo();
#endif
	display_logo(&s5pv210_fb);
#if(DISP_MODE == TRULY043)
	backlight_brigness_init(0);
#else//AT070TN92
	backlight_brigness_init(1);
#endif

//	pFB = (unsigned short int *)CFG_LCD_FBUFFER;
	/*
        for(i=0; i<800*480; i++)
        	*pFB++ = 0xf800;
	*/
/*	 for(i=0; i<800*100; i++)
        *pFB++ = 0xf800;
	 for(i=800*100; i<800*200; i++)
        *pFB++ = 0x07e0;
	 for(i=800*200; i<800*480; i++)
        *pFB++ = 0x001f;
*/

#if(DISP_MODE == TRULY043)
	writel((readl(GPF3DAT) & ~(0x1<<5)) | (0x1<<5), GPF3DAT);
#endif
}
