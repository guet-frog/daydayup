
-- modify -> git add -> git status:
# on brach master
# changes to be committed:
# (use "git reset HEAD <file> ..." to unstage)  // where is head ? (working directory)(stage)(repository)
#
#   modified: readme.txt
#

git diff HEAD -- readme.txt命令可以查看工作区和版本库里面最新版本的区别

公钥用来加密信息，私钥用来数字签名

数字签名(digital signature) 数字证书(digital certificate)

默认中断优先级

find -- 知道某个文件/文件夹名称, 却不知道路径
grep -- 知道某个"xxx", 看在哪个文件中出现

du -sh : 查看当前目录总共占的容量。而不单独列出各子项占用的容量 
du -lh --max-depth=1 : 查看当前目录下一级子文件和子目录占用的磁盘容量




// -----------------------------------------------
	设置查找高亮
	忽略大小写
	vi查找 (全词匹配)
// -----------------------------------------------
	ctrl + shift + o/e
	ctrl + shift + n/p  //terminator 串口切换


// 开发中不适用标准的C库

// file u-boot
// 移植代码一般不去修改前面写的, 尽量打补丁后面给覆盖上

	makefile中第一个目标为默认目标
	
	shell exit 1 && exit 0
    
    linux设备驱动不正常可能无法开机(如:显卡拔掉)
    
    append: 附加, 添加; 贴上; 盖章
	
// -- 芯片在上电和休眠唤醒都需要对cache进行操作 v7_flush_dcache_all

 -- static inline S5PC11X_TIMES	*S5PC11X_GetBase_TIMES(void) 	// inline
    {
		return (S5PC11X_TIMES *)ELFIN_TIMER_BASE;
    }

// -- 磁盘的读写基本单位, 操作系统读写磁盘的基本单位
// -- 文件系统是操作系统的一部分, 所以文件系统操作文件的最小单位是块

 -- printf data段, bss段起始地址
 -- gcc编译不使用标准库


 -- for test
// 烧录后sd中bin文件, 其中应该有两份重复的BL1
// movi.c 	line51:	 printf("MOVI_BL2_POS = %d\r\n", MOVI_BL2_POS); && ./sd_fusing/sd_fusing.sh BL2设置
// start.S	line411: printf("_start_armboot = %d\r\n", _start_armboot);
// start.S 	line57:  printf("reset = %p\r\n", reset);

// arm程序中两种方法实现程序流程的跳转
// (1) 专用的跳转指令(b, bl)
// (2) 直接向程序计数器PC写入跳转地址值(mov lr, pc; mov pc, xxx)

============================================================================2018-7-31 21:38:24
// printf sprintf vsprintf vprintf

 -- int printf(const char *format, ...);
 -- int sprintf(char *s, const char *format, ...);
 -- int vsprintf(char *s, const char *format, va_list arg);
 -- int vprintf(const char *format, va_list arg);

int sprintf (char *s, const char *format, ...)
{
	int     iReturn ;
	va_list pArgs ;
	va_start (pArgs, szFormat) ;
	iReturn = vsprintf (szBuffer, szFormat, pArgs) ;
	va_end (pArgs) ;
	return iReturn ;
}

int printf(const char *fmt, ...)
{
	int i;
	int len;
	
	va_list args;

	va_start(args, fmt);
	len = vsprintf(g_pcOutBuf,fmt,args);	// 格式化输出到buf
	va_end(args);
	
	for (i = 0; i < strlen(g_pcOutBuf); i++)
	{
		putc(g_pcOutBuf[i]);
	}
	return len;
}

int printf(const char *fmt, ...)
{
	va_list args;
	
	va_start(args, fmt);
	vprintf(fmt, args);		// 格式化输出到stdout
	va_end(args);
}
============================================================================2018-7-31 21:38:33
// makefile
	GNU链接器几个开关项的解释：-lm -lc -lgcc  
	-lm 代表链接器将连接GCC的数学库libm.a  
	-lc 代表链接器将连接GCC的标准C库libc.a  
	-lgcc 代表链接器将连接GCC的支持库libgcc.a 
	
	-g 可执行程序包含调试信息
	-o 指定输出文件名
	-c 只编译不链接
	
#LIBPATH = -lgcc -L/usr/local/arm/arm-2009q3/lib/gcc/arm-none-linux-gnueabi/4.4.1
LIBPATH	= /usr/local/arm/arm-2009q3/lib/gcc/arm-none-linux-gnueabi/4.4.1

uart.bin: start.o led.o uart.o speed.o main.o $(LIBPATH)/libgcc.a	// 链接静态库(1)
	#arm-linux-ld -Ttext 0x0 -o uart.elf $^ $(LIBPATH)	// 链接静态库(2)
============================================================================2018-8-3 22:23:16
// 编译bootloader、kernel、裸机  -nostdlib	// 此类程序不需要 启动文件、标准库文件
============================================================================2018-8-8 23:44:17
// #define DMC0_MEMCONFIG_0	0x20F01323s 	// 0x38000000内存地址可以被访问
============================================================================
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	