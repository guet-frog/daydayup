#!/bin/sh

	cp uboot/include/configs/x210_sd.h /home/aston/src/uboot/include/configs/


	
	cd /home/aston/src/uboot/
	make distclean
	make x210_sd_config
	make -j4
