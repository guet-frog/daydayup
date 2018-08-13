CC			= arm-linux-gcc
LD			= arm-linux-ld
OBJCOPY		= arm-linux-objcopy
OBJDUMP		= arm-linux-objdump
AR			= arm-linux-ar

INCDIR		:= $(shell pwd)/include

CFLAGS		:= -Wall -O2 -fno-builtin
CPPFLAGS 	:= -nostdlib -nostdinc -I$(INCDIR)

objs		:= main.o start.o led.o uart.o speed.o clock.o copy_sd2sram.o
objs 		+= ./libgcc.a ./lib/printf_lib.a

export CC AR CFLAGS CPPFLAGS

bl1.bin: $(objs)
	#$(LD) -Ttext 0x0 -o uart.elf $^
	$(LD) -Tlink.lds -o bl1.elf $^
	$(OBJCOPY) -O binary bl1.elf bl1.bin
	$(OBJDUMP) -D bl1.elf > bl1_elf.dis
	gcc mkv210_image.c -o mkx210
	./mkx210 bl1.bin 210_bl1.bin

%.o : %.S
	$(CC) -o $@ $< -c $(CFLAGS) $(CPPFLAGS)

%.o : %.c
	$(CC) -o $@ $< -c $(CFLAGS) $(CPPFLAGS)

lib/printf_lib.a:
	make -C ./lib

clean:
	make clean -C ./lib
	rm *.o *.elf *.bin *.dis mkx210 -f

	
	
