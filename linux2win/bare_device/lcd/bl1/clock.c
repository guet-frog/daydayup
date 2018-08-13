#include "clock.h"

void clock_init(void)
{
	// set clock src0, change to FINPLL	-- iROM has already set clk src: clksrc0 = pll
	CLK_SRC0_REG = 0x0;
	
	// set lock time
	// 设置PLL后，时钟从Fin提升到目标频率时，需要一定的时间，即锁定时间
	APLL_LOCK_REG = 0xFFFF;		// max value
    MPLL_LOCK_REG = 0xFFFF;
	
	// set pll value
	APLL_CON0_REG = APLL_VAL;	// set APLL value && enable APLL
	MPLL_CON_REG  = MPLL_VAL;		
	
	// wait pll lock
	while (!(APLL_CON0_REG & (1<<29)));		// warning: ! or ~
	while (!(MPLL_CON_REG  & (1<<29)));
	
	// set clock src0, change to PLL	
	CLK_SRC0_REG = (1<<0) | (1<<4) | (1<<8) | (1<<12);
	
	CLK_DIV0_REG = 	(0 << 0)  |   /* APLL_RATIO = 0, freq(ARMCLK) = MOUT_MSYS / (APLL_RATIO + 1) = 1000MHz */
					(4 << 4)  |   /* A2M_RATIO = 4, freq(A2M) = SCLKAPLL / (A2M_RATIO + 1) = 200MHz */
					(4 << 8)  |   /* HCLK_MSYS_RATIO = 4, freq(HCLK_MSYS) = ARMCLK / (HCLK_MSYS_RATIO + 1) = 200MHz */
					(1 << 12) |   /* PCLK_MSYS_RATIO = 1, freq(PCLK_MSYS) = HCLK_MSYS / (PCLK_MSYS_RATIO + 1) = 100MHz */
					(3 << 16) |   /* HCLK_DSYS_RATIO = 3, freq(HCLK_DSYS) = MOUT_DSYS / (HCLK_DSYS_RATIO + 1) = 166MHz */
					(1 << 20) |   /* PCLK_DSYS_RATIO = 1, freq(PCLK_DSYS) = HCLK_DSYS / (PCLK_DSYS_RATIO + 1) = 83MHz */
					(4 << 24) |   /* HCLK_PSYS_RATIO = 4, freq(HCLK_PSYS) = MOUT_PSYS / (HCLK_PSYS_RATIO + 1) = 133MHz */
					(1 << 28);    /* PCLK_PSYS_RATIO = 1, freq(PCLK_PSYS) = HCLK_PSYS / (PCLK_PSYS_RATIO + 1) = 66MHz */
}
