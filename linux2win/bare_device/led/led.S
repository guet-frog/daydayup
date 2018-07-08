
#define GPJ0CON	0xE0200240
#define GPJ0DAT	0xE0200244

.global _start     //��_start�������Ը�Ϊ�ⲿ�����������ļ��Ϳ��Կ���_start��
                   //arm-linux-ld: warning: cannot find entry symbol _start
				   //defaulting to 00000000
_start:
	ldr r0, =0x11111111			// '='ldrαָ��
	ldr r1, =GPJ0CON
	str r0, [r1]
	
flash:
	ldr r0, =~(1<<3)
	ldr r1, =GPJ0DAT
	str r0, [r1]
	
	bl delay
	
	b flash		// beq flash ... ��һ����������ת?
//  b .
	
delay:
	ldr r2, =9000000
	ldr r3, =0x0
delay_loop:	
	sub r2, r2, #1		// r2 = r2 - 1
	cmp r2, r3			// cmp�޸�z��־λ, ���r2����r3��z=1, ��һ����eq�ͻ����
	bne delay_loop		// b no equal
	mov pc, lr
	
	// push {lr}
	// pop  {pc}





