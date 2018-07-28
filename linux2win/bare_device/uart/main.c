#include "stdint.h"
#include "led.h"
#include "uart.h"

void main(void)
{
	led_init();
	
	while(1)
	{
		// uart_putc('a');
		// delay();
	}
}