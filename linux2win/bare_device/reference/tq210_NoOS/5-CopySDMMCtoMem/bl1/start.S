.global _start				/* 声明一个全局的标号 */
_start:
		bl clock_init		/* 时钟初始化 */
		bl uart_init		/* 串口初始化 */
		bl main				/* 跳转到C函数去执行 */
		ldr pc, =0xD0022800 /* 跳转到bl2的起始地址执行 */
halt:
		b halt
