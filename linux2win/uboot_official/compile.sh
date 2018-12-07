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
cp -f uboot_official/board/samsung/goni/uboot_official_led_c.c /home/aston/src/uboot_official/board/samsung/goni/

# board/samsung/goni/Makefile修改
cp -f uboot_official/board/samsung/goni/Makefile /home/aston/src/uboot_official/board/samsung/goni/Makefile

# arch/arm/cpu/u-boot.lds修改
cp -f uboot_official/arch/arm/cpu/u-boot.lds /home/aston/src/uboot_official/arch/arm/cpu/u-boot.lds

# led添加到lowlevel_init.S
cp -f uboot_official/board/samsung/goni/lowlevel_init.S /home/aston/src/uboot_official/board/samsung/goni/

# 添加s5p_goni.h相关宏定义	-- 注意及时更新文件
cp -f uboot_official/include/configs/s5p_goni.h /home/aston/src/uboot_official/include/configs/
cp -f uboot_official/include/s5pc110.h /home/aston/src/uboot_official/include/

# 添加ddr_init.S
cp -f uboot_official/board/samsung/goni/ddr_init.S /home/aston/src/uboot_official/board/samsung/goni/

# 添加relocate_sd2sdram.c
cp -f uboot_official/board/samsung/goni/relocate_sd2sdram.c /home/aston/src/uboot_official/board/samsung/goni/



############################################################################ enter uboot step 2

# 修改crt0.S
cp -f uboot_official/arch/arm/lib/crt0.S /home/aston/src/uboot_official/arch/arm/lib/

# 修改board.c
cp -f uboot_official/arch/arm/lib/board.c /home/aston/src/uboot_official/arch/arm/lib/

# 修改goni.c	-- modify dram_init_banksize(), arch_num
cp -f uboot_official/board/samsung/goni/goni.c /home/aston/src/uboot_official/board/samsung/goni/

############################################################################


############################################################################

# 删除原mmc.c
#rm -f /home/aston/src/uboot_official/drivers/mmc/mmc.c
#rm -f /home/aston/src/uboot_official/drivers/mmc/s5p_sdhci.c
#rm -f /home/aston/src/uboot_official/drivers/mmc/sdhci.c

# 覆盖原mmc.c, mmc.h
cp -f uboot_official/drivers/mmc/mmc.c /home/aston/src/uboot_official/drivers/mmc/
cp -f uboot_official/include/mmc.h /home/aston/src/uboot_official/include/

# 修改cmd_mmc.c相关 -- mmc read write相关操作
cp -f uboot_official/common/cmd_mmc.c /home/aston/src/uboot_official/common/

# s5p_goni.h

############################################################################


cd /home/aston/src/uboot_official/

make distclean			# make clean error

make s5p_goni_config

make -j4