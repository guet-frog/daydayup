#include "led.h"
#include "uart.h"
#include "clock.h"
#include "copy_sd2sram.h"
#include "./include/stdio.h"

void bsp_init(void)
{
	led_init();
	
	uart_init(UART2_115200);
	
	clock_init();
	
	print_cpuinfo();
}

int main(void)
{
	uint8_t ret = 0;
	
	bsp_init();
	
	while(1)
	{
		led_blink(LED_RIGHT, 3);
		
		break;
	}
	
	printf("copying sd to ddr ...\n");
	
	ret = copy_sd2sram((uint32_t)SD_BL2_POS, (uint16_t)SD_BL2_CNT, (uint32_t *)SRAM_BL2_BASE);
	
	if (ERROR == ret)
	{
		printf("func@main: copy_sd2ddr() exe error\n");
	}
	
	printf("sd copy to ddr exe success && ready jump to bl2\n");
	led_blink(LED_RIGHT, 3);
	
	jump_2_bl2();
	
	printf("never get here\n");
	
	while (1);
	
	return 0;
}
