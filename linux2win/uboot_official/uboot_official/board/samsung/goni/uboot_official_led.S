
#define GPJ0CON	0xE0200240
#define GPJ0DAT	0xE0200244
#define GPD0CON	0xE02000A0
#define GPD0DAT 0xE02000A4

.global uboot_official_delay
.global led_init
.global open_left_led
.global open_medium_led
.global open_right_led
.global close_left_led
.global close_medium_led
.global close_right_led
.global open_all_led
.global close_all_led


led_init:
	ldr r0, =0x11111111
	ldr r1, =GPJ0CON
	str r0, [r1]
	mov pc, lr
	
open_left_led:
	ldr r0, =~(0x1<<3)
	ldr r1, =GPJ0DAT
	str r0, [r1]
	mov pc, lr
	
open_medium_led:
	ldr r0, =~(0x1<<4)
	ldr r1, =GPJ0DAT
	str r0, [r1]
	mov pc, lr
	
open_right_led:
	ldr r0, =~(0x1<<5)
	ldr r1, =GPJ0DAT
	str r0, [r1]
	mov pc, lr
	
open_all_led:
	ldr r0, =~(0x7<<3)
	ldr r1, =GPJ0DAT
	str r0, [r1]
	mov pc, lr
	
close_left_led:
	ldr r0, =(0x1<<3)
	ldr r1, =GPJ0DAT
	str r0, [r1]
	mov pc, lr
	
close_medium_led:
	ldr r0, =(0x1<<4)
	ldr r1, =GPJ0DAT
	str r0, [r1]
	mov pc, lr
	
close_right_led:
	ldr r0, =(0x1<<5)
	ldr r1, =GPJ0DAT
	str r0, [r1]
	mov pc, lr
	
close_all_led:
	ldr r0, =(0x7<<3)
	ldr r1, =GPJ0DAT
	str r0, [r1]
	mov pc, lr


uboot_official_delay:
	ldr r2, =50000000
	ldr r3, =0x0
delay_loop:
	sub r2, r2, #1
	cmp r2, r3
	bne delay_loop
	mov pc, lr



	
// D22 GPJ0_3 低电平点亮 -> 左
// D23 GPJ0_4 低电平点亮
// D24 GPJ0_5 低电平点亮

// D25 GPD0_1 低电平点亮(PWMTOUT1)