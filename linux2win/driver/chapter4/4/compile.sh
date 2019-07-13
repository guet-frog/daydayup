
cp /home/aston/linux2win/driver/chapter4/4/leds-s5pv210.c /home/aston/src/kernel/drivers/leds/ -f

cp /home/aston/kernel/kernel/arch/arm/mach-s5pv210/include/mach/leds-gpio.h /home/aston/src/kernel/arch/arm/mach-s5pv210/include/mach/ -f
cp /home/aston/kernel/kernel/arch/arm/mach-s5pv210/mach-x210.c /home/aston/src/kernel/arch/arm/mach-s5pv210/ -f

cd /home/aston/src/kernel

#make menuconfig

cd ~/src/kernel/

make
