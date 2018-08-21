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

# env_init() in which file -- not in below files
# env_movi.c line81
# env_nowhere line57
# env_auto.c line147; line549
cp -f uboot/common/env_movi.c /home/aston/src/uboot/common/env_movi.c
cp -f uboot/common/env_nowhere.c /home/aston/src/uboot/common/env_nowhere.c
cp -f uboot/common/env_auto.c /home/aston/src/uboot/common/env_auto.c

# test default env in which file -- default_environment -> default_environment_test
# test ENV_IS_EMBEDDED defined or not
# test gd->env_valid == 0 or 1
cp -f uboot/common/env_common.c /home/aston/src/uboot/common/env_common.c

# test main_loop()
cp -f uboot/common/main.c /home/aston/src/uboot/common/main.c

# command.c格式修改
cp -f uboot/common/command.c /home/aston/src/uboot/common/command.c

# cmd_bootm.c -- for do_bootm()
cp -f uboot/common/cmd_bootm.c /home/aston/src/uboot/common/cmd_bootm.c
############################################################################


cd /home/aston/src/uboot/

#CLEAN = "distclean"	warning: shell script need consider [space] carefually
CLEAN="distclean"

make ${CLEAN}
# if [${CLEAN} = ""];then	#error
# if [${CLEAN}=""];then		#error
if [ ${CLEAN}="" ]; then	# warning: = not ==
	make x210_sd_config
	make -j4
fi
