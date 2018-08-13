#ifndef COPY_SD2SRAM_H
#define COPY_SD2SRAM_H

#include "local_typedef.h"

#define GLOBAL_BLOCK_SIZE		0xD0037480
#define GLOBAL_SDHC_INFO_BIT	0xD0037484
#define GLOBAL_BOOT_CHANNEL		0xD0037488

#define COPY_SDMMC2MEM			0xD0037F98	/* func pointer's location */

#define SD_BL2_POS		50
#define SD_BL2_CNT		32
#define SRAM_BL2_BASE	0xD0026000

/**
* This Function copy MMC(MoviNAND/iNand) Card Data to memory.
* Always use EPLL source clock.
* This function works at 20Mhz.
* @param u32 StartBlkAddress : Source card(MoviNAND/iNand MMC)) Address.(It must block address.)
* @param u16 blockSize : Number of blocks to copy.
* @param u32* memoryPtr : Buffer to copy from.
* @param bool with_init : determined card initialization.
* @return bool(u8) - Success or failure.
*/
//#define CopySDMMCtoMem(z,a,b,c,e)(((bool(*)(int, unsigned int, unsigned short, unsigned int*, bool))(*((unsigned int *)0xD0037F98)))(z,a,b,c,e))

typedef bool (*sd2sram_t)(int, unsigned int, unsigned short, unsigned int*, bool);		// warning
//typedef (bool (*sd2sram_t)(int, unsigned int, unsigned short, unsigned int*, bool));

typedef void (*pFunc_t)(void);

uint8_t copy_sd2sram(uint32_t sd_start_block, uint16_t sd_block_cnt, uint32_t *mem_ptr);

void jump_2_bl2(void);

#endif