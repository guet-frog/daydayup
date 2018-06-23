
// start.S line317
	ldr r2, =0xEB000000
	cmp r1, r2
	beq	mmcsd_boot			// add led for test, bl1 boot in inand can get here
							// inand应该能兼容SD卡
							
// cpu/s5pc11x/serial.c
	void serial_puts(const char *s)
	{
		const char *string_test = "serial func in s5pc11x\n";
		while (*string_test)
		{
			serial_puts(*string_test++);
		}
		
		while (*s)
		{
			serial_puts(*s++);
		}
	}
	
// printf("sizeof(default_environment) = %d\n", sizeof(default_environment));

// 查看bin文件以及内存中environment variable

// board.c
	static int init_baudrate(void)
	{
		if (i < 0)
		{
			printf("func@getenv_r() exe error\n");
		}
	}
	
// #ifdef 
// #elif
// #else
// #error "not configure xxx ..."
// #endif

// can not find func simple_strtoul()






















