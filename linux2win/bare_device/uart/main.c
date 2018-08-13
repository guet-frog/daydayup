#include "led.h"
#include "uart.h"
#include "clock.h"
#include "./include/stdio.h"

int main(void)
{
	char ch = '\0';
	
	led_init();
	uart_init(UART2_115200);
	clock_init();
	
	print_cpuinfo();
	
	while(1)
	{
		led_blink(LED_RIGHT, 1);
		
		printf("ch = %c\n", ++ch);
		
		ch = getc();
		printf("ch = %c\n", ch);
	}
	
	return 0;
}
