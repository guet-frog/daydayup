#include "types.h"

#define	NFCONF  (*(volatile u32 *)0xB0E00000) 
#define	NFCONT  (*(volatile u32 *)0xB0E00004) 	
#define	NFCMMD  (*(volatile u32 *)0xB0E00008) 
#define	NFADDR  (*(volatile u32 *)0xB0E0000C)
#define	NFDATA  (*(volatile u8 *)0xB0E00010)
#define	NFSTAT  (*(volatile u32 *)0xB0E00028)

#define MP0_1CON  (*(volatile u32 *)0xE02002E0)
#define	MP0_3CON  (*(volatile u32 *)0xE0200320)
#define	MP0_6CON  (*(volatile u32 *)0xE0200380)

#define PAGE_SIZE	2048
#define BLOCK_SIZE	(PAGE_SIZE * 64)

/* 等待NAND准备好 */
static void inline nand_wait_ready()
{
	while(!(NFSTAT & (1 << 0)));
}

/* 片选 */
static void inline nand_select_chip()
{
	NFCONT &= ~(1 << 1);
}

/* 取消片选 */
static void inline nand_deselect_chip()
{
	NFCONT |= (1 << 1);
}

/* 发命令 */
static void inline nand_cmd(u32 cmd)
{
	NFCMMD = cmd;
}

/* 发地址（5个周期） */
static void nand_addr(u32 addr)
{
	u32 col = addr % PAGE_SIZE;	/* 页内偏移 */
	u32 row = addr / PAGE_SIZE;	/* 页地址 */
	
	NFADDR = col & 0xFF;
	NFADDR = (col >> 8) & 0xF;
	NFADDR = row & 0xFF;
	NFADDR = (row >> 8) & 0xFF;
	NFADDR = (row >> 16) & 0x07;
}

/* 读1byte数据 */
static u8 inline nand_read()
{
	return NFDATA;
}

/* 写1byte数据 */
static void inline nand_write(u8 data)
{
	NFDATA = data;
}

/* 读size byte数据 */
static void nand_read_buf(u8 *buf, int size)
{
	int i = 0;
	for (; i < size; i++)
		buf[i] =  NFDATA;
}

/* 写size byte数据 */
static void nand_write_buf(u8 *buf, int size)
{
	int i = 0;
	for (; i < size; i++)
		NFDATA = buf[i];
}

/* 复位NAND */
static void nand_reset()
{
	nand_select_chip();
	nand_cmd(0xFF);
	nand_wait_ready();
	nand_deselect_chip();
}

/* NAND初始化 */
void nand_init()
{
	/* HCLK_PSYS=133MHz(7.5ns) */
	NFCONF =	(0x1 << 23) |	/* Disable 1-bit and 4-bit ECC */
				/* 下面3个时间参数稍微比计算出的值大些（我这里依次加1），否则读写不稳定 */
				(0x3 << 12) |	/* 7.5ns * 2 > 12ns tALS tCLS */
				(0x2 << 8) | 	/* (1+1) * 7.5ns > 12ns (tWP) */
				(0x1 << 4) | 	/* (0+1) * 7.5 > 5ns (tCLH/tALH) */
				(0x0 << 3) | 	/* SLC NAND Flash */
				(0x0 << 2) |	/* 2KBytes/Page */
				(0x1 << 1);		/* 5 address cycle */
				
	/* 
	** The setting all nCE[3:0] zero can not be allowed. Only 
	** one nCE can be asserted to enable external NAND flash 
	** memory. The lower bit has more priority when user set all 
	** nCE[3:0] zeros. 
	*/
	NFCONT =	(0x1 << 1) |	/* Disable chip select */
				(0x1 << 0);		/* Enable NAND Flash Controller */
					
	/*
	** Port Map
	** CE1->Xm0CSn2-> MP01_2
	** CE2->Xm0CSn3-> MP01_3
	** CE3->Xm0CSn4-> MP01_4
	** CE4->Xm0CSn5-> MP01_5
	** CLE->Xm0FCLE-> MP03_0
	** ALE->Xm0FALE-> MP03_1
	** WE->Xm0FWEn->  MP03_2
	** RE->Xm0FREn->  MP03_3
	** RB1->Xm0FRnB0->MP03_4
	** RB2->Xm0FRnB1->MP03_5
	** RB3->Xm0FRnB2->MP03_6
	** RB4->Xm0FRnB3->MP03_7
	** IO[7:0]->Xm0DATA[7:0]->MP0_6[7:0]
	*/
	MP0_1CON &= ~(0xFFFF << 8);
	MP0_1CON |= (0x3333 << 8);
	MP0_3CON = 0x22222222;
	MP0_6CON = 0x22222222;
	
	nand_reset();
}

/* 读NAND ID */
void nand_read_id(u8 id[])
{
	int i;
	nand_select_chip();
	nand_cmd(0x90);
	NFADDR = 0x00;
	for (i = 0; i < 5; i++)
		id[i] = nand_read();

	nand_deselect_chip();
}

/* 擦除一个块 */
void nand_erase(u32 addr)
{	
	if (addr & (BLOCK_SIZE - 1))
	{
		printf("not block align\n");
		return;
	}
	u32 row = addr / PAGE_SIZE;
	
	nand_select_chip();
	nand_cmd(0x60);
	
	NFADDR = row & 0xff;			
	NFADDR = (row >> 8) & 0xff;
	NFADDR = (row >> 16) & 0x07;
	
	nand_cmd(0xD0);
	nand_wait_ready();
	nand_deselect_chip();
}

/* 读一页数据 */
void nand_read_page(u8 *buf, u32 addr)
{
	if (addr & (PAGE_SIZE - 1))
	{
		printf("not page align\n");
		return;
	}
	
	int i;
	nand_select_chip();
	nand_cmd(0);
	nand_addr(addr);
	nand_cmd(0x30);
	nand_wait_ready();
	
	nand_read_buf(buf, PAGE_SIZE);
	
	nand_deselect_chip();
}

/* 随机读:从任意地址读任意字节的数据 */
void nand_read_random(u8 *buf, u32 addr, u32 size)
{	
	nand_select_chip();
	nand_cmd(0);
	nand_addr(addr);
	nand_cmd(0x30);
	nand_wait_ready();
	
	int i;
	u32 col = addr % PAGE_SIZE;	/* 页内偏移 */

	for(i = col; i < size + col; i++)
	{
		nand_cmd(0x05);
		NFADDR = i & 0xFF;
		NFADDR = (i >> 8) & 0xF;
		nand_cmd(0xE0);
		*buf++ = nand_read();
	}
	
	nand_deselect_chip();
}

/* 写一页数据 */
void nand_write_page(u8 *buf, u32 addr)
{
	if (addr & (PAGE_SIZE - 1))
	{
		printf("not page align\n");
		return;
	}
	
	int i;
	nand_select_chip();
	nand_cmd(0x80);
	nand_addr(addr);
	nand_wait_ready();
	
	nand_write_buf(buf, PAGE_SIZE);
	
	nand_cmd(0x10);
	nand_wait_ready();
	nand_deselect_chip();
}
