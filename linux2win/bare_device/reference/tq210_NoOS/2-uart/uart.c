#define GPA0CON		*((volatile unsigned int *)0xE0200000)
#define ULCON0 		*((volatile unsigned int *)0xE2900000)
#define UCON0 		*((volatile unsigned int *)0xE2900004)
#define UFCON0 		*((volatile unsigned int *)0xE2900008)
#define UTRSTAT0 	*((volatile unsigned int *)0xE2900010)
#define UTXH0  		*((volatile unsigned int *)0xE2900020)
#define URXH0 		*((volatile unsigned int *)0xE2900024)
#define UBRDIV0 	*((volatile unsigned int *)0xE2900028)
#define UDIVSLOT0	*((volatile unsigned int *)0xE290002C)

/* UART0初始化 */
void uart_init()
{
	/*
	** 配置GPA0_0为UART_0_RXD
	** 配置GPA0_1为UART_0_TXD
	*/
	GPA0CON &= ~0xFF;
	GPA0CON |= 0x22;

	/* 8-bits/One stop bit/No parity/Normal mode operation */
	ULCON0 = 0x3 | (0 << 2) | (0 << 3) | (0 << 6);

	/* Interrupt request or polling mode/Normal transmit/Normal operation/PCLK/*/
	UCON0 = 1 | (1 << 2) | (0 << 10);

	/* 静止FIFO */
	UFCON0 = 0;

	/*
	** 波特率计算：115200bps
	** PCLK = 66MHz
	** DIV_VAL = (66000000/(115200 x 16))-1 = 35.8 - 1 = 34.8
	** UBRDIV0 = 34(DIV_VAL的整数部分)
	** (num of 1's in UDIVSLOTn)/16 = 0.8
	** (num of 1's in UDIVSLOTn) = 12
	** UDIVSLOT0 = 0xDDDD (查表)
	*/
	UBRDIV0 = 34;
	UDIVSLOT0 = 0xDDDD;
}

static void uart_send_byte(unsigned char byte)
{
	while (!(UTRSTAT0 & (1 << 2)));	/* 等待发送缓冲区为空 */
	UTXH0 = byte;					/* 发送一字节数据 */		
}

static unsigned char uart_recv_byte()
{
	while (!(UTRSTAT0 & 1));		/* 等待接收缓冲区有数据可读 */
	return URXH0;					/* 接收一字节数据 */		
}

void putchar(int c)
{
	uart_send_byte(c);
	/* 如果只写'\n'，只是换行，而不会跳到下一行开头 */
	if (c == '\n')
		uart_send_byte('\r');
}

int getchar()
{
	int c;
	c = uart_recv_byte();
	return c;
}

void puts(char *str)
{
	char *p = str;
	while (*p)
		putchar(*p++);
	putchar('\n');
}
