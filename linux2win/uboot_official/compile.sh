#!/bin/sh

#CLEAN=""	# define shell var

echo "shell param num is $#"	# warning: $#, real param num (not include file name)

if [ $# -eq 1 ]; then		# warnign: [] need have a space
	CLEAN="distclean"
else
	echo "default make distclean then make"
fi

############################################################################

# Makefile修改
cp -f uboot_official/Makefile /home/aston/src/uboot_official/Makefile

# start.S修改
cp -f uboot_official/arch/arm/cpu/armv7/start.S /home/aston/src/uboot_official/arch/arm/cpu/armv7/start.S

# uboot_official_led.S添加
cp -f uboot_official/board/samsung/goni/uboot_official_led.S /home/aston/src/uboot_official/board/samsung/goni/

# board/samsung/goni/Makefile修改
cp -f uboot_official/board/samsung/goni/Makefile /home/aston/src/uboot_official/board/samsung/goni/Makefile

# arch/arm/cpu/u-boot.lds修改
cp -f uboot_official/arch/arm/cpu/u-boot.lds /home/aston/src/uboot_official/arch/arm/cpu/u-boot.lds

# led添加到lowlevel_init.S
cp -f uboot_official/board/samsung/goni/lowlevel_init.S /home/aston/src/uboot_official/board/samsung/goni/lowlevel_init.S

############################################################################


cd /home/aston/src/uboot_official/

make distclean			# make clean error

make s5p_goni_config

make -j4