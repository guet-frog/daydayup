#ifndef _LED_H
#define _LED_H
#include <stdint.h>

#define GPJ0CON_VALUE	((0x1<<12)|(0x1<<16)|(0x1<<20))
#define GPD0CON_VALUE	(0x1<<4)

#define GPJ0CON	0xE0200240
#define GPJ0DAT	0xE0200244
#define GPD0CON	0xE02000A0
#define GPD0DAT 0xE02000A4

#define rGPJ0CON	(*((volatile uint32_t *)GPJ0CON))
#define rGPJ0DAT	(*((volatile uint32_t *)GPJ0DAT))
#define rGPD0CON	(*((volatile uint32_t *)GPD0CON))
#define rGPD0DAT	(*((volatile uint32_t *)GPD0DAT))

#define LED_LEFT	0x1
#define LED_MEDIUM	0x2
#define LED_RIGHT	0x4

void delay_1s(void);
void led_init(void);
void led_close_all(void);
void led_open(uint8_t led_pos);
void led_close(uint8_t led_pos);
void led_blink(uint8_t led_pos, uint8_t blink_num);


#endif


// D22 GPJ0_3 低电平点亮 -> 左

// D23 GPJ0_4 低电平点亮

// D24 GPJ0_5 低电平点亮

// D25 GPD0_1 低电平点亮(PWMTOUT1)