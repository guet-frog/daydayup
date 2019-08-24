
3.1、开发环境搭建和确认
	(1) mount -t nfs -o nolock 192.168.1.77:/home/aston/src/rootfs/rootfs /opt
	(2) DPO_MT7601U_LinuxSTA_3.0.0.4_20130913.tar.bz2	// 源码包
	(3) lsusb查看网卡的VID和PID
		Bus 001 Device 003: ID 148f:7601

4.驱动源码修改及编译
	(1) 清理源码包 make clean
	(2) 确认USB的VID和PID	// usb_dev_id.c	 -- driver和dev配对
	(3) 确认该源码是否支持该设备id	// USB_DEVICE_ID rtusb_dev_id[];

4.2、修改Makefile
	(1) PLATFORM 		=	SMDK
	(2) LINUX_SRC 		= 	linux-src-tree
	(3) CROSS_COMPILE	= 	arm-linux-

	// 修改Makefile编译源码：ARCH CROSS_COMPILE
	// 编译驱动 -- 源码树目录

4.3、修改网卡名字(可选)
	(1) 常用无线网卡名称：rax、wlanx
	(2) 修改include/rtmp_def.h文件
		#define INF_MAIN_DEV_NAME "ra"
		#define INF_MBSSID_DEV_NAME "ra"

4.4、添加wpa_supplicant支持
	确保config.mk文件中WPA_SUPPLICANT=y		// 在driver源码根目录 find -name "config.mk"

4.5、编译生成驱动模块	// os/linux/mt7601Usta.ko
	// modinfo xxx.ko
	// 对比内核上电打印的 vermagic

5.1、驱动源码分析
	// 首先找到源码入口  -- 对于driver程序，搜索 module_init()

	// 在工程源码中搜索不到
	// 宏可以在Makefile、配置脚本中定义

	// 先 make clean
	// 后 grep "MT7601U" * -nR

	 -- os/linux/config.mk:674
	 WFLAG += -DMT7601U

	// 配置过程中，通过-D传递符号

6.1、iwconfig工具集的介绍和使用演示		 // busybox已经集成该命令
	(1) linux下专用来配置无线网络的一些命令集
	(2) 因为iwconfig本身有一定限制，只支持一定的加密格式

	/* 一般无线网卡都可以工作在sta和AP模式 */

7.1、wpa_supplicant配置文件		// 管理WiFi
	(2) /etc/Wireless/RT2870STA/RT2870STA.dat
	(3) /etc/wpa_supplicant.conf	// wpa工具配置文件

7.2、网卡配置命令序列
	insmod mt7601Usta.ko

	ifconfig ra0 up 	//开启无线网卡 -- 需要xxx.dat配置文件

	wpa_supplicant -B -c /etc/Wireless/RT2870STA/wpa_supplicant.conf -i ra0
	wpa_cli -i ra0 status

	ifconfig ra0 192.168.0.166
	ping 192.168.0.1

	route add default gw 192.168.0.1 dev ra0
	ping 8.8.8.8

	vi /etc/resolv.conf
	nameserver 192.168.0.1

	ping www.baidu.com   

8.2、使用dhcp动态分配IP		// udhcpc -i ra0

9.在自己定制的rootfs中移植网卡
9.1、确认自己制作的rootfs并启动
	(1)busybox交叉编译
	(2)启动后直接nfs方式挂载文件夹形式的rootfs，测试ok后再做成镜像烧录
	(3)挂载参数bootargs：setenv bootargs root=/dev/nfs nfsroot=192.168.1.141:/root/rootfs ip=192.168.1.10:192.168.1.141:192.168.1.1:255.255.255.0::eth0:off  init=/linuxrc console=ttySAC2,115200 

	set bootcmd 'tftp 30008000 zImage; bootm 30008000'
	
	// tftp服务器重启
	
9.2、需要的工具集确认
	(1) iwconfig工具集：另外移植的，不是busybox中的
	(2) dhcp工具集(udhcpc)：是busybox中集成的，确认busybox的menuconfig中配置支持了这个
	(3) wpa_supplicant工具集：另外移植的，不是busybox中的

9.3、交叉编译iwconfig
	(1)源码下载
	(2)配置
	(3)交叉编译
	(4)部署安装 	// file iwconfig -- dynamic linked  -- 安装在 /lib/libiw.so
	(5)测试
	
	PREFIX = /home/aston/src/rootfs/rootfs
	INSTALL_DIR=$(PREFIX)/sbin/
	
	// make
	// make install


10_11.移植wpa_supplicant并制作镜像1_2
10.1、交叉编译wpa_supplicant
(1)下载wpa_supplicant源码并配置编译。参考http://blog.csdn.net/hktkfly6/article/details/48949863
(2)下载配套版本的openssl并配置编译
(3)去掉配置NL相关的选项省去移植libnl。参考：http://www.cnblogs.com/helloworldtoyou/p/6145995.html
10.2、在nfs中测试wpa_supplicant使用
10.3、制作ext2镜像并刷机测试

附：/etc/wpa_supplicant.conf文件内容：
ctrl_interface=/var/run/wpa_supplicant

network={
        key_mgmt=WPA-PSK			// error
        ssid="zhulaoshi"
        psk="www.zhulaoshi.org"
}

// chmod -R 777 /var/

insmod /wifi_driver/mt7601Usta.ko
ifconfig ra0 up
wpa_supplicant -Dwext -ira0 -c/etc/Wireless/RT2870STA/wpa_supplicant.conf &
wpa_cli -i ra0 status








