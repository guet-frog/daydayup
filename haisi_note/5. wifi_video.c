
第二部分、章节介绍
5.1.海思平台上USB WIFI移植概述
	本节讲解本季课程的起点和先前相关的课程，让大家对USB WIFI移植有个计划。
5.2.AP模式USB WIFI驱动移植
	本节带大家手把手移植USB WIFI在AP模式下的驱动源码。
5.3.AP模式USB WIFI传输视频实战
	本节手把手带大家部署USB WIFI，并且做好设置，使用Windows连接开发板做视频传输。
5.4.USB WIFI做sta模式的驱动移植和部署
	本节带大家手把手移植USB WIFI在sta模式下的驱动源码。
5.5.移植wpa_supplicant
	本节带大家手把手移植wpa_supplicant以及openssl，这个是linux下管理wifi的利器。
5.6.USB WIFI做sta模式的ORTP视频传输实验
	本节手把手带大家部署sta模式的USB WIFI，并且连接路由器，使用Windows连接开发板做视频传输。
5.7.纯WIFI无线调试环境的搭建
	本节将虚拟机ubuntu也拉进来，实现开发板、windows、虚拟机ubuntu三者作为sta连接路由器，并且做好设置，开机自动无线调试。
	
	// 图传模块

5.1.2、移植的起点
(1)《 》课程，
(2)基于USB WIFI网卡官方移植匹配后的驱动来移植
(3)基于海思SDK中已经编译过的内核源码树来编译驱动
5.1.3、本季课程安排
(1)HI3518E+WIFI做AP，PC做sta，实现局域网内ortp视频传输
(2)HI2518E+WIFI做sta，PC做sta，外部路由器做AP，实现局域网内ortp视频传输
(3)HI2518E+WIFI做sta，PC做sta，外部路由器做AP，实现全程无线调试（彻底不用有线网卡）


5.2.AP模式USB WIFI驱动移植
5.2.1、源码
(1)复制到ubuntu中实验目录
(2)解压进入
5.2.2、修改移植
(1)include/rtmp_def.h中1627行左右，修改网络名始终为：wlan
(2)使用提供的Makefile替换掉原来的Makefile
(3)./os/linux/config.mk中添加EXTRA_FLAGS
(4)修改Makefile或者手工复制xx.ko到/home/aston/rootfs中


5.3.AP模式USB WIFI传输视频实战
5.3.1、部署USB WIFI驱动使之工作为AP
(1)先lsusb，然后插入USB WIFI模块，再lsusb，确认模块被识别了
(2)在开发板中部署 /etc/Wireless/RT2870AP/RT2870AP.dat，并修改以下几个配置项
	SSID=MT7601AP_WPA（这里的名字是在pc机上看到的连接ssid名）
	AuthMode=WPA2PSK（加密方式）
	EncrypType=TKIP;AES
	WPAPSK=1234567890（这个是密码）
(3)insmod xx.ko安装USB WIFI驱动模块
(4)ifconfig -a看是否有wlan0
(5)ifconfig wlan0 up, ifconfig wlan0 192.168.0.100
(6)ifconfig查看，确认wlan0工作正常
(7)用PC或手机的WIFI搜索看一下，是否有一个名为：MT7601AP_WPA的网络了，有了就成功了
(8)给PC强制分配静态IP
5.3.2、测试用例准备
(1)ORTP版本的sample，common/sample_common_venc.c中，修改LOCAL_HOST_IP为192.168.0.30
(2)make clean，make
(3)复制得到的sample_venc到/home/aston/rootfs中
5.3.3、测试实验
(1)开发板端：cp /mnt/sample_venc /home/	然后cd /home
(2)拔掉网线（此时也可以重启下开发板以彻底消除前面的影响）
(3)PC端WIFI连接 MT7601AP_WPA，输入密码 1234567890
(4)PC端设置无线网络的静态IP地址为：192.168.0.30
(5)PC端cmd去ping 192.168.0.100，或者反过来开发板端ping PC的无线IP
(6)开发板端执行 ./sample_venc
(7)PC端打开vlc播放器，使用第3季中的sdp配置，注意c=IN IP4 192.168.0.100，就能播放了。
注：
无线：
windows：	192.168.0.30
AP：		192.168.0.100
ubuntu：	192.168.0.50	（ubuntu的IP现在可以不管）

有线：
windows：	192.168.1.20
开发板：	192.168.1.10
ubuntu：	192.168.1.141


5.4.USB WIFI做sta模式的驱动移植和部署
5.4.1、源码修改
(1)用sta版本的driver，可以简单看一下区别
(2)修改makefile，参考AP模式下的修改点修改
(3)config.mk中修改，参考AP模式下的修改点修改
(4)修改网卡名称为wlan
(5)make clean && make
5.4.2、部署驱动
(1)部署/etc/Wireless/RT2870STA/RT2870STA.dat 
(2)部署ko


5.5.移植wpa_supplicant
5.5.1、源码下载
(1)wpa_supplicant	 http://hostap.epitest.fi/wpa_supplicant/
(2)openssl			 ftp://ftp.openssl.org/source/old/0.9.x/
下载openssl-0.9.8za.tar.gz
分别tar -zxvf解压wpa_supplicant-2.5.tar.gz和openssl-0.9.8za.tar.gz，
5.5.2、移植openssl
(1)给openssl打补丁。把wpa_supplicant-2.5里面的patches文件夹下的openssl-0.9.8za-tls-extensions.patch文件拷贝到openssl-0.9.8za目录下，运行：patch -p1 < openssl-0.9.8za-tls-extensions.patch
(2)修改Makefile
CC=arm-hisiv300-linux-gcc
AR=arm-hisiv300-linux-ar $(ARFLAGS) 
RANLIB=arm-hisiv300-linux-ranlib

INSTALLTOP = /tmp/openssl
OPENSSLDIR = /tmp/openssl
(3)make && make install
(4)如果报错
****Expected text after =item, not a number
****Expected text after =item, not a number
****Expected text after =item, not a number
****Expected text after =item, not a number
原因：OpenSSL 与 perl版本不兼容
解决方法：rm -f /usr/bin/pod2man 
5.5.3、移植wpa_supplicant
(1)解压并进入主目录
(2)cp /wpa_supplicant/defconfig .config
(3)修改.config，增加以下内容：
CC=arm-hisiv300-linux-gcc -L/tmp/openssl/lib
CFLAGS+=-I /tmp/openssl/include
LIBS+=-L/tmp/openssl/lib
(4)make		如果报错：
driver_nl80211.c:17:31: fatal error: netlink/genl/genl.h
进入wpa_supplicant目录下.config 
将CONFIG_DRIVER_NL80211=y 注释掉即可
(5)继续make，生成wpa_supplicant和wpa_cli后整个移植成功完成


5.6.USB WIFI做sta模式的ORTP视频传输实验
5.6.1、部署wpa_supplicant
(1)将wpa_supplicant、wpa_cli丢到开发板/usr/local/bin目录下
(2)创建配置文件/etc/wap_supplicant.conf，内容如下：
	ctrl_interface=/var/run/wpa_supplicant

	network={
	ssid="ZLSWLW-2"
	scan_ssid=1
	key_mgmt= WPA-EAP WPA-PSK IEEE8021X NONE
	pairwise=TKIP CCMP
	group=CCMP TKIP WEP104 WEP40
	psk="justdoit1234"
	}
5.6.2、sta模式的USB WIFI设置
(1)安装驱动 insmod mt7601Usta.ko
(2)设置静态IP地址
ifconfig wlan0 up, ifconfig wlan0 192.168.0.233
(3)开发板sta连接AP	（-dd打开调试信息）
wpa_supplicant -Dwext -iwlan0 -c/etc/wpa_supplicant.conf -dd &
(4)查看连接状态
wpa_cli -i wlan0 status      
(5)测试连接效果
ping 192.168.0.1		（网关AP）
route add default gw 192.168.0.1 dev wlan0
ping 8.8.8.8			（google的dnw server）
ping www.zhulaoshi.org
5.6.3、ORTP视频传输测试
(1)修改LOCAL_HOST_IP 为192.168.0.100（和我的Widnows的WIFI自动分配的IP一样）
(2)编译得到sample并到开发板运行
(3)修改sdp中c=IN IP4 192.168.0.232,打开sdp播放
注：
AP：		192.168.0.1
PC：		192.168.0.100
开发板：	192.168.0.233
ubuntu：	192.168.0.244


5.7.纯WIFI无线调试环境的搭建
5.7.1、原理
(1)UBUNTU和Windows桥接，这样ubuntu、windows、开发板三者都做sta，连同一个AP
(2)开发板事先移植部署好USB WIFI驱动、wpa_supplicant等
(3)开发板/etc/profile中设计好配置WIFI启动、连接AP、静态IP等
(4)开发板/etc/profile中设置让开发板通过无线mount虚拟机
5.7.2、实战
(1)先确认开发板部署好USB WIFI驱动、wpa_supplicant工具
(2)sample放到虚拟机ubuntu的共享文件夹中
(3)开发板/etc/profile中做好各种配置
(4)设置ubuntu桥接到windows的无线网卡，并给ubuntu设置静态IP地址
(5)开发板重启，看效果

#ifconfig eth0 192.168.1.10
#mount -t nfs -o nolock 192.168.0.103:/home/aston/rootfs /mnt
cd /ko            
./load3518e -i -sensor ar0130 -osmem 32 -total 64
                                           
insmod /home/mt7601Usta.ko                       
ifconfig wlan0 up                          
ifconfig wlan0 192.168.0.233               
wpa_supplicant -Dwext -iwlan0 -c/etc/wpa_supplicant.conf &
route add default gw 192.168.0.1 dev wlan0                   
                                                             
mount -t nfs -o nolock 192.168.0.244:/home/aston/rootfs /mnt
cd /mnt    


// Makefile -- LINUX

// make xxx xxx xxx moudles