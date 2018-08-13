#include "led.h"
#include "uart.h"
#include "lcd.h"
#include "./include/stdio.h"

// void bsp_init(void)
// {
	// led_init();
	
	// uart_init(UART2_115200);
	
	// clock_init();
	
	// print_cpuinfo();
// }

int main(void)
{
	lcd_init();
	backlight_ctl(1);
	lcd_enable(1);
	clear_screen();
	
	while(1)
	{
		lcd_draw_background(RED);
		led_blink(LED_LEFT | LED_MEDIUM, 3);
		
		lcd_draw_background(GREEN);
		led_blink(LED_LEFT | LED_MEDIUM, 3);
		
		lcd_draw_background(BLUE);
		led_blink(LED_LEFT | LED_MEDIUM, 3);
		
		printf("func@bl2 main\n");
	}
	
	return 0;
}
