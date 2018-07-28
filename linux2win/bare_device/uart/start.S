
#define SVC_STACK	0xD0037D80

#define GPJ0CON		0xE0200240
#define GPJ0DAT		0xE0200244

#define SOFT_SWITCH	0xE010E81C

.global _start

_start:
	
	// 第2步：初始化时钟
	// bl clock_init
	
	// 第3步：设置SVC栈
	ldr sp, =SVC_STACK
	
	ldr r1, =SOFT_SWITCH
	ldr r0, [r1]
	//orr r0, r0, =((1<<9)|(1<<8)|(1<<0))
	//orr r0, r0, =0x301
	ldr r0, =0x301
	str r0, [r1]
	
	ldr r0, =0x11111111
	ldr r1, =GPJ0CON		// warning: =
	str r0, [r1]
	ldr r0, =(~(0x1<<3))
	ldr r1, =GPJ0DAT
	str r0, [r1]
	bl delay_asm
	
	bl main

	// b .
	
	
delay_asm:
	ldr r2, =9000000
	ldr r3, =0x0
delay_loop:	
	sub r2, r2, #1		// r2 = r2 - 1
	cmp r2, r3			// cmp修改z标志位, 如果r2等于r3则z=1, 下一句中eq就会成立
	bne delay_loop		// b no equal
	mov pc, lr
	
	
