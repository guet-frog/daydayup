OBJ=bl2
$(OBJ).bin: start.o uart.o lcd.o main.o
	arm-linux-ld -Ttext 0x20000000 -o $(OBJ).elf $^ -lgcc -L/opt/arm-cortex_a8-linux-gnueabi-4.7.3/lib/gcc/arm-cortex_a8-linux-gnueabi/4.7.3/
	arm-linux-objcopy -O binary $(OBJ).elf $@
	arm-linux-objdump -D $(OBJ).elf > $(OBJ).dis
	
%.o : %.c
	arm-linux-gcc -c $< -o $@ -fno-builtin
%.o : %.S
	arm-linux-gcc -c $< -o $@
	
clean:
	rm *.o *.elf *.bin *.dis
