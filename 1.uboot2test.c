
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

// can not find func simple_strtoul()

// uboot在复位后首先在sram中运行bl1, 然后跳转到ddr中从bl2开始运行

// uboot源码在编译 除法 没有报错

// can not find func env_init()

// init_baudrate() --> printf("func@init_baudrate() i = %d\n", i);

// #define PHYS_SDRAM_2 MEMORY_BASE_ADDRESS2
#if defined(PHYS_SDRAM_2)	// -- warning: #if defined
	gd->bd->bi_dram[1].start = PHYS_SDRAM_2;        // 0x4000_0000
	gd->bd->bi_dram[1].size = PHYS_SDRAM_2_SIZE;
#endif

 // line 493
#else
	printf("not defined CFG_NO_FLASH");
	
	typedef	struct	global_data
	{
		bd_t		    *bd;
		unsigned long	flags;
		unsigned long	baudrate;
		unsigned long	have_console;	/* serial_init() was called */
		unsigned long	reloc_off;	    /* Relocation Offset */
		
		unsigned long	env_addr;	    /* Address  of Environment struct */
		unsigned long	env_valid;	    /* Checksum of Environment valid? */
		
		unsigned long	fb_base;	    /* base address of frame buffer */
		
		void		**jt;		/* jump table */
	} gd_t;
	
	typedef struct bd_info
	{
		int			            bi_baudrate;	/* serial console baudrate */
		unsigned long	        bi_ip_addr;	    /* IP Address */
		unsigned char	        bi_enetaddr[6]; /* Ethernet adress */
		struct environment_s    *bi_env;
		ulong	                bi_arch_number;	/* unique id for this board */
		ulong	                bi_boot_params;	/* where this board expects params */
		
		struct
		{
			ulong start;
			ulong size;
		} bi_dram[CONFIG_NR_DRAM_BANKS]; /* RAM configuration */
	} bd_t;
	
	// movi.c movi_bl2_copy() puts("other SDHC info");

	// eat leading white space
	// for (p = commandline; *p == ' '; p++);
	
	// set uboot env && save in inand, then download uboot && exe print command
	
	// setenv(stdio_names[i], stdio_devices[i]->name)


















