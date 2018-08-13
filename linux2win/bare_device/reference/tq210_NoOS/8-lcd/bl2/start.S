.global _start				/* 声明一个全局的标号 */
_start:	
		bl main				/* 跳转到C函数去执行 */
halt:
		b halt
