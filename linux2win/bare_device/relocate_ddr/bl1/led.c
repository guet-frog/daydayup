#include "led.h"

void led_init(void)
{
	rGPJ0CON = GPJ0CON_VALUE;
	rGPD0CON = GPD0CON_VALUE;
	
	rGPJ0DAT = ((0x1<<3)|(0x1<<4)|(0x1<<5));
	rGPD0DAT = (0x1<<1);
}

void led_close_all(void)
{
	rGPJ0DAT = ((0x1<<3)|(0x1<<4)|(0x1<<5));
	rGPD0DAT = (0x1<<1);
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

void delay_1s(void)
{
	volatile unsigned int i = 900000;
	while (i--);
}

	
