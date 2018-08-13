CC			= arm-linux-gcc
LD			= arm-linux-ld
OBJCOPY		= arm-linux-objcopy
OBJDUMP		= arm-linux-objdump
AR			= arm-linux-ar

INCDIR		:= $(shell pwd)/include

CFLAGS		:= -Wall -O2 -fno-builtin
CPPFLAGS 	:= -nostdlib -nostdinc -I$(INCDIR)

objs		:= main.o start.o led.o uart.o
objs 		+= ./lib/printf_lib.a

export CC AR CFLAGS CPPFLAGS

bl2.bin: $(objs)
	#$(LD) -Ttext 0x0 -o uart.elf $^
	$(LD) -Tlink.lds -o bl2.elf $^
	$(OBJCOPY) -O binary bl2.elf bl2.bin
	$(OBJDUMP) -D bl2.elf > bl2_elf.dis

%.o : %.S
	$(CC) -o $@ $< -c $(CFLAGS) $(CPPFLAGS)

%.o : %.c
	$(CC) -o $@ $< -c $(CFLAGS) $(CPPFLAGS)

lib/printf_lib.a:
	make -C ./lib

clean:
	make clean -C ./lib
	rm *.o *.elf *.bin *.dis mkx210 -f

	
	
