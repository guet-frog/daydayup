#include "uart.h"
#include "led.h"


void uart_init(uint8_t baud_rate)
{
	rGPA1CON &= ~(0xff<<0);			// warning: 0x00001111
	rGPA1CON |= (0x2<<4)|(0x2<<0);
	
	rULCON2 &= ~(0x3<<0);
	rULCON2 |= (0x3<<0);
	
	rUCON2 &= ~((0x1<<2)|(0x1<<0));
	rUCON2 |= ((0x1<<2)|(0x1<<0));
	
	if (UART2_115200 == baud_rate)
	{
		rUBRDIV2 = 35;
		rUDIVSLOT2 = 0x0888;
	}
	else if (UART2_9600 == baud_rate)
	{
		rUBRDIV2 = 429;
		rUDIVSLOT2 = 0xD5D5;
	}
	else
	{
		
	}
}

//void uart_putc(char c)
void putc(unsigned char c)
{	
	// 串口发送一个字符，其实就是把一个字节丢到发送缓冲区中去
	// 因为串口控制器发送1个字节的速度远远低于CPU的速度，所以CPU发送1个字节前必须
	// 确认串口控制器当前缓冲区是空的(串口已经发完了上一个字节)
	while (!(rUTRSTAT2 & (1<<1)));
	
	rUTXH2 = c;
}

// 串口接收程序，轮询方式，接收一个字节
//char uart_getc(void)
unsigned char getc(void)
{
	while (!(rUTRSTAT2 & (1<<0)));
	
	return (rURXH2 & 0xFF);
}
















