#include "led.h"
#include "uart.h"
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
	led_init();
	
	while(1)
	{
		led_blink(LED_LEFT | LED_MEDIUM, 1);
		
		printf("func@bl2 main\n");
	}
	
	return 0;
}
