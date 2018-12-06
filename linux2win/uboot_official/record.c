	
	Makefile
		// 添加 CROSS_COMPILE
		// 屏蔽掉 line791
	
	start.S
		// 添加16字节占位
		// 添加led				-- 编译OK, 但是应该不在前8K, 没有正常运行
		// 调用C函数前重新设置栈 -- bl led_init后程序挂掉 -- 不是没有正确设置栈的原因, 汇编函数返回	mov pc, lr
		// clear bss修改 -- .globl
		// uart_asm_init修改 -- mov r1, #0x4f
	
	board/samsung/goni/Makefile
		// uboot_official_led.S -- 编译到libgoni.o中
	
	arch/arm/cpu/u-boot.lds
		// 修改link顺序 -- board/samsung/goni/libgoni.o link到前8K
	
	board/samsung/goni/lowlevel_init.S
		// 恢复修改
		// 整理添加system_clock_init		// 整理clock init流程, 添加pll lock check
	
		// 编译error	-- 没有及时更新s5p_goni.h文件
		// 运行error	-- 没有push {r0}
	
	board/samsung/goni/ddr_init.S
		// 添加 ddr_init.S
		// 添加 s5pc110.h 头文件 --> 添加 ddr 相关寄存器base && offset
		// 修改 s5p_goni.h --> 添加 ddr 寄存器set value
		// 修改 s5pc110.h --> 添加gpio相关(与ddr相连)
	
	arch/arm/lib/crt0.S
		// step2 entry --> 添加uart确认enter uboot step2 -- 此时已经relocate
	
	arch/arm/lib/board.c
		// 修改DRAM bank && size config
		// 添加打印 -- in board_init()
		// 屏蔽enable_cache() printf()
		
	include/configs/s5p_goni.h
		//#define CONFIG_ENV_IS_IN_ONENAND	1
		#define CONFIG_ENV_IS_IN_ONENAND  	0
		#define CONFIG_ENV_IS_IN_MMC		1

		// CONFIG_ENV_IS_IN_ONENAND=0
		// CONFIG_ENV_IS_IN_MMC=y		-- not 1
	