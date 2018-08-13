led.bin: start.o led.o
	arm-linux-ld -Ttext 0xD0020010 -o led.elf $^
	arm-linux-objcopy -O binary led.elf $@
	arm-linux-objdump -D led.elf > led.dis
	
led.o : main.c
	arm-linux-gcc -c $< -o $@
start.o : start.S
	arm-linux-gcc -c $< -o $@
	
clean:
	rm *.o *.elf *.bin *.dis


