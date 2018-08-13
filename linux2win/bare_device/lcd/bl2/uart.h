#ifndef _UART_H
#define _UART_H
#include "local_typedef.h"

#define UART2_115200	0
#define UART2_9600		1

#define UART2_BASE			0xE2900800
#define ULCON_OFFSET		0x00
#define	UCON_OFFSET			0x04
#define USTAT_OFFSET		0x10
#define UTX_OFFSET			0x20
#define URX_OFFSET			0x24
#define UBRDIV2_OFFSET		0x28
#define UDIVSLOT2_0FFSET	0x2C

#define GPA1CON				0xE0200020


#define rGPA1CON	(*((volatile uint32_t *)GPA1CON))

#define rULCON2		(*(volatile uint32_t *)(UART2_BASE + ULCON_OFFSET))		// warning
#define rUCON2		(*(volatile uint32_t *)(UART2_BASE + UCON_OFFSET))

#define rUTRSTAT2	(*(volatile uint32_t *)(UART2_BASE + USTAT_OFFSET))
#define rUTXH2		(*(volatile uint32_t *)(UART2_BASE + UTX_OFFSET))
#define rURXH2		(*(volatile uint32_t *)(UART2_BASE + URX_OFFSET))

#define rUBRDIV2	(*(volatile uint32_t *)(UART2_BASE + UBRDIV2_OFFSET))
#define rUDIVSLOT2	(*(volatile uint32_t *)(UART2_BASE + UDIVSLOT2_0FFSET))


void uart_init(uint8_t baud_rate);		// uart2 init

void putc(unsigned char c);

unsigned char getc(void);

#endif