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

# 添加s5p_goni.h相关宏定义	-- 注意及时更新文件
cp -f uboot_official/include/configs/s5p_goni.h /home/aston/src/uboot_official/include/configs/
cp -f uboot_official/include/s5pc110.h /home/aston/src/uboot_official/include/s5pc110.h

# 添加ddr_init.S
cp -f uboot_official/board/samsung/goni/ddr_init.S /home/aston/src/uboot_official/board/samsung/goni/ddr_init.S

# 添加relocate_sd2sdram.c
cp -f uboot_official/board/samsung/goni/relocate_sd2sdram.c /home/aston/src/uboot_official/board/samsung/goni/relocate_sd2sdram.c


############################################################################


cd /home/aston/src/uboot_official/

make distclean			# make clean error

make s5p_goni_config

make -j4