
2.13.1.2、背光电路分析
	x210的LCD模组上使用MP3202芯片来做调光, PWMTOUT0(GPD0_0)接在了IC的FB引脚上			// 低电平直接点亮
	SoC应该通过该引脚输出一定频率给调光IC, 控制MP3202输出的电流大小, 从而控制屏幕亮度
	L_DISP(DISP、SYS_OE、GPF3_5)接在了MP3202的EN引脚, 高电平enable

2.13.1.3、X210的uboot中LCD代码分析		// x210_preboot_init()

2.13.2.1、LCD驱动mpadfb.c分析
	step1: lcd init	// lcd init执行完毕, 写lcd framebuff则可以显示
		(1)fb_init()		// 填充info
		(2)lcd_port_init()	// port -> gpio	
		(3)lcd_reg_init()	// lcd controller reg init

	step2: write framebuff && open backlight
		(1)display_logo()
		(2)backlight_init()

	// important struct && global variable(init)
	static struct fb_info info = {
		.name		= "fb",
	}
	// fb_info.bitmap.xxx, info全局变量的其他成员变量在fb_init()中赋值

	static struct fb s5pv210_fb = {
		.info		= &info,
		.map_color	= fb_ft_map_color,
	}

2.13.2.2、解决新版开发板X210BV3S的logo显示问题		// lcd参数, 分辨率修改1024*600

2.13.3.1、什么是fastboot		// Android刷机: fastboot, recovery

2.13.4.2、关键点：rx_handler
do_fastboot
	fastboot_poll
		fboot_usb_int_hndlr
			fboot_usb_pkt_receive
				fboot_usb_int_bulkout
					fastboot_interface->rx_handler（函数指针）
						指向cmd_fastboot.c/rx_handler

2.13.4.3、代码分析
(1)大文件download机制	// usb传输单次传输最大只能有限个字节(64、256)

(2)down后的响应机制, 开发板端通过fastboot_tx_status函数向主机发送响应, 主机显示这个响应

(3)fastboot有关宏定义
	CFG_FASTBOOT_TRANSFER_BUFFER
	CFG_FASTBOOT_TRANSFER_BUFFER_SIZE		// download max size

