led_on.bin: led_on.o 
	arm-linux-ld -Ttext 0xD0020010 -o led_on.elf $^
	arm-linux-objcopy -O binary led_on.elf $@
	arm-linux-objdump -D led_on.elf > led_on.dis
	
led_on.o : led_on.S
	arm-linux-gcc -c $< -o $@

clean:
	rm *.o *.elf *.bin *.dis
