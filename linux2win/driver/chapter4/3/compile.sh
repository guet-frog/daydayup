
cp /home/aston/linux2win/driver/chapter4/3/leds-s5pv210.c /home/aston/src/kernel/drivers/leds/ -r
cp /home/aston/kernel/kernel/drivers/leds/Kconfig /home/aston/src/kernel/drivers/leds/ -r
cp /home/aston/kernel/kernel/drivers/leds/Makefile /home/aston/src/kernel/drivers/leds/ -r

cd /home/aston/src/kernel

make menuconfig

