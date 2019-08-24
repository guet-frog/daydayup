
1.3.0、demo板测试
	(5) 运行视频服务器例程/mnt/sample_venc		// /home/aston/rootfs
	(6) 在vlc中输入地址，即可浏览实时画面
		菜单栏：媒体->打开网络串流->网络，输入：rtsp://192.168.1.10:554/stream_chn0.h264
		勾选：显示更多选项。在正在缓冲中设置为300（原来是1000）

1.3.1、整个流程
	(2) 原始视频产生：镜头和sensor
	(3) 图像处理：ISP(image signal processing)
	(4) 视频编码压缩：h.264/h.265压缩算法，运算，内置DSP进行压缩运算
	(5) 视频流传输：网络传输、http/rtsp等
	(6) 视频存储：打包成MP4等格式存储，等待调阅
	(7) 视频回放：解码+播放

1.4.1、HI3518E硬件信息
	(1) HI3518EV200单芯片提供：CPU+DSP+内64MB DDR+ETHERNET MAC
	(2) spi flash、sdCard、usb wifi、sensor接口、串口

	// 产品出货一般使用8Mflash
	// 一般kernel可以精简到1M多
	// 4G emmc 20+

1.6.1、2篇相关文档		// linux开发环境、uboot开发环境
1.6.2、SDK包复制到linux原生目录中并解压		// tar -xvf 
1.6.3、SDK包操作的脚本程序研究 // 主要分析主目录下3个脚本 -- 执行不成功通过source xxx.sh
							   
// which sh
// ls -l bash
// ln -s /bin/bash /bin/sh

1.8.在ubuntu16.0403X64上安装海思交叉编译工具链
1.8.2、给ubuntu16.0403X64安装32位兼容包
	(1) 参考：http://blog.csdn.net/ma57457/article/details/68923623
		或者：https://www.cnblogs.com/leaven/p/5084902.html
	(2) 用aptitude方式安装lib32z1。使用sudo apt-get install lib32z1，发现装不了。
		原因是ubuntu太新了，里面很多库都是新版本的，但是基于这个新版本的lib32z1还
		没有，所以不能装。解决方案就是用aptitude工具来装。看前导课程《嵌入式linux
		开发环境搭建》的第6节的6.3部分。
	(3) 测试执行arm-xxx-gcc -v，提示找不到stdc++错误：
		./arm-hisiv300-linux-uclibcgnueabi-gcc -v
		./arm-hisiv300-linux-uclibcgnueabi-gcc: error while loading shared libraries: libstdc++.so.6: cannot open shared object file: No such file or directory
	(4) 再用aptitude方式安装lib32stdc++6-4.8-dbg
		sudo aptitude install lib32stdc++6-4.8-dbg
		再次测试arm-xxx-gcc -v，终于可以运行了。

1.8.3、再次测试整体编译osdrv
	(1) 仍然提示找不到arm-hisiv300-linux-gcc
	(2) 修改Makefile中OSDRV_CROSS的路径，结果不行，分析原因是：命令行传参覆盖了
	(3) export导出到环境变量，参考裸机课程1.4节，格式为：
		export PATH=/home/aston/sambashare/Hi3518E_SDK_V1.0.3.0/osdrv/arm-hisiv300-linux/bin:$PATH

		// vi ~/.bashrc
		// export ...
		// source ~/.bashrc

		直接测试可以执行了，但是编译还是出错。分析原因：名字不对
	(4) 解决方案有2个：一种是修改make时传参的名字，
		另一种是给安装好的交叉编译工具链创建符号链接。
		实际尝试后发现第一种Makefile要改的太多，所以走第2种。

1.8.4、使用install脚本安装交叉编译工具链
	(1) install到/opt目录下并建立符号链接
	(2) 导出到PATH并测试可以执行
	(3) 再次编译

1.9.编译osdrv
1.9.1、编译策略和方法研究
	(1)整体编译，遇到问题解决问题。
		// /uboot/tools/mkimage

	(2)部分编译，需要哪部分单独编译哪部分。
1.9.2、编译uboot
1.9.3、编译kernel

1.10.1、缺zlib错误。
	错误：compr_zlib.c:39:18: fatal error: zlib.h: No such file or directory
	因为找不到zlib.h所以编译错误，
	zlib.h在tools/pc/zlib/tmp/include目录中有，
	只需要复制到tools/pc/jffs2_tool/tmp/include目录中即可。注意同时要将
	zconf.h也复制过去的。还有，要将tools/pc/jffs2_tool/tmp/lib目录下
	的libz.a  libz.so  libz.so.1  libz.so.1.2.7等4个文件复制到
	tools/pc/jffs2_tool/tmp/lib目录下，不然一会儿还得报错。

1.10.2、其他错误
	还报错就不管了，此时已经有了mkfs.jffs2了，我们就是只要这个而已，手工
	将其复制到osdrv/pub/bin/pc目录下即可。

1.10.2、报错。
	错误信息：serve_image.c:32:18: error: storage size of ‘hints’ isn’t known
	解决方案，参考：http://blog.csdn.net/mtbiao/article/details/77052659

1.10.3、在32位ubuntu中编译时的一个错误
	编译错误：/usr/bin/ld: i386:x86-64 architecture of input 
	file `mkyaffs2image.o' is incompatible with i386 output
	这个是因为sdk中本来就有在64位系统下编译的.o文件，而我们用的是32位的
	ubuntu，解决办法是进入tools/pc/mkyaffs2image/mkyaffs2image目录下，
	rm *.o（或者make clean）删除所有之前编译的痕迹即可。实际上我们板子上
	用的是spi flash，合适用jffs2文件系统，所以不会做yaffs2文件系统，
	这个东西要不要都无所谓的。

1.10.4、手工单独制作rootfs
	(1) mkfs.jffs2 -d rootfs_uclibc -l -e 0x10000 -o rootfs_uclibc_64k.jffs2

1.12.1、裸机烧录uboot
	(1) 上位机HiTool + isp

1.12.2、flash分区	// 手动指定管理
	(1) bootloader	// 0x00000000 - 0x00100000 1M
	(2) kernel		// 0x00100000 - 0x00400000 3M
	(3) rootfs		// 0x00400000 - 0x01000000 12M

1.16.1、mpp的文件结构详解
	(1) ko			// cp ko / -r
	(2) lib			// cp lib/* /usr/lib/		-- /lib 系统库
	(3) sample
	(4) 其他几个

1.16.2、开发板启动自动挂载主机
	(1) 在profile中添加设置网卡IP地址
	(2) 在profile中添加自动挂载主机nfs服务器
		mount -t nfs -o nolock 192.168.1.141:/home/aston/rootfs /mnt

		/home/aston/rootfs	// sudo showmount -e

1.16.3、部署ko文件	// mpp/ko/xxx.sh
	./load3518e -i -sensor ar0130 -osmem 32 -total 64

	// insert_moudle()
	// insmod xxx
	
	// himm 0x2000f007C 0x1;		himm命令初始化sensor

1.16.4、部署lib文件

1.18.sample的编译和测试		// sample/venc/		录像功能
1.18.1、sample的编译
	(1) sample结构简单浏览		// *.ko *.so *.a需要部署到roofs; *.h编译时用
	(2) Makefile研究			// mpp/sample/Makefile.param	-- 配置chip_id sensor_type
	(3) 编译得到sample_venc

1.18.2、sample的部署和测试
	(1) nfs方式运行sample			// 不能再board上直接运行 -- flash大小
	(2) 得到录像文件xx.h264
	(3) 导出xx.h264到windows下用vlc播放器播放验证

1.18.3、镜像重新制作
	(1) 在编译目录的osdev/pub/下找到rootfs_uclibc.tgz，解压开
	(2) 按照上节和本节验证成功的部署步骤部署整个rootfs
	(3) 手工制作rootfs.jffs2镜像，再烧录测试即可
		osdrv/pub/bin/pc/mkfs.jffs2 -d osdrv/pub/rootfs_uclibc -l -e 0x10000 -o osdrv/pub/rootfs_uclibc_64k.jffs2

	SDRAM		// 80000000 - 83FFFFFF 		64M
	linux		// 80000000 - 81FFFFFF		32M
	MMZ			// 82000000 - 83FFFFFF		32M

// ---------------------------------------------------------
tftp更新并重新烧写kernel的命令序列：
mw.b 0x82000000 ff 0x300000
tftp 0x82000000 uImage_hi3518ev200
sf probe 0
sf erase 0x100000 0x300000
sf write 0x82000000 0x100000 0x300000
---------------------------------------------------
tftp更新并重新烧写rootfs的命令序列：
mw.b 0x82000000 ff 0xc00000
tftp 0x82000000 rootfs_uclibc_64k.jffs2
sf probe 0
sf erase 0x400000 0xc00000
sf write 0x82000000 0x400000 0xc00000

set bootcmd 'sf probe 0;sf read 0x82000000 0x100000 0x300000;bootm 0x82000000'
set bootargs mem=32M console=ttyAMA0,115200 root=/dev/mtdblock2 rootfstype=jffs2 mtdparts=hi_sfc:1024K(boot),3072K(kernel),12288K(rootfs)
// cat /proc/cmdline

root=/dev/mtdblock2 	// block0 - uboot, block1 - kernel, block2 - root


