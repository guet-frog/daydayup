#!/bin/sh

#CLEAN=""	# define shell var

echo "shell param num is $#"	# warning: $#, real param num (not include file name)

if [ $# -eq 1 ]; then		# warnign: [] need have a space
	CLEAN="distclean"
else
	echo "default make distclean then make"
fi

############################################################################
cp -f uboot/include/configs/x210_sd.h /home/aston/src/uboot/include/configs/

#board.c -- start_armboot.c不相关宏删除
cp -f uboot/lib_arm/board.c /home/aston/src/uboot/lib_arm/

# env_init() in which file
cp -f uboot/common/env_movi.c /home/aston/src/uboot/common/env_movi.c
cp -f uboot/common/env_nowhere.c /home/aston/src/uboot/common/env_nowhere.c
############################################################################


cd /home/aston/src/uboot/

#CLEAN = "dist clean"	warning: shell script need consider [space] carefually
CLEAN="distclean"

make ${CLEAN}
# if [${CLEAN} = ""];then	#error
# if [${CLEAN}=""];then		#error
if [ ${CLEAN}="" ]; then	# warning: = not ==
	make x210_sd_config
	make -j4
fi
