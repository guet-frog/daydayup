
2.18.1.1��Ϊʲô��Ҫ���ļ�ϵͳ
	(1) init���̵�Ӧ�ó����ڸ��ļ�ϵͳ��
	(2) ���ļ�ϵͳ�ṩ�˸�Ŀ¼/
	(3) �ں��������Ӧ�ò�����(etcĿ¼)�ڸ��ļ�ϵͳ��	// ���а�=�ں�+rootfs
	(4) shell��������ڸ��ļ�ϵͳ�ϡ�Ʃ��ls��cd������
		�ܽ᣺һ��linux��ϵ��ֻ���ں˱�������ʹ�ã�
			����Ҫrootfs(����etcĿ¼�µ������ļ���/bin  /sbin��Ŀ¼�µ�shell�������/libĿ¼�µĿ��ļ���)����ϲ��ܹ���

// which ls
// file /bin/ls
// file /bin/ping		-- shell�������Ӧһ��Ӧ�ó���

2.18.1.2�����ļ�ϵͳ��ʵ����ʲô
	(1) ���ļ�ϵͳ��������;���ļ�ϵͳ
	(2) ���ļ�ϵͳҲ��������ĳ���ļ�ϵͳ��ʽ��rootfstype=

2.18.2.1�������ļ���ʽ	// rootfs_qt4.ext3
	(1) ʹ�ù��������Ŀɹ���¼�ľ����ļ��������а����˸��ļ�ϵͳ�е������ļ�
		��¼�˾��������ڶ���Ӧ������ʽ��
	(4) �����ļ�ϵͳ����һ���ĸ�ʽ����ʽ���ڻ��ģ����ļ�����׺�޹�
	
		// ��¼���ں���������

2.18.2.2���ļ�����ʽ		// �����ڸ�ʽ����ʽ�����������񹤾�ȷ��
	(1) �����ض����ݵ��ļ���

		// nfs��ʽ����

2.18.3.1��mke2fs����
	(1) mke2fs��һ��Ӧ�ó�����ubuntu��Ĭ���ǰ�װ�˵ġ����Ӧ�ó����������
		����ext2��ext3��ext4�ȸ�ʽ�ĸ��ļ�ϵͳ��
	(2) һ�������������ֲ�ͬ��ʽ��rootfs��Ӧ�ó�������ֶ������ƣ�
		������mkfs.xxx��Ʃ����������ext2��ʽ��rootfs�Ĺ��߽�mkfs.ext2������
		����jffs2��ʽ��rootfs�Ĺ��߾ͽ�mkfs.jffs2��
	(3) ubuntu14.04 �е�mkfs.ext2�ȶ���mke2fs�ķ������Ӷ��ѡ�

2.18.3.2����������ext3��ʽ�ĸ��ļ�ϵͳ
	(1) ����rootfs.ext2�ļ������ص�ĳһ��Ŀ¼�²��Է���
		// http://blog.csdn.net/zhengmeifu/article/details/24174513
		dd if=/dev/zero of=rootfs.ext2 bs=1024 count=20480
		losetup  /dev/loop1 rootfs.ext2
		mke2fs -m 0 /dev/loop1 20480
		mount -t ext2 /dev/loop1 ./rootfs/

		umount /dev/loop1		// ж��
		losetup -d /dev/loop1

2.18.4.2�������ں���֧��nfs��Ϊrootfs
	(1) bootloader������nfs������ʽ��bootargs

	(2) ��menuconfig������֧��nfs������ʽ(�ο���Ƶ�ļ����µ��ĵ�)
		// setenv bootargs root=/dev/nfs nfsroot=192.168.1.141:/root/porting_x210/rootfs ip=192.168.1.10:192.168.1.141:192.168.1.1:255.255.255.0::eth0:off  init=/linuxrc console=ttySAC2,115200

		���ز���nfs�����Ƿ���
		// sudo mount -t nfs -o nolock 192.168.0.88:/home/aston/src/rootfs/rootfs /opt
		// umount /opt

2.18.5.����ʲô��linuxrc
		/linuxrc��һ����ִ�е�Ӧ�ó�����ں��޹أ���arm-linux-gcc����õ�������
		һ���ɾ�̬���ӵõ�

2.18.5.3��/linuxrc����ϵͳ�����������		// etc��execute time config
	(1) �ͺ���һ�����ӽ���֮����ֱ��ס����Ҫװ��һ��������ϵͳ����������Ҳ����ֱ���ã�Ҫ�����¡�
	(2) ����ϵͳ�������Ӧ�ò�����ã�һ�������ʱ���ã�Ӣ�ļ�дetc��
		��Ϊ�������ǵĲ���ϵͳ�����������㣬���ʺ��Ҹ��˵İ��û���ʵ���ԡ�

2.18.5.4��/linuxrc��Ƕ��ʽlinux��һ�����busybox
	(1) busybox��һ��C����д��������Ŀ����������˺ܶ�.c�ļ���.h�ļ���
		�����Ŀ���Ա����ñ���ɸ���ƽ̨����������е�Ӧ�ó���
		���������arm-linux-gcc������busybox�ͻ�õ�һ������������
		������linux�ں������е�Ӧ�ó���
	(2) busybox������򿪷���������Ϊ����Ƕ��ʽ�����¹���rootfsʹ�õģ�
		Ҳ����˵������ר�ſ�����init����Ӧ�ó���
	(3) busyboxΪ��ǰϵͳ�ṩ��һ���׵�shell������򼯡�Ʃ��vi��cd��mkdir��ls�ȡ�
		��������linux���а棨Ʃ��ubuntu��redhat��centOS�ȣ���vi��cd��ls�ȶ���
		һ��һ���ĵ�����Ӧ�ó���
		������Ƕ��ʽlinux�У�Ϊ��ʡ�����ǰ�vi��cd�����г��õ�shell����ϵ�һ��
		������һ��shell�������������busybox��

2.18.6.rootfs�л�Ӧ����ʲô					// /etc ����ʱ�����ļ�
	(1) /linuxrc
	(2)	/dev/		// �в����豸�ļ���rootfs�Ǳ����
	(3) /sys /proc
	(4) /usr
	(5) /etc
	(6) /lib		// ��̬�;�̬���ӿ�

2.18.7.VFS���
2.18.7.1��ʲô��VFS
	(1) VFS��linux�ں˵�һ����������ƻ��ơ�VFS����vitrual file system���������ļ�ϵͳ��
	(2) �����һЩ�ļ�ϵͳ��FAT��NTFS��ext2��ext3��jffs2��yaffs2��ubi��
		��Ҫ���Ŀ����Ϊ�˹������豸��Ӳ�̡�Nand��������
	(3) VFS�ǽ�����ļ�ϵͳ��������ͨ���ļ�ϵͳ���ײ����Թ�����
		������������ʽ���ʣ�ת����Ŀ¼+�ļ����ķ�ʽ�����ʣ�
		��Ӳ���豸�ķ���Ҳ���⻯���˶�Ŀ¼+�ļ��ķ��ʡ�
		��������VFS�����ǿ���ͨ���豸�ļ���Ŀ¼+�ļ�����Ʃ��/dev/mmcblk0p2��
		�ķ�ʽ������ϵͳ�е�Ӳ���豸��

2.18.7.2��VFS������
	(1) ����˵���Ķ�Ӳ���豸�ķ��ʣ�����Ӳ���豸�ķ��ʺͶ���ͨ�ļ��ķ���
		���ӿ�ͳһ���ˣ�linux��һ�н����ļ�����
	(2) ������ϵͳ�ϲ㣨Ӧ�ò㣩���²㲻ͬ�ļ�ϵͳ���͵ķ���ϸ�ڸ����ε��ˡ�
		������û��VFS������дcp�����������Ҳһ������ʱ��Ͳ��ò�ȥ
		������cp������ļ���ʲô�ļ�ϵͳ�����¡�
		����cp����ǳ����ӣ����Ҫ���Ǿ�����ļ�ϵͳ���͡�����VFS������Ͳ�ͬ�ˡ�
		VFS����һ������㣬�������²�Ĳ�ͬ�ļ�ϵͳ�Ĳ����ԣ����ϲ�Ӧ���ṩһ��ͳһ�Ľӿڡ�
	(3) VFS����ͬ�ļ�ϵͳ���²�Ӳ���豸�����豸������֮���ϸ��Ҳ�������ˡ�
		��ͬ���͵��ļ�ϵͳ�ڱ������ʱ�ǲ��ÿ��Ǹ��ֲ�ͬ��Ӳ���豸�ľ����������ģ�
		������һ��������VFS��������

2.18.7.3��VFS������ѧϰ�Ĺ�ϵ
	(1) VFS���ƺ�rootfs���أ��������ļ�ϵͳ�Ĺ��ض����й�����
	(2) �ں�����һЩsys proc���������ļ�ϵͳ���ⶫ��Ҳ�Ǻ�VFS�����й�
	(3) /dev/Ŀ¼�µ��豸�ļ�����VFS�йأ�����ѧϰ�����Ʋ���VFS

// ���ú�����nfs����
sudo vi /etc/exports
sudo /etc/init.d/nfs-kernel-server restart

// bootargs
setenv bootargs root=/dev/nfs nfsroot=192.168.0.88:/home/aston/src/rootfs/rootfs ip=192.168.0.66:192.168.0.88:192.168.0.1:255.255.255.0::eth0:off init=/linuxrc console=ttySAC2,115200 
setenv bootargs root=/dev/nfs nfsroot=192.168.0.88:/root/rootfs ip=192.168.0.66:192.168.0.88:192.168.0.1:255.255.255.0::eth0:off init=/linuxrc console=ttySAC2,115200 
setenv bootargs root=/dev/nfs nfsroot=192.168.1.141:/root/porting_x210/rootfs ip=192.168.1.10:192.168.1.141:192.168.1.1:255.255.255.0::eth0:off  init=/linuxrc console=ttySAC2,115200

// showmount localhost -e

setenv bootargs
root=/dev/nfs
nfsroot=192.168.1.141:/root/porting_x210/rootfs ip=192.168.1.10:192.168.1.141:192.168.1.1:255.255.255.0::eth0:off
init=/linuxrc console=ttySAC2,115200

set bootargs console=ttySAC2,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext2

