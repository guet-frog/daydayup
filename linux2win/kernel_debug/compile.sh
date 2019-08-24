
cp plat-s5p/devs.c /home/aston/src/kernel/arch/arm/plat-s5p/ -f

cp mach-s5pv210/mach-x210.c mach-s5pv210/Makfile /home/aston/src/kernel/arch/arm/mach-s5pv210/ -f

cp touchscreen/gslX680.c touchscreen/gslX680.h touchscreen/gsl_point_id touchscreen/Makefile /home/aston/src/kernel/drivers/input/touchscreen/ -f



cd /home/aston/src/kernel

#make menuconfig

cd ~/src/kernel/

make -j4

cp arch/arm/boot/zImage /tftpboot/ -f

ls -l /tftpboot/zImage
