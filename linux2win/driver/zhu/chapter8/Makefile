
KERN_DIR = ~/src/kernel
CC = /usr/local/arm/arm-2009q3/bin/arm-linux-gcc

obj-m	+= input_button.o

all:
	make -C $(KERN_DIR) M=`pwd` modules

	gcc app_input_pc.c -o app_input_pc
	$(CC) app_input_board.c -o app_input_board
	
cp:
	cp app_input_board ~/src/rootfs/rootfs/app_test
	
	cp *.ko ~/src/rootfs/rootfs/driver_test

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -f app_input_pc app_input_board *.ko
	

