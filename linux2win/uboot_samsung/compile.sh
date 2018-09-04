#!/bin/sh

#CLEAN=""	# define shell var

echo "shell param num is $#"	# warning: $#, real param num (not include file name)

if [ $# -eq 1 ]; then		# warnign: [] need have a space
	CLEAN="distclean"
else
	echo "default make distclean then make"
fi

############################################################################
# no modify
cp -f uboot_samsung/include/configs/x210_sd.h /home/aston/src/uboot_samsung/include/configs/

#board.c -- 删除一些宏定义代码; 添加打印信息
cp -f uboot_samsung/lib_arm/board.c /home/aston/src/uboot_samsung/lib_arm/

# env_auto.c -- env_relocate_spec_movinand() -- for default_environment[]
cp -f uboot_samsung/common/env_auto.c /home/aston/src/uboot_samsung/common/env_auto.c

# test default env in which file -- default_environment -> default_environment_test
# test gd->env_valid == 0 or 1
cp -f uboot_samsung/common/env_common.c /home/aston/src/uboot_samsung/common/env_common.c

# test main_loop()
cp -f uboot_samsung/common/main.c /home/aston/src/uboot_samsung/common/main.c

# command.c格式修改
# command.c	-- find_cmd()
cp -f uboot_samsung/common/command.c /home/aston/src/uboot_samsung/common/command.c

# cmd_bootm.c -- for do_bootm() image addr
cp -f uboot_samsung/common/cmd_bootm.c /home/aston/src/uboot_samsung/common/cmd_bootm.c

# bootm.c -- for do_bootm_linux() kernel entry pointe
cp -f uboot_samsung/lib_arm/bootm.c /home/aston/src/uboot_samsung/lib_arm/bootm.c

# mmc.c -- for test mmc init(SD card init?)
# find_mmc_device() -- for test init mmc channel2
# movi_write() -- mmc_bwrite(0, start, blkcnt, src) -- for test saveenv in channel2(SD)
# movi_read() -- for test env relocate
cp -f uboot_samsung/drivers/mmc/mmc.c /home/aston/src/uboot_samsung/drivers/mmc/mmc.c

# setup_hsmmc.c -- seup_hsmmc_clock()
cp -f uboot_samsung/cpu/s5pc11x/setup_hsmmc.c /home/aston/src/uboot_samsung/cpu/s5pc11x/setup_hsmmc.c

# lowlevel_init.S
cp -f uboot_samsung/board/samsung/smdkc110/lowlevel_init.S /home/aston/src/uboot_samsung/board/samsung/smdkc110/lowlevel_init.S
############################################################################


cd /home/aston/src/uboot_samsung/

#CLEAN = "distclean"	warning: shell script need consider [space] carefually
CLEAN="distclean"

make ${CLEAN}
# if [${CLEAN} = ""];then	#error
# if [${CLEAN}=""];then		#error
if [ ${CLEAN}="" ]; then	# warning: = not ==
	make smdkv210single_config
	make -j4
fi
