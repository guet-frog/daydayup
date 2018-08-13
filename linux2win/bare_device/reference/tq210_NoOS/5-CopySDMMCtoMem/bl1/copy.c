#include "types.h"

typedef u32(*copy_sd_mmc_to_mem)(u32 channel, u32 start_block, u16 block_size, u32 *trg, u32 init);

/*
** start_block:从哪个块开始拷贝
** block_size：拷贝多少块
** addr：拷贝到哪里
*/
void copy_sd(u32 start_block, u16 block_size, u32 addr)
{
	u32 V210_SDMMC_BASE = *(volatile u32 *)(0xD0037488);	// V210_SDMMC_BASE
	u8 ch;
	copy_sd_mmc_to_mem copy_bl2 = (copy_sd_mmc_to_mem)(*(u32 *)(0xD0037F98));
	
	/* 参考S5PV210手册7.9.1 SD/MMC REGISTER MAP */
	if (V210_SDMMC_BASE == 0xEB000000)		// 通道0
		ch = 0;
	else if (V210_SDMMC_BASE == 0xEB200000)	// 通道2	
		ch = 2;
	
	copy_bl2(ch, start_block, block_size, (u32 *)addr, 0);
}
