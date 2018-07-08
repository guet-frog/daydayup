
1.6.1.1、什么是时钟？SoC为什么需要时钟？
	(1)时钟是同步工作系统的同步节拍

1.6.1.2、SoC时钟一般如何获得
	(1)外部直接输入时钟信号, SoC有个引脚用来输入外部时钟信号	// 方便同步
	(2)外部晶振+内部时钟发生器产生时钟		// 低频单片机
	(3)外部晶振+内部时钟发生器+内部PLL产生高频时钟+内部分频器分频得到各种频率的时钟	// s5pv210

1.6.1.5、时钟和功耗控制的关系
	// 关闭外设时钟, 关闭外设从而降低功耗
	// 外设工作时钟一般默认是关闭的, 使用外设之前注意使能时钟

1.6.2.3、PLL：APLL、MPLL、EPLL、VPLL
	APLL: 		Cortex-A8内核 MSYS域
	MPLL&EPLL: 	DSYS PSYS
	VPLL: 		Video视频相关模块

1.6.2.1、时钟域：MSYS、DSYS、PSYS	//s5pv210内部外设模块间工作时钟差异大, 因此把整个内部的时钟划分3个域
	(1) MSYS域:
		ARMCLK:		cpu内核(cortex-a8 core)工作时钟(主频)
		HCLK_MSYS:	MSYS域的高频时钟, 给DMC0和DMC1使用

		PCLK_MSYS:	MSYS域的低频时钟
		HCLK_IMEM:	给iROM和iRAM（合称iMEM）使用

	(2) DSYS域:	// 视频显示, 编解码模块等
		HCLK_DSYS: DSYS域的高频时钟
		PCLK_DSYS: DSYS域的低频时钟

	(3) PSYS域:	// 内部的各种外设(串口、SD接口、I2C、AC97、USB等)
		HCLK_PSYS: PSYS域的高频时钟
		PCLK_PSYS: PSYS域的低频时钟
		SCLK_ONENAND:

总结：210内部的各个外设都是接在（内部AMBA总线）总线上面的，AMBA总线有1条高频分支叫AHB，有一条低频分支叫APB。
	上面的各个域都有各自对应的HCLK_XXX和PCLK_XXX，
	其中HCLK_XXX就是XXX这个域中AHB总线的工作频率；PCLK_XXX就是XXX这个域中APB总线的工作频率。
	SoC内部的各个外设其实是挂在总线上工作的，也就是说这个外设的时钟来自于他挂在的总线，
	譬如串口UART挂在PSYS域下的APB总线上，因此串口的时钟来源是PCLK_PSYS。

1.6.3.4、各时钟典型值（默认值，iROM中设置的值）
(1)当210刚上电时，默认是外部晶振+内部时钟发生器产生的24MHz频率的时钟直接给ARMCLK的，这时系统的主频就是24MHz，运行非常慢。
(2)iROM代码执行时第6步中初始化了时钟系统，这时给了系统一个默认推荐运行频率。这个时钟频率是三星推荐的210工作性能和稳定性最佳的频率。
(3)各时钟的典型值：
	? freq(ARMCLK) 			= 1000 MHz
	? freq(HCLK_MSYS) 		= 200 MHz
	? freq(HCLK_IMEM) 		= 100 MHz
	? freq(PCLK_MSYS) 		= 100 MHz
	? freq(HCLK_DSYS) 		= 166 MHz
	? freq(PCLK_DSYS) 		= 83 MHz
	? freq(HCLK_PSYS) 		= 133 MHz
	? freq(PCLK_PSYS) 		= 66 MHz
	? freq(SCLK_ONENAND) 	= 133 MHz, 166 MHz

1.6.4.S5PV210时钟体系框图详解
(1)时钟体系框图的位置：数据手册P361&P362，Figure3-3
(2)两张图之间是渐进的关系。第一张图从左到右依次完成了原始时钟生成->PLL倍频得到高频时钟->初次分频得到各总线时钟；
	第二张图是从各中间时钟（第一张图中某个步骤生成的时钟）到各外设自己使用的时钟（实际就是个别外设自己再额外分频的设置）。
	可见，第一张图是理解整个时钟体系的关键，第二种图是进一步分析各外设时钟来源的关键。
(3)要看懂时钟体系框图，2个符号很重要：一个是MUX开关，另一个是DIV分频器。
(3.1)MUX开关就是个或门，实际对应某个寄存器的某几个bit位的设置，设置值决定了哪条通道通的，分析这个可以知道右边的时钟是从左边哪条路过来的，从而知道右边时钟是多少。
(3.2)DIV分频器，是一个硬件设备，可以对左边的频率进行n分频，分频后的低频时钟输出到右边。分频器在编程时实际对应某个寄存器中的某几个bit位，我们可以通过设置这个寄存器的这些对应bit位来设置分频器的分频系数（譬如左边进来的时钟是80MHz，分频系统设置为8，则分频器右边输出的时钟频率为10MHz）。
(3.3)寄存器中的clock source x就是在设置MUX开关；clock divider control寄存器就是在设置分频器分频系数。

1.6.5.时钟设置的关键性寄存器
1.6.5.1、xPLL_LOCK
	xPLL_LOCK寄存器主要控制PLL锁定周期的。
1.6.5.2、xPLL_CON/xPLL_CON0/xPLL_CON1
	PLL_CON寄存器主要用来打开/关闭PLL电路，设置PLL的倍频参数，查看PLL锁定状态等
1.6.5.3、CLK_SRCn(n：0～6)
	CLK_SRC寄存器是用来设置时钟来源的，对应时钟框图中的MUX开关。
1.6.5.4、CLK_SRC_MASKn
	CLK_SRC_MASK决定MUX开关n选1后是否能继续通过。默认的时钟都是打开的，好处是不会因为某个模块的时钟关闭而导致莫名其妙的问题，坏处是功耗控制不精细、功耗高。【时钟关掉记得打开！！！】
1.6.5.5、CLK_DIVn
	各模块的分频器参数配置
1.6.5.6、CLK_GATE_x
	类似于CLK_SRC_MASK，对时钟进行开关控制
1.6.5.7、CLK_DIV_STATn
1.6.5.8、CLK_MUX_STATn
	这两类状态位寄存器，用来查看DIV和MUX的状态是否已经完成还是在进行中
	【如：48M分频到24M并不是瞬间完成，需要时间】
总结：其中最重要的寄存器有3类：CON、SRC、DIV。其中CON决定PLL倍频到多少，SRC决定走哪一路，DIV决定分频多少。

1.6.6.汇编实现时钟设置代码详解1
1.6.6.1、时钟设置的步骤分析：
	第1步：先选择不使用PLL。让外部24MHz原始时钟直接过去，绕过APLL那条路
	第2步：设置锁定时间。默认值为0x0FFF，保险起见我们设置为0xFFFF
	第3步：设置分频系统，决定由PLL出来的最高时钟如何分频得到各个分时钟
	第4步：设置PLL，主要是设置PLL的倍频系统，决定由输入端24MHz的原始频率可以得到多
			大的输出频率。我们按照默认设置值设置输出为ARMCLK为1GHz
	第5步：打开PLL。前面4步已经设置好了所有的开关和分频系数，本步骤打开PLL后PLL开始
			工作，锁定频率后输出，然后经过分频得到各个频率。
总结：以上5步，其实真正涉及到的寄存器只有5个而已

1.6.6.2、CLK_SRC寄存器的设置分析
	CLK_SRC寄存器其实是用来设置MUX开关的。在这里先将该寄存器设置为全0，主要是bit0和bit4设置为0，表示APLL和MPLL暂时都不启用
1.6.6.3、CLK_LOCK寄存器的设置分析
	设置PLL锁定延时的。官方推荐值为0xFFF，我们设置为0xFFFF
1.6.6.4、CLK_DIV寄存器的设置分析
0x14131440这个值的含义分析：
	PCLK_PSYS = HCLK_PSYS / 2
	HCLK_PSYS = MOUT_PSYS / 5
	PCLK_DSYS = HCLK_DSYS / 2
	HCLK_DSYS = MOUT_DSYS / 4
	·······
	HCLK_MSYS = ARMCLK / 5
	ARMCLK = MOUT_MSYS / 1

1.6.7.汇编实现时钟设置代码详解2
1.6.7.1、PLL倍频的相关计算
(1)、我们设置了APLL和MPLL两个，其他两个没有管
(2)、APLL和MPLL设置的关键都是M、P、S三个值，这三个值都来自于官方数据手册的推荐值
(3)、M、P、S的设置依赖《4.2.C语言位运算》中讲过的位运算技术

1.6.7.2、结合寄存器、时钟框图、代码三者综合分析S5PV210的时钟系统

1.6.7.3、代码编译、烧写实验

1.6.8.C语言实现时钟设置代码详解

1.6.8.1、C和汇编操作寄存器的不同

1.6.8.2、C的优势：位运算更加简单

1.6.8.3、用C语言重写时钟初始化代码























