/* main.c */
#define GPC0CON		*((volatile unsigned int *)0xE0200060)
#define GPC0DAT		*((volatile unsigned int *)0xE0200064)

void delay(volatile unsigned int t)
{
	volatile unsigned int t2 = 0xFFFF;
	while (t--)
		for (; t2; t2--);
}

int main()
{
	int toggle = 0;
	GPC0CON &= ~(0xFF << 12);
	GPC0CON |= 0x11 << 12;			// 配置GPC0_3和GPC0_4为输出
	
	while (1)
	{
		GPC0DAT &= ~(0x3 << 3);		// 熄灭LED1和LED2
		
		if (toggle)
			GPC0DAT |= 1 << 3;		// 点亮LED1
		else
			GPC0DAT |= 1 << 4;		// 点亮LED2
		
		toggle = !toggle;
		delay(0x50000);
	}
	
	return 0;
}

