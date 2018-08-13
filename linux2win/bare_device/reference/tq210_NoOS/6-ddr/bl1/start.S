.global _start				/* 声明一个全局的标号 */
_start:
		bl clock_init		/* 时钟初始化 */
		bl uart_init		/* 串口初始化 */
		bl ddr_init			/* DDR初始化 */
		bl main				/* 跳转到C函数去执行 */
		ldr sp, =0x60000000	/* 重新设在栈为DDR内存的最高地址 */
		ldr pc, =0x20000000	/* 跳转到DDR的起始地址执行 */
halt:
		b halt
