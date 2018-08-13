
#define GPJ0CON	0xE0200240
#define GPJ0DAT	0xE0200244

.global _start     //把_start链接属性改为外部，这样其他文件就可以看见_start了
                   //arm-linux-ld: warning: cannot find entry symbol _start
				   //defaulting to 00000000
_start:
	ldr r0, =0x11111111			// '='ldr伪指令
	ldr r1, =GPJ0CON
	str r0, [r1]
	
flash:
	ldr r0, =~(0x7<<3)
	ldr r1, =GPJ0DAT
	str r0, [r1]
	
	bl delay
	
	ldr r0, =(0x7<<3)
	ldr r1, =GPJ0DAT
	str r0, [r1]
	
	bl delay
	
	b flash		// beq flash ... 在一个函数内跳转?  -- PC跳转后继续执行
//  b .
	
delay:
	ldr r2, =9000000
	ldr r3, =0x0
delay_loop:	
	sub r2, r2, #1		// r2 = r2 - 1
	cmp r2, r3			// cmp修改z标志位, 如果r2等于r3则z=1, 下一句中eq就会成立
	bne delay_loop		// b no equal
	mov pc, lr
	
	// push {lr}
	// pop  {pc}






