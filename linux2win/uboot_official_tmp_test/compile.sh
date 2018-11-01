#!/bin/sh

############################################################################
# no modify
#cp -f uboot/include/configs/x210_sd.h /home/aston/src/uboot/include/configs/

cp -f uboot_official_led.S /home/aston/src/uboot/cpu/s5pc11x/
cp -f start.S /home/aston/src/uboot/cpu/s5pc11x/
cp -f Makefile /home/aston/src/uboot/cpu/s5pc11x/

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
