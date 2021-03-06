#!/bin/sh

#CLEAN=""	# define shell var

echo "shell param num is $#"	# warning: $#, real param num (not include file name)

if [ $# -eq 1 ]; then		# warnign: [] need have a space
	CLEAN="distclean"
else
	echo "default make distclean then make"
fi

############################################################################

#board.c -- 删除一些宏定义代码; 添加打印信息
#cp -f uboot_samsung/lib_arm/board.c /home/aston/src/uboot_samsung/lib_arm/

# env_auto.c -- env_relocate_spec_movinand() -- for default_environment[]
#cp -f uboot_samsung/common/env_auto.c /home/aston/src/uboot_samsung/common/env_auto.c

# test default env in which file -- default_environment -> default_environment_test
# test gd->env_valid == 0 or 1
#cp -f uboot_samsung/common/env_common.c /home/aston/src/uboot_samsung/common/env_common.c

# test main_loop()
#cp -f uboot_samsung/common/main.c /home/aston/src/uboot_samsung/common/main.c

# command.c格式修改
# command.c	-- find_cmd()
#cp -f uboot_samsung/common/command.c /home/aston/src/uboot_samsung/common/command.c

# cmd_bootm.c -- for do_bootm() image addr
#cp -f uboot_samsung/common/cmd_bootm.c /home/aston/src/uboot_samsung/common/cmd_bootm.c

# bootm.c -- for do_bootm_linux() kernel entry pointe
#cp -f uboot_samsung/lib_arm/bootm.c /home/aston/src/uboot_samsung/lib_arm/bootm.c


############################################################################
# modify config
cp -f uboot_samsung/include/configs/smdkv210single.h /home/aston/src/uboot_samsung/include/configs/smdkv210single.h

# mmc.c -- for mmc init
cp -f uboot_samsung/drivers/mmc/mmc.c /home/aston/src/uboot_samsung/drivers/mmc/mmc.c
cp -f uboot_samsung/drivers/mmc/s3c_hsmmc.c /home/aston/src/uboot_samsung/drivers/mmc/s3c_hsmmc.c
cp -f uboot_samsung/cpu/s5pc11x/setup_hsmmc.c /home/aston/src/uboot_samsung/cpu/s5pc11x/setup_hsmmc.c

# lowlevel_init.S -- MMU remap	line596
cp -f uboot_samsung/board/samsung/smdkc110/lowlevel_init.S /home/aston/src/uboot_samsung/board/samsung/smdkc110/lowlevel_init.S

# smdkc110.c -- virt_to_phy_smdkc110(ulong addr) line186
cp -f uboot_samsung/board/samsung/smdkc110/smdkc110.c /home/aston/src/uboot_samsung/board/samsung/smdkc110/smdkc110.c

#cpu_init.S -- ddr init	line122
cp -f uboot_samsung/cpu/s5pc11x/s5pc110/cpu_init.S	/home/aston/src/uboot_samsung/cpu/s5pc11x/s5pc110/cpu_init.S
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
