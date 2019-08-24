
CC = /usr/local/arm/arm-2009q3/bin/arm-linux-gcc

all:
	$(CC) lcd_app.c -o lcd_app
	$(CC) appfb.c -o appfb
	
cp:
	cp lcd_app appfb ~/src/rootfs/rootfs/app_test

clean:
	rm -f lcd_app appfb
	

