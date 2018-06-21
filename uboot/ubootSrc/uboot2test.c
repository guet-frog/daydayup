
// start.S line317
	ldr r2, =0xEB000000
	cmp r1, r2
	beq	mmcsd_boot			// add led for test, bl1 boot in inand can get here
							// inand应该能兼容SD卡