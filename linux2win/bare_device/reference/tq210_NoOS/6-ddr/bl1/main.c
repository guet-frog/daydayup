#include "uart.h"

int main()
{
	puts("\n##### Running in BL1 #####");
	
	/* 从SD卡的块20拷贝10个块到DDR的起始地址0x2000000地址处 */
	copy_sd(20, 10, 0x20000000);
	
	return 0;
}
