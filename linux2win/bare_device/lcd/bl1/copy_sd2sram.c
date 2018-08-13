#include "copy_sd2sram.h"

uint8_t copy_sd2sram(uint32_t sd_start_block, uint16_t sd_block_cnt, uint32_t *mem_ptr)
{
	uint8_t ret = 0;
	ret = ((sd2sram_t)(*(uint32_t *)COPY_SDMMC2MEM))(2, sd_start_block, sd_block_cnt, mem_ptr, 0);
	
	return ret;
}

void jump_2_bl2(void)
{
	pFunc_t jump2bl2 = (pFunc_t)SRAM_BL2_BASE;
	jump2bl2();
}
