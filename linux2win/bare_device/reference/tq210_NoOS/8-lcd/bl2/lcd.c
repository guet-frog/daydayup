#include "lcd.h"

#define GPD0CON			*((volatile u32 *)0xE02000A0)	// 00000001
#define GPD0DAT			*((volatile u32 *)0xE02000A4)	// 00000001

#define GPF0CON			*((volatile u32 *)0xE0200120)	// 22222222
#define GPF1CON			*((volatile u32 *)0xE0200140)	// 22222222
#define GPF2CON			*((volatile u32 *)0xE0200160)	// 22222222
#define GPF3CON			*((volatile u32 *)0xE0200180)	// 00002222

#define DISPLAY_CONTROL	*((volatile u32 *)0xE0107008)	// 00000002

#define VIDCON0			*((volatile u32 *)0xF8000000)	// 41b10133
#define VIDCON1			*((volatile u32 *)0xF8000004)	// 01b6c060
#define VIDCON2			*((volatile u32 *)0xF8000008)	// 00000000
#define VIDTCON0		*((volatile u32 *)0xF8000010)	// 0009150c
#define VIDTCON1		*((volatile u32 *)0xF8000014)	// 001a0d13
#define VIDTCON2		*((volatile u32 *)0xF8000018)	// 000efb1f
#define VIDTCON3		*((volatile u32 *)0xF800001C)	// 00000000
#define WINCON0			*((volatile u32 *)0xF8000020)	// 0000802d
#define SHODOWCON		*((volatile u32 *)0xF8000034)	// 00000001
#define VIDOSD0A		*((volatile u32 *)0xF8000040)	// 00000000
#define VIDOSD0B		*((volatile u32 *)0xF8000044)	// 003203bf
#define VIDOSD0C		*((volatile u32 *)0xF8000048)	// 000bb800
#define VIDW00ADD0B0	*((volatile u32 *)0xF80000A0)	// 28000000
#define VIDW00ADD1B0	*((volatile u32 *)0xF80000D0)	// 28177c80

#define VBPD	23
#define VFPD	22
#define VSPW	2
#define HEIGHT	480

#define HBPD	46
#define HFPD	210
#define HSPW	2
#define WIDTH	800
#define BPP		24

static u32 bytes_per_line;
static u32 frame_buffer_size;
static u32 frame_buffer_addr = 0x28000000;

void lcd_init()
{
	GPF0CON  = 0x22222222;						/* LCD_VD[3:0] LCD_VCLK LCD_VDEN LCD_VSYNC LCD_HSYNC */
	GPF1CON  = 0x22222222;						/* LCD_VD[11:4] */
	GPF2CON	 = 0x22222222;						/* LCD_VD[19:12] */
	GPF3CON	 = 0x00002222;						/* LCD_VD[23:20] */
	GPD0CON  = (GPD0CON & ~0xF) | (0x1 << 0);	/* LCD PWR(backlight) */
	
	DISPLAY_CONTROL = (2 << 0);					/* Display path selection:RGB=FIMD I80=FIMD ITU=FIMD */
	
	bytes_per_line = WIDTH;
	if (BPP > 16)
		bytes_per_line *= 32;
	else
		bytes_per_line *= BPP;
	bytes_per_line /= 8;
	
	frame_buffer_size = bytes_per_line * HEIGHT;
	
	/* Configures video output format and displays enable/disable */
	VIDCON0  = 	(5 << 6) |				/* CLKVAL = 4, HCLK(166MHz) / (5 + 1) = 27MHz */
				(1 << 4) |				/* Selects the clock source as divide using CLKVAL_F */
				(0 << 2);				/* Selects the video clock source:HCLK=166MHz */
	
	/* Specifies RGB I/F control signal */
	VIDCON1  = 	(0 << 7) |				/* Video data is fetched at VCLK falling edge */
				(1 << 6) |				/* Inverted HSYNC pulse polarity */
				(1 << 5) |				/* Inverted VSYNC pulse polarity */
				(0 << 4);				/* Normal VDEN signal polarity ?????*/
	
	/* Specifies output data format control */
	VIDCON2  = 	(0b000 << 19) |			/* RGB interface output order(Even line, line #2,4,6,8):RGB */
				(0b000 << 16);			/* RGB interface output order(Odd line, line #1,3,5,7):RGB */
	
	/* Configures video output timing and determines the size of display */
	VIDTCON0 =	(VBPD << 16) |			/* VBPD */
				(VFPD << 8) |			/* VFPD */
				(VSPW << 0);			/* VSPW */
				
	VIDTCON1 =	(HBPD << 16) |			/* HBPD */
				(HFPD << 8) |			/* HFPD */
				(HSPW << 0);			/* HSPW */

	VIDTCON2 = 	((HEIGHT - 1) << 11) |	/* vertical size of display(LINEVAL + 1) */
				(WIDTH - 1);			/* horizontal size of display(HOZVAL + 1) */

	VIDTCON3 =	(0b1 << 31);			/* Enables VSYNC Signal Output */
	
	WINCON0  =	(0 << 30) |				/* BUFSEL_H = 0 */
				(0 << 20) |				/* BUFSEL_L = 0, BUFSEL=0b00(Selects the Buffer set 0) */
				(1 << 15) |				/* the Word swap Enable */
				(0xB << 2);				/* Unpacked 24 bpp ( non-palletized R:8-G:8-B:8 ) */
	
	/* Specifies window position setting */
	VIDOSD0A = 	(0 << 11) | 0;			/* LeftTop */

	VIDOSD0B = 	((WIDTH - 1) << 11) |	/* RightBotX */
				(HEIGHT - 1);			/* RightBotY */
	
	/* Specifies the Window Size:Height * Width (Number of Word) */
	VIDOSD0C = frame_buffer_size / 4;
	
	/* the start address for Video frame buffer */
	VIDW00ADD0B0 =  frame_buffer_addr;
	VIDW00ADD1B0 =  VIDW00ADD0B0 + frame_buffer_size;
	
	SHODOWCON = 0x1;	/* Enables Channel 0 */
}

void backlight_ctl(u8 ctl)
{
	if (ctl)
		GPD0DAT |= 1;
	else
		GPD0DAT &= ~1;
}

void lcd_enable(u8 enable)
{
	if (enable)
	{	
		VIDCON0 |= 0x3;
		WINCON0 |= 1;
	}
	else
	{
		WINCON0 &= ~1;
		
		/* see the note in the framebuffer datasheet about why you
		** cannot take both of these bits down at the same time. */
		if (VIDCON0 & (1 << 1))
			VIDCON0 &= ~1;
	}
}

void draw_pixel(int x, int y, u32 color)
{
	u32 *p = (u32 *)frame_buffer_addr;
	*(p + y * bytes_per_line / 4 + x) = color;
}

void clear_screen()
{
	int x, y;
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
			draw_pixel(x, y, 0);
	}
}

void draw_line(int x0, int y0, int x1, int y1, u32 color)
{
	int x, y, dx, dy, Dx, Dy, e, i;
	Dx = x1 - x0;
	Dy = y1 - y0;

	dx = x1 - x0;
	if (dx < 0)
		dx *= -1;
	dy = y1 - y0;
	if (dy < 0)
		dy *= -1;
	x = x0;
	y = y0;
	if(dy > dx)
	{
		e = -dy;
		for(i = 0; i < dy; i++)
		{
			draw_pixel(x, y, color);
			if(Dy >= 0)
				y++;  
			else
				y--;   
			e += 2 * dx;
			if(e >= 0)
			{
				if(Dx >= 0)
					x++;
				else
					x--; 
				e -= 2 * dy;
			}
		}
	}
	else
	{
		e = -dx;
		for(i = 0; i < dx; i++)
		{
			draw_pixel(x, y, color);
			if(Dx >= 0)
				x++;
			else
				x--;
			e += 2 * dy;
			if(e >= 0)
			{
				if(Dy >= 0)
					y++;
				else
					y--;
				e -= 2 * dx;
			}
		}
	}
}

void draw_circle(int x, int y, int r, u32 color)
{
	int a, b, num; 
	a = 0; 
	b = r; 
	while(2 * b * b >= r * r) 
	{ 
		draw_pixel(x + a, y - b,color); 
		draw_pixel(x - a, y - b,color); 
		draw_pixel(x - a, y + b,color); 
		draw_pixel(x + a, y + b,color); 
		draw_pixel(x + b, y + a,color); 
		draw_pixel(x + b, y - a,color); 
		draw_pixel(x - b, y - a,color); 
		draw_pixel(x - b, y + a,color); 
		a++; 
		num = (a * a + b * b) - r*r; 
		if(num > 0) 
		{ 
			b--; 
			a--; 
		} 
	} 
}
