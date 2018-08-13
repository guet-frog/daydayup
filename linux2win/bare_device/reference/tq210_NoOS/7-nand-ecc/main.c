#include "types.h"
#include "uart.h"

void bzero(u8 *s, int size)
{
	int i = 0;
	for (; i < size; i++)
		s[i] = 0;
}

/* 测试：使用Ecc校验写入一页数据，然后将某个位反转后不使用Ecc再此写入，然后使用Ecc校验读出数据 */

void main()
{	
	int i, ret;
	u8 oob[64];
	u8 buf[2048];
	
	bzero(buf, 2048);

	/* 1.擦除第0块 */
	nand_erase(0);
	
	for (i = 0; i < 2048; i++)
		buf[i] = 0xAA;
	
	printf("begin write data..\n");
	/* 
	** 2.写入1页数据到0地址(正确数据，全部为0xAA)
	** 同时将计算出的ECC校验码保存到oob中
	*/
	nand_select_chip();
	nand_write_page_8bit(buf, 0, oob);

	/* 3.擦除第0块 */
	nand_erase(0);
	
	/*
	** 4.将第一个数据的第0位取反(0xAB)，不使用应硬件ECC，将1页数据写入第0页
	** 同时将上面记录下的ECC写入spare区
	*/
	buf[0] ^= 0x01;
	
	nand_write_page(buf, 0, oob);
	
	nand_deselect_chip();
	
	bzero(buf, 2048);

	/* 5.使用三星提供的8位硬件ECC校验拷贝函数读取一页数据 */
	//ret = NF8_ReadPage_8ECC(0, buf);
	
	ret = nand_read_page_8bit(buf, 0)
	
	printf("\nret = %d\n", ret);
	/* 打印第一个数据 */
	for (i = 0; i < 10; i++)
		printf("%X ", buf[i]);
}
