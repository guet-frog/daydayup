#include "uart.h"
#include "types.h"

void delay(u32 t)
{
	u32 t2 = 0xFFFF;
	while (t--)
		for (; t2; t2--);
}

int main()
{
	u32 t = 10000;
	
	puts("\n\nUart Test for printf");
	while (1)
	{
		printf("t = %d (0x%X)\n", t, t);
		t++;
		delay(500000);
	}
	return 0;
}