#include "types.h"
#include "uart.h"

void bzero(u8 *s, int size)
{
	int i = 0;
	for (; i < size; i++)
		s[i] = 0;
}

void main()
{	
	u8 buf[2048];
	int i;
	
	bzero(buf, 2048);

	nand_read_id(buf);
	printf("\nID:");
	for (i = 0; i < 5; i++)
	{
		printf("%X ", buf[i]);
	}
	putchar('\n');
	
	nand_erase(0x80000);			/* 擦除以0x80000地址开始的一个块 */
	
	/*
	** 要写入的数据格式：
	** 0x00 0x01 0x02 .... 0x0F
	** 0x10 0x11 0x12 .... 0x1F
	** ...
	*/
	for (i = 0; i < 2048; i++)
		buf[i] = i % 255;
	
	nand_write_page(buf, 0x80000);	/* 写入1页数据到0x80000地址 */
	
	bzero(buf, 2048);
	
	//nand_read_page(buf, 0x80000);			/* 从0x80000地址读取一页数据（顺序读） */
	nand_read_random(buf, 0x80000, 2048);	/* 从0x80000地址读取一页数据（随机读） */
	
	
	/* 打印读取到的数据，与写入的数据一致 */
	for (i = 0; i < 2048; i++)
	{
		if (i % 16 == 0)
			putchar('\n');
		printf("%X ", buf[i]);
	}
}
