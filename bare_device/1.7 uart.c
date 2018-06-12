
7.1.2、电子通信概念1：同步通信和异步通信
(1)、同步和异步的区别：首先很多地方都有同步和异步的概念，简单来说就是发送方和接收方按照同一个时钟节拍工作就叫同步，发送方和接收方没有统一的时钟节拍、而各自按照自己的节拍工作就叫异步。
(2)、同步通信中，通信双方按照统一节拍工作，一般需要发送方给接收方发送信息同时发送时钟信号，接收方根据发送方给它的时钟信号来安排自己的节奏。同步通信用在通信双方信息交换频率固定。
(3)、异步通信又叫异步通知。在双方通信的频率不固定时（有时3ms收发一次，有时3天才收发一次）不适合使用同步通信，而适合异步通信。异步通信时接收方不必一直在意发送方，发送方需要发送信息时会首先给接收方一个信息开始的起始信号，接收方接收到起始信号后就认为后面紧跟着的就是有效信息，才会开始注意接收信息，直到收到发送方发过来的结束标志。
7.1.3、电子通信概念2：电平信号和差分信号
(1)、电平信号和差分信号是用来描述通信线路传输方式的。也就是说如何在通信线路上表达1和0.
(2)、电平信号的传输线中有一个参考电平线（一般是GND），然后信号线上的信号值是由信号线电平和参考电平线的电压差决定。
(3)、差分信号的传输线中没有参考电平，所有都是信号线。然后1和0的表达靠信号线之间的电压差。
总结：电平信号的2根通信线之间的电平差异容易受到干扰，传输容易失败；差分信号不容易受到干扰因此传输质量比较稳定，现代通信一般都使用差分信号，电平信号几乎没有了。
总结2：看起来似乎相同根数的通信线下，电平信号要比差分信号要快；但是实际还是差分信号快，因为差分信号抗干扰能力强，因此1个发送周期更短。
(3)、在差分信号下，2根线（彼此差分）可以同时发送1位二进制；如果需要同时发送8位二进制，需要16根线。
总结：其实这么多年发展，最终胜出的是：异步、串行、差分，譬如USB和网络通信。
7.2.串口通信的基本概念
7.2.1、串口通信的特点：异步、电平信号、串行
(1)、异步：串口通信的发送方和接收方之间是没有统一的时钟信号的。
(2)、电平信号：串口通信出现的时间较早，速率较低，传输的距离较近，所以干扰还不太明显，因此当时使用了电平信号传输。后期出现的传输协议都改成差分信号传输了。
(3)、串行通信：串口通信每次同时只能传输1个二进制位。

7.2.2、RS232电平和TTL电平
(1)电平信号是用信号线电平减去参考线电平得到电压差，电压差决定了传输值是1还是0.
(2)在电平信号时多少V代表1，多少V代表0不是固定的，取决于电平标准。譬如RS232电平中-3V～-15V表示1；+3～+15V表示0；TTL电平则是+5V表示1，0V表示0.
(3)不管哪种电平都是为了在传输线上表示1和0.区别在于适用的环境和条件不同。RS232的电平定义比较大，适合干扰大、距离远的情况；TTL电平电压范围小，适合距离近且干扰小的情况。
(4)我们台式电脑后面的串口插座就是RS232接口的，在工业上用串口时都用这个，传输距离小于15米；TTL电平一般用在电路板内部两个芯片之间。
(5)对编程来说，RS232电平传输还是TTL电平是没有差异的。所以电平标准对硬件工程师更有意义，而软件工程师只要略懂即可。（把TTL电平和RS232电平混接是不可以的）
7.2.3、波特率
(1)波特率（bandrate），指的是串口通信的速率，也就是串口通信时每秒钟可以传输多少个二进制位。譬如每秒种可以传输9600个二进制位（传输一个二进制位需要的时间是1/9600秒，也就是104us），波特率就是9600.
(2)串口通信的波特率不能随意设定，而应该在一些值中去选择。一般最常见的波特率是9600或者115200（低端单片机如51常用9600，高端单片机和嵌入式SoC一般用115200）.为什么波特率不可以随便指定？主要是因为：第一，通信双方必须事先设定相同的波特率这样才能成功通信，如果发送方和接收方按照不同的波特率通信则根本收不到，因此波特率最好是大家熟知的而不是随意指定的。第二，常用的波特率经过长久发展，就形成了共识，大家常用就是9600或者115200.
7.2.4、起始位、数据位、奇偶校验位、停止位
(1)串口通信时，收发是一个周期一个周期进行的，每周期传输n个二进制位。这一个周期就叫做一个通信单元，一个通信单元是由：起始位+数据位+奇偶校验位+停止位组成的。
(2)起始位表示发送方要开始发送一个通信单元；数据位是一个通信单元中发送的有效信息位；奇偶校验位是用来校验数据位，以防止数据位出错的；停止位是发送方用来表示本通信单元结束标志的。
(3)起始位的定义是串口通信标准事先指定的，是由通信线上的电平变化来反映的。
(4)数据位是本次通信真正要发送的有效数据，串口通信一次发送多少位有效数据是可以设定的（一般可选的有6、7、8、9，99%情况下我们都是选择8位数据位。因为我们一般通过串口发送的文字信息都是ASCII码编码的，而ASCII码中一个字符刚好编码为8位。）
(5)奇偶校验位是用来给数据位进行奇偶校验（把待校验的有效数据逐个位的加起来，总和为奇数奇偶校验位就为1，总和为偶数奇偶校验位就为0）的，可以在一定程度上防止位反转。
(6)停止位的定义是串口通信标准事先指定的，是由通信线上的电平变化来反映的。常见的有1位停止位，1.5位停止位，2位停止位等。99%情况下都是用1位停止位。
总结：串口通信时因为是异步通信，所以通信双方必须事先约定好通信参数，这些通信参数包括：波特率、数据位、奇偶校验位、停止位（串口通信中起始位定义是唯一的，所以一般不用选择）
7.3.1、三根通信线：Rx Tx GND
(1)任何通信都要有信息传输载体，或者是有线的或者是无线的。
(2)串口通信是有线通信，是通过串口线来通信的。
(3)串口通信线最少需要2根（GND和信号线），可以实现单工通信，也可以使用3根通信线（Tx、Rx、GND）来实现全双工。
(4)一般开发板都会引出SoC上串口引脚直接输出的TTL电平的串口（X210开发板没有），插座用插针式插座，每个串口引出的都有3个线（Tx、Rx、GND），可以用这些插座直接连接外部的TTL电平的串口设备。
7.3.2、收发双方事先规定好通信参数（波特率、数据位、奇偶校验位、停止位等）
(1)串口通信属于基层基本性的通信规约，它自己本身不会去协商通信参数，需要通信前通信双方事先约定好通信参数（一般4个最重要的）
(2)串口通信的任何一个关键参数设置错误，都会导致通信失败。譬如波特率调错了，发送方发送没问题，接收方也能接收，但是接收到全是乱码···
7.3.3、信息以二进制流的方式在信道上传输
(1)、串口通信的发送方每隔一定时间（时间固定为1/波特率，单位是秒）将有效信息（1或者0）放到通信线上去，逐个二进制位的进行发送。
(2)接收方通过定时（起始时间由读到起始位标志开始，间隔时间由波特率决定）读取通信线上的电平高低来区分发送给我的是1还是0。依次读取数据位、奇偶校验位、停止位，停止位就表示这一个通信单元（帧）结束，然后中间是不定长短的非通信时间（发送方有可能紧接着就发送第二帧，也可能半天都不发第二帧，这就叫异步通信），下来就是第二帧·····
总结：第一，波特率非常重要，波特率错了整个通信就乱套了；数据位、奇偶校验位、停止位也很重要，否则可能认不清数据。第三，通过串口不管发数字、还是文本还是命令还是什么，都要先对发送内容进行编码，编码成二进制再进行逐个位的发送。
(3)串口发送的一般都是字符，一般都是ASCII码编码后的字符，所以一般设置数据位都是8，方便刚好一帧发送1个字符。




7.3.5、DB9接口介绍
(1)DB9接口是串口通信早期比较常用的一种规范化接口。
(2)串行通信在早期是计算机与外界通信的主要手段，那时候的计算机都有标准配置的串口以实现和外部通信。那时候就定义了一套标准的串口规约，DB9接口就是标准接口。
(3)DB9接口中有9根通信线，其中3根很重要，为GND、Tx、Rx，必不可少；剩余6根都是和流控有关的，现代我们使用串口都是用来做调试一般都禁用流控，所以这6根没用。
(4)现在一般使用串口时要记得把流控禁止掉，不然可能发生意想不到的问题。

7.4 S5PV210串行通信接口详解1
7.4.0、串口的名称
(1)S5PV210的数据手册中串口控制器在section8.1
(2)串口的官方名称叫：universal asynchronous reciver and transmitter，通用异步收发器
英文缩写是uart，中文简称串口。
7.4.1、S5PV210的串口控制器工作原理框图
(1)整个串口控制器包含transmitter和receiver两部分，两部分功能彼此独立，transmitter负责210向外部发送信息，receiver负责从外部接收信息到210内部。
(2)总线角度来讲，串口控制器是接在APB总线上的。对我们编程有影响的是：将来计算串口控制器的源时钟时是以APB总线来计算的。
(3)transmitter由发送缓冲区和发送移位器构成。我们要发送信息时，首先将信息进行编码（一般用ASCII码）成二进制流，然后将一帧数据（一般是8位）写入发送缓冲区（从这里以后程序就不用管了，剩下的发送部分是硬件自动的），发送移位器会自动从发送缓冲区中读取一帧数据，然后自动移位（移位的目的是将一帧数据的各个位分别拿出来）将其发送到Tx通信线上。
(4)receiver由接收缓冲区和接收移位器构成。当有人通过串口线向我发送信息时，信息通过Rx通信线进入我的接收移位器，然后接收移位器自动移位将该二进制位保存入我的接收缓冲区，接收完一帧数据后receiver会产生一个中断给CPU，CPU收到中断后即可知道receiver接收满了一帧数据，就会来读取这帧数据。
总结：发送缓冲区和接收缓冲区是关键。发送移位器和接收移位器的工作都是自动的，不用编程控制的，所以我们写串口的代码就是：首先初始化（初始化的实质是读写寄存器）好串口控制器（包括发送控制器和接收控制器），然后要发送信息时直接写入发送缓冲区，要接收信息时直接去接收缓冲区读取即可。可见，串口底层的工作（譬如怎么移位的、譬如起始位怎么定义的、譬如TTL电平还是RS232电平等）对程序员是隐藏的，程序员不用去管。软件工程师对串口操作的接口就是发送/接收缓冲区（实质就是寄存器，操作方式就是读写内存）
(5)串口控制器中有一个波特率发生器，作用是产生串口发送/接收的节拍时钟。波特率发生器其实就是个时钟分频器，它的工作需要源时钟（APB总线来），然后内部将源时钟进行分频（软件设置寄存器来配置）得到目标时钟，然后再用这个目标时钟产生波特率（硬件自动的）。

7.4.2、自动流控（AFC：Auto flow control）
(1)为什么需要流控？流控的目的是让串口通信非常可靠，在发送方速率比接收方快的时候流控可以保证发送和接收不会漏掉东西。
(2)现在为什么不用流控？现在计算机之间有更好更高级（usb、internet）的通讯方式，串口已经基本被废弃了。现在串口的用途更多是SoC用来输出调试信息的。由于调试信息不是关键性信息、而且由于硬件发展串口本身速度已经相对慢的要死了，所以硬件都能协调发送和接收速率，因此流控已经失去意义了，所以现在基本都废弃了。

7.5.S5PV210串行通信接口详解2
1.7.5.0、本来串口的功能就是上节讲过的部分，但是后来的技术发展给串口叠加了一些高级功能，在像210这类的高级SoC的串口控制器中，都有这类高级功能。
7.5.1、FIFO模式及其作用
(1)典型的串口设计，发送/接收缓冲区只有1字节，每次发送/接收只能处理1帧数据。这样在单片机中没什么问题，但是到复杂SoC中（一般有操作系统的）就会有问题，会导致效率低下，因为CPU需要不断切换上下文。
(2)解决方案就是想办法扩展串口控制器的发送/接收缓冲区，譬如将发送/接收缓冲器设置为64字节，CPU一次过来直接给发送缓冲区64字节的待发送数据，然后transmitter慢慢发，发完再找CPU再要64字节。但是串口控制器本来的发送/接收缓冲区是固定的1字节长度的，所以做了个变相的扩展，就是FIFO。
(3)FIFO就是first in first out，先进先出。fifo其实是一种数据结构，这里这个大的缓冲区叫FIFO是因为这个缓冲区的工作方式类似于FIFO这种数据结构。

7.5.2、DMA模式及其作用
(1)DMA direct memory access，直接内存访问。DMA本来是DSP中的一种技术，DMA技术的核心就是在交换数据时不需要CPU参与，模块可以自己完成。
(2)DMA模式要解决的问题和上面FIFO模式是同一个问题，就是串口发送/接收要频繁的折腾CPU造成CPU反复切换上下文导致系统效率低下。
(3)传统的串口工作方式（无FIFO无DMA）效率是最低的，适合低端单片机；高端单片机上CPU事物繁忙所以都需要串口能够自己完成大量数据发送/接收。这时候就需要FIFO或者DMA模式。FIFO模式是一种轻量级的解决方案，DMA模式适合大量数据迸发式的发送/接收时。

7.5.3、IrDA模式及其用法
(1)IrDA其实就是红外，红外就是红外线通信（电视机、空调遥控器就是红外通信的）。
(2)红外通信的原理是发送方固定间隔时间向接收方发送红外信号（表示1或0）或者不发送红外信号（表示0或者1），接收方每隔固定时间去判断有无红外线信号来接收1和0.
(3)分析可知，红外通信和串口通信非常像，都是每隔固定时间发送1或者0（判断1或0的物理方式不同）给接收方来通信。因此210就利用串口通信来实现了红外发送和接收。
(4)210的某个串口支持IrDA模式，开启红外模式后，我们只需要向串口写数据，这些数据就会以红外光的方式向外发射出去（当然是需要一些外部硬件支持的），然后接收方接收这些红外数据即可解码得到我们的发送信息。

7.6.S5PV210串行通信接口详解3
1.7.6.1、串行通信与中断的关系
(1)串口通信分为发送/接收2部分。发送方一般不需要（也可以使用）中断即可完成发送，接收方必须（一般来说必须，也可以轮询方式接收）使用中断来接收。
(2)发送方可以选择使用中断，也可以选择不使用中断。使用中断的工作情景是：发送方先设置好中断并绑定一个中断处理程序，然后发送方丢一帧数据给transmitter，transmitter发送耗费一段时间来发送这一帧数据，这段时间内发送方CPU可以去做别的事情，等transmitter发送完成后会产生一个TXD中断，该中断会导致事先绑定的中断处理程序执行，在中断处理程序中CPU会切换回来继续给transmitter放一帧数据，然后CPU切换离开；不使用中断的工作情景是：发送方事先禁止TXD中断（当然也不需要给相应的中断处理程序了），发送方CPU给一帧数据到transmitter，然后transmitter耗费一段时间来发送这帧数据，这段时间CPU在这等着（CPU没有切换去做别的事情），待发送方发送完成后CPU再给它一帧数据继续发送直到所有数据发完。CPU是怎么知道transmitter已经发送完了？原来是有个状态寄存器，状态寄存器中有一个位叫发送缓冲区空标志，transmitter发送完成（发送缓冲区空了）就会给这个标志位置位，CPU就是通过不断查询这个标志位为1还是0来指导发送是否已经完成的。
(3)因为串口通信是异步的，异步的意思就是说发送方占主导权。也就是说发送方随时想发就能发，但是接收方只有时刻等待才不会丢失数据。所以这个差异就导致发送方可以不用中断，而接收方不得不使用中断模式。


7.6.2、210串行通信接口的时钟设计
(1)串口通信为什么需要时钟？因为串口通信需要一个固定的波特率，所以transmitter和receiver都需要一个时钟信号。
(2)时钟信号从哪里来？源时钟信号是外部APB总线（PCLK_PSYS，66MHz）提供给串口模块的（这就是为什么我们说串口是挂在APB总线上的），然后进到串口控制器内部后给波特率发生器（实质上是一个分频器），在波特率发生器中进行分频，分频后得到一个低频时钟，这个时钟就是给transmitter和receiver使用的。
(3)串口通信中时钟的设置主要看寄存器设置。重点的有：寄存器源设置（为串口控制器选择源时钟，一般选择为PCLK_PSYS，也可以是SCLK_UART），还有波特率发生器的2个寄存器。
(4)波特率发生器有2个重要寄存器：UBRDIVn和UDIVSLOTn，其中UBRDIVn是主要的设置波特率的寄存器，UDIVSLOTn是用来辅助设置的，目的是为了校准波特率的。

7.7.S5PV210串行通信编程实战1
7.7.1、整个程序流程分析
(1)整个串口通信相关程序包含2部分：uart_init负责初始化串口，uart_putc负责发送一个字节
7.7.2、串口控制器初始化关键步骤
(1)初始化串口的Tx和Rx引脚所对应的GPIO（查原理图可知Rx和Rx分别对应GPA0_1和GPA0_0）
(2)GPA0CON（0xE0200000），bit[3:0] = 0b0010	bit[7:4] = 0b0010	
(3)初始化这几个关键寄存器UCON0 ULCON0 UMCON0 UFCON0 UBRDIV0 UDIVSLOT0
1.7.7.3、主要的几个寄存器
(1)ULCON0 = 0x3		// 0校验位、8数据位、1停止位
(2)UCON = 0x5		// 发送和接收都是polling mode【轮询】
(3)UMCON0 = 0x0		// 禁止modem、afc
(4)UFCON0 = 0x0		// 禁止FIFO模式
(5)UBRDIV0和UDIVSLOT0和波特率有关，要根据公式去算的

7.7.4、在C源文件中定义访问寄存器的宏
定义好了访问寄存器的宏之后，将来写代码时直接使用即可。
7.8.S5PV210串行通信编程实战2
7.8.1、串口Tx、Rx对应的GPIO的初始化
	给GPA0CON的相应bit位赋值为相应值，用C语言位操作来完成。
7.8.2、UCON、ULCON、UMCON、UFCON等主要控制寄存器
	依据上节中分析的值进行依次设置即可。
7.8.3、波特率的计算和设置
(1)第一步，用PCLK_PSYS和目标波特率计算DIV_VAL: DIV_VAL = (PCLK / (bps x 16))-1
(2)第二步，UBRDIV0寄存器中写入DIV_VAL的整数部分
(3)第三步，用小数部分*16得到1个个数，查表得uBDIVSLOT0寄存器的设置值
7.8.4、串口发送和接收函数的编写
(1)写发送函数，主要发送前要用while循环等待发送缓冲区为空才能发送。
7.8.5、综合调试
注意Makefile的修改。
7.8.6、扩展练习-更改波特率后再调试
自己练习。注意程序中改了波特率后，SecureCRT也要相应修改，不然收不到东西。

7.9.uart stdio的移植1
7.9.1、什么是stdio
(1)#include <stdio.h>
(2)stdio：standard input output，标准输入输出
(3)标准输入输出就是操作系统定义的默认的输入和输出通道。一般在PC机的情况下，标准输入指的是键盘，标准输出指的是屏幕。
【每一个进程都有三个流：标准输入输出是操作系统可以定义的两个流，标准错误】
(4)printf函数和scanf函数可以和底层输入/输出函数绑定，然后这两个函数就可以和stdio绑定起来。也就是说我们直接调用printf函数输出，内容就会被从标准输出输出出去。
(5)在我们这里，标准输出当然不是屏幕了，而是串口。标准输出也不是键盘，而是串口。

7.9.2、printf函数的工作原理
(1)printf函数工作时内部实际调用了2个关键函数：一个是vsprintf函数（主要功能是格式化打印信息，最终得到纯字符串格式的打印信息等待输出），另一个就是真正的输出函数putc（操控标准输出的硬件，将信息发送出去）

7.9.3、移植printf函数的三种思路
(1)我们希望在我们的开发板上使用printf函数进行（串口）输出，使用scanf函数进行（串口）输入，就像在PC机上用键盘和屏幕进行输入输出一样。因此需要移植printf函数/scanf函数
(2)我们说的移植而不是编写，我们不希望自己完全从新编写而是想尽量借用也有的代码（叫移植）
(3)一般移植printf函数可以有3个途径获取printf的实现源码：最原始最原本的来源就是linux内核中的printk。难度较大、关键是麻烦；稍微简单些的方法是从uboot中移植printf；更简单的方法就是直接使用别人移植好的。
(3)我们课程中使用第三种方法，别人移植好的printf函数来自于友善之臂的Tiny210的裸机教程中提供的。


7.9.4、移植好的printf介绍
参考视频中讲解。【uartstdio的移植1.mp4 移植2.mp4 Makefile的修改】
objs := div64.o lib1funcs.o ctype.o muldi3.o printf.o string.o vsprintf.o
定义变量objs，保证所有的源文件都能被编译
objs := xxx.o xxx.o xxx.o
uart.bin: $(objs)              //在Makefile中定义一个变量，再去引用这个变量【$ 引用】
libc.a: $(objs)
目标是libc.a : .a表示是一个库，相当于Windows中的静态链接库.lib 
            .so Linux中动态链接库，Windows中叫 .dll
			   			         
7.10.uart stdio的移植2
7.10.1、修改Makefile进行printf移植
7.10.2、Makefile及gcc的库文件介绍
7.10.3、多文件夹裸机工程的结构解析
7.10.4、编译运行及测试

7.11.uart stdio的移植3
7.11.1、在移植后的uart stdio项目中添加link.lds链接脚本，指定连接地址到0xd0020010
7.11.2、gcc可变参数及va_arg介绍
(1)printf函数中首先使用了C语言的可变参数va_start/va_arg/va_end；
(2)建议大家先去baidu“C语言可变参数”，然后按照别人的教程、博客实际写几个简单的变参的使用示例，先明白可变参数怎么工作，然后再来分析这里。
7.11.3、vsprintf函数详解
printf
	vsprintf
		vsnprintf
			number
vsprintf函数的作用是按照我们的printf传进去的格式化标本，对变参进行处理，然后将之格式化后缓存在一个事先分配好的缓冲区中。
printf后半段调用putc函数将缓冲区中格式化好的字符串直接输出到标准输出。

7.12.串口实验烧录问题总结
7.12.1、usb下载的问题
(1)USB下载时在Win7 X64系统下，下载前面章节的小代码时没问题，下载串口通信的小代码时也没问题，下载uart stdio的移植就有问题了。有时候下载不动、有时候能下载但是不运行、有时候又正常下载运行。我已经试过下载其他的dnw或dnw驱动更新，都无法解决。
7.12.2、SD卡镜像烧录
(1)SD卡烧录镜像做裸机实验，在第四部分1.4.2节中有讲过。
(2)本次我们在Windows下烧录（linux下的烧录参考以前的）
(3)Windows下烧录镜像是使用九鼎提供的工具（X210光盘资料\A盘\tools\x210_Fusing_Tool.exe），注意运行时右键“以管理员身份运行”。
7.12.3、启动方式设置
(1)X210开发板的启动方式的选择，请参考1.2.11节。其实就是OM5的问题，OM5设置为VCC则从USB启动，OM5设置成GND，则从iNand/SD卡启动。
(2)开发板选择从iNand启动后，还要确保iNand中uboot是被擦除的。
(3)关于如何破坏uboot的问题，大家可以参考之前课程中讲的在linux/android系统中破坏uboot的方法。我之前讲过在uboot中破坏uboot的方法：movi write u-boot 0x30000000。很多同学反映擦除后错乱，进不了系统也从SD卡启动不了，只能通过USB刷机来解决。后来又分析，改为：mw 0x30000000 0x0 0x100000，然后再movi write u-boot 0x30000000
。但是反馈结果有人说可以了，有人说还是不行·······
(4)不管怎么擦除uboot，总之首先确保你的板子SD卡启动是成功的。怎么确保？先用SD卡烧录启动之前的LED闪烁的项目，确保看到现象就证明烧录SD卡方法和启动SD卡都成功了，再做本节课的实验。
7.12.4、链接脚本的影响
7.12.5、bin文件大于16KB怎么办？
通过USB下载最多也只能下载96KB大小的bin，如果bin大于96KB肯定SRAM放不下会出错。如果用SD卡启动，那么mkv210_image.c决定了bin文件最大不能超过16KB。
超过了怎么办？2种解法：
第一，在USB下载时，可以先下载一个x210_usb.bin，然后再将裸机程序连接到0x23E00000，然后再修改dnw中下载地址，将裸机代码下载到0x23E00000运行。（这时不需要重定位了）
第二，在SD卡启动时，将整个裸机工程分为2部分；第一部分大小16KB以内，第二部分放剩下的（放在SD卡的后面的某个扇区开始的位置，譬如放在第50个扇区开始的位置），然后在裸机代码中进行重定位（SD卡中重定位）。这个暂时没讲，以后如果有用到就讲。





