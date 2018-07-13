/*
 * 文件名：	led.s	
 * 作者：	朱老师
 * 描述：	演示重定位（在SRAM内部重定位）
 */

#define WTCON		0xE2700000

#define SVC_STACK	0xd0037d80

.global _start					// 把_start链接属性改为外部，这样其他文件就可以看见_start了
_start:
	// 第1步：关看门狗（向WTCON的bit5写入0即可）
	ldr r0, =WTCON
	ldr r1, =0x0
	str r1, [r0]
	
	// 第2步：设置SVC栈
	ldr sp, =SVC_STACK
	
	// 第3步：开/关icache
	mrc p15,0,r0,c1,c0,0;			// 读出cp15的c1到r0中
	//bic r0, r0, #(1<<12)			// bit12 置0  关icache
	orr r0, r0, #(1<<12)			// bit12 置1  开icache
	mcr p15,0,r0,c1,c0,0;

	// 第4步：初始化ddr
	bl sdram_asm_init
	
	// 第5步：重定位
	// adr指令用于加载_start当前运行地址
	adr r0, _start  		// adr加载时就叫短加载		
	// ldr指令用于加载_start的链接地址:0xd0024000
	ldr r1, =_start // ldr加载时如果目标寄存器是pc就叫长跳转，如果目标寄存器是r1等就叫长加载	
	// bss段的起始地址
	ldr r2, =bss_start	// 就是我们重定位代码的结束地址，重定位只需重定位代码段和数据段即可
	cmp r0, r1			// 比较_start的运行时地址和链接地址是否相等
	beq clean_bss		// 如果相等说明不需要重定位，所以跳过copy_loop，直接到clean_bss
						// 如果不相等说明需要重定位，那么直接执行下面的copy_loop进行重定位
						// 重定位完成后继续执行clean_bss。

// 用汇编来实现的一个while循环
copy_loop:
	ldr r3, [r0], #4    // 源
	str r3, [r1], #4	// 目的   这两句代码就完成了4个字节内容的拷贝
	cmp r1, r2			// r1和r2都是用ldr加载的，都是链接地址，所以r1不断+4总能等于r2
	bne copy_loop

	// 清bss段，其实就是在链接地址处把bss段全部清零
clean_bss:
	ldr r0, =bss_start					
	ldr r1, =bss_end
	cmp r0, r1				// 如果r0等于r1，说明bss段为空，直接下去
	beq run_on_dram			// 清除bss完之后的地址
	mov r2, #0
clear_loop:
	str r2, [r0], #4		// 先将r2中的值放入r0所指向的内存地址（r0中的值作为内存地址），
	cmp r0, r1				// 然后r0 = r0 + 4
	bne clear_loop

run_on_dram:	
	// 长跳转到led_blink开始第二阶段
	ldr pc, =led_blink				// ldr指令实现长跳转
	
	
	// 从这里之后就可以开始调用C程序了
	//bl led_blink					// bl指令实现短跳转
	
// 汇编最后的这个死循环不能丢
	b .
	

	
	
	

	
	
	
	
	
	
	
	
	
