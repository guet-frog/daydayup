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
cp -f uboot/include/configs/x210_sd.h /home/aston/src/uboot/include/configs/

#board.c -- 删除一些宏定义代码; 添加打印信息
cp -f uboot/lib_arm/board.c /home/aston/src/uboot/lib_arm/

# env_auto.c -- env_relocate_spec_movinand() -- for default_environment[]
cp -f uboot/common/env_auto.c /home/aston/src/uboot/common/env_auto.c

# test default env in which file -- default_environment -> default_environment_test
# test gd->env_valid == 0 or 1
cp -f uboot/common/env_common.c /home/aston/src/uboot/common/env_common.c

# test main_loop()
cp -f uboot/common/main.c /home/aston/src/uboot/common/main.c

# command.c格式修改
# command.c	-- find_cmd()
cp -f uboot/common/command.c /home/aston/src/uboot/common/command.c

# cmd_bootm.c -- for do_bootm() image addr
cp -f uboot/common/cmd_bootm.c /home/aston/src/uboot/common/cmd_bootm.c

# bootm.c -- for do_bootm_linux() kernel entry pointe
cp -f uboot/lib_arm/bootm.c /home/aston/src/uboot/lib_arm/bootm.c

# mmc.c -- for test mmc init(SD card init?)
# find_mmc_device() -- for test init mmc channel2
# movi_write() -- mmc_bwrite(0, start, blkcnt, src) -- for test saveenv in channel2(SD)
# movi_read() -- for test env relocate
cp -f uboot/drivers/mmc/mmc.c /home/aston/src/uboot/drivers/mmc/mmc.c

# setup_hsmmc.c -- seup_hsmmc_clock()
cp -f uboot/cpu/s5pc11x/setup_hsmmc.c /home/aston/src/uboot/cpu/s5pc11x/setup_hsmmc.c
############################################################################


cd /home/aston/src/uboot/

#CLEAN = "distclean"	warning: shell script need consider [space] carefually
CLEAN="distclean"

make ${CLEAN}
# if [${CLEAN} = ""];then	#error
# if [${CLEAN}=""];then		#error
if [ ${CLEAN}="" ]; then	# warning: = not ==
	make distclean
	make x210_sd_config
	make -j4
fi
