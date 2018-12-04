
typedef unsigned char 	uint8_t;
typedef unsigned int 	uint32_t;

#define LED_LEFT	0x1
#define LED_MEDIUM	0x2
#define LED_RIGHT	0x4

#define GPJ0CON	0xE0200240
#define GPJ0DAT	0xE0200244
#define GPD0CON	0xE02000A0
#define GPD0DAT 0xE02000A4

#define rGPJ0DAT	*((volatile uint32_t *)GPJ0DAT)
#define rGPJ0CON	*((volatile uint32_t *)GPJ0CON)

void delay_1s(void)
{
	volatile unsigned int i = 900000;
	while (i--);
}

// void led_init(void)
// {
	// rGPJ0CON = GPJ0CON_VALUE;
	// rGPD0CON = GPD0CON_VALUE;
	
	// rGPJ0DAT = ((0x1<<3)|(0x1<<4)|(0x1<<5));
	// rGPD0DAT = (0x1<<1);
// }

void led_close_all(void)
{
	rGPJ0DAT = ((0x1<<3)|(0x1<<4)|(0x1<<5));
	//rGPD0DAT = (0x1<<1);
}

void led_blink(uint8_t led_pos, uint8_t blink_num)
{
	while(blink_num)
	{
		rGPJ0DAT = ~(led_pos<<3);
		delay_1s();
		
		rGPJ0DAT = ((1<<3)|(1<<4)|(1<<5));
		delay_1s();
		
		blink_num -= 1;
	}
}

void led_open(uint8_t led_pos)
{
	rGPJ0DAT &= ~(led_pos<<3);
}

void led_close(uint8_t led_pos)
{
	rGPJ0DAT |= (led_pos<<3);		// warning: |=
}
