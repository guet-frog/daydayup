#include "uart.h"

int main()
{
	puts("\n##### Running in BL1 #####");
	
	/* 从SD卡的块20拷贝10个块到SRAM的0xD0022800地址处 */
	copy_sd(20, 10, 0xD0022800);
	
	return 0;
}