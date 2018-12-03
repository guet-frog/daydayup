
#define CUR_BOOT_CHANNEL	0xD0037488
#define BOOT_CHANNEL_0		0xEB000000
#define BOOT_CHANNEL_2		0xEB200000

#define COPYFUNCADDR_SDMMC	0xD0037F98
#define START_BLK			49
#define BLK_SIZE			(2*1024*1024/512)
//#define MEM_ADDR			0x30000000
#define MEM_ADDR			0x34800000


//#define CopySDMMCtoMem(z,a,b,c,e)	(((bool(*)(int, unsigned int, unsigned short, unsigned int*, bool))(*((unsigned int *)0xD0037F98)))(z,a,b,c,e))

//typedef bool (*sdmmc2sdram_copy_fn)(int, unsigned int, unsigned short, unsigned int*, bool)
typedef int (*sdmmc2sdram_copy_fn)	\
	(int channel, unsigned int start_blk, unsigned short blk_size, unsigned int* mem_addr, int init);



void relocate_uboot2sdram(void)
{
	//unsigned int boot_channel = 0;
	
	sdmmc2sdram_copy_fn copy_func = (sdmmc2sdram_copy_fn)(*(unsigned int *)COPYFUNCADDR_SDMMC);
	
	if (BOOT_CHANNEL_2 == *((unsigned int *)CUR_BOOT_CHANNEL))
	{
		copy_func(2, START_BLK, BLK_SIZE, (unsigned int *)MEM_ADDR, 0);
	}
	else
	{
		// BOOT_CHANNEL_0 == *(unsigned int *)CUR_BOOT_CHANNEL
		while (1);
	}
}