#define GPC0CON		*((volatile unsigned int *)0xE0200060)
#define GPC0DAT		*((volatile unsigned int *)0xE0200064)

int main()
{
	int c;

	GPC0CON &= ~(0xFF << 12);
	GPC0CON |= 0x11 << 12;		// 配置GPC0_3和GPC0_4为输出
	GPC0DAT &= ~(0x3 << 3);		// 熄灭LED1和LED2

	puts("UART Test in S5PV210");
	puts("1.LED1 Toggle");
	puts("2.LED2 Toggle");
	puts("Please select 1 or 2 to Toggle the LED");
	
	while (1)
	{
		c = getchar();			// 从串口终端获取一个字符
		putchar(c);				// 回显
		putchar('\r');

		if (c == '1')
			GPC0DAT ^= 1 << 3;	// 改变LED1的状态
		else if (c == '2')
			GPC0DAT ^= 1 << 4;	// 改变LED2的状态
	}
	return 0;
}