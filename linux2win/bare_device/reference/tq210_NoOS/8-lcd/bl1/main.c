#include "uart.h"

int main()
{
	/* 从SD卡的块20拷贝80个块到DDR的起始地址0x2000000地址处 */
	copy_sd(20, 80, 0x20000000);
	
	return 0;
}
