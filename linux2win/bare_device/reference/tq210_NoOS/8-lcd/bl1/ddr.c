#define MP1_0DRV (*(volatile unsigned long*)0xE02003CC)
#define MP1_1DRV (*(volatile unsigned long*)0xE02003EC)
#define MP1_2DRV (*(volatile unsigned long*)0xE020040C)
#define MP1_3DRV (*(volatile unsigned long*)0xE020042C)
#define MP1_4DRV (*(volatile unsigned long*)0xE020044C)
#define MP1_5DRV (*(volatile unsigned long*)0xE020046C)
#define MP1_6DRV (*(volatile unsigned long*)0xE020048C)
#define MP1_7DRV (*(volatile unsigned long*)0xE02004AC)
#define MP1_8DRV (*(volatile unsigned long*)0xE02004CC)

#define DMC0_BASE			0xF0000000
#define DMC1_BASE			0xF1400000

#define DMC0_CONCONTROL		*((volatile unsigned int *)(DMC0_BASE + 0x00))
#define DMC0_MEMCONTROL		*((volatile unsigned int *)(DMC0_BASE + 0x04))
#define DMC0_MEMCONFIG0		*((volatile unsigned int *)(DMC0_BASE + 0x08))
#define DMC0_MEMCONFIG1		*((volatile unsigned int *)(DMC0_BASE + 0x0C))
#define DMC0_DIRECTCMD		*((volatile unsigned int *)(DMC0_BASE + 0x10))
#define DMC0_PRECHCONFIG	*((volatile unsigned int *)(DMC0_BASE + 0x14))
#define DMC0_PHYCONTROL0 	*((volatile unsigned int *)(DMC0_BASE + 0x18))
#define DMC0_PHYCONTROL1 	*((volatile unsigned int *)(DMC0_BASE + 0x1C))
#define DMC0_PWRDNCONFIG 	*((volatile unsigned int *)(DMC0_BASE + 0x28))
#define DMC0_TIMINGAREF 	*((volatile unsigned int *)(DMC0_BASE + 0x30))
#define DMC0_TIMINGROW 		*((volatile unsigned int *)(DMC0_BASE + 0x34))
#define DMC0_TIMINGDATA 	*((volatile unsigned int *)(DMC0_BASE + 0x38))
#define DMC0_TIMINGPOWER 	*((volatile unsigned int *)(DMC0_BASE + 0x3C))
#define DMC0_PHYSTATUS 		*((volatile unsigned int *)(DMC0_BASE + 0x40))
#define DMC0_CHIP0STATUS 	*((volatile unsigned int *)(DMC0_BASE + 0x48))
#define DMC0_CHIP1STATUS 	*((volatile unsigned int *)(DMC0_BASE + 0x4C))
#define DMC0_AREFSTATUS 	*((volatile unsigned int *)(DMC0_BASE + 0x50))
#define DMC0_MRSTATUS 		*((volatile unsigned int *)(DMC0_BASE + 0x54))

#define DMC1_CONCONTROL		*((volatile unsigned int *)(DMC1_BASE + 0x00))
#define DMC1_MEMCONTROL		*((volatile unsigned int *)(DMC1_BASE + 0x04))
#define DMC1_MEMCONFIG0		*((volatile unsigned int *)(DMC1_BASE + 0x08))
#define DMC1_MEMCONFIG1		*((volatile unsigned int *)(DMC1_BASE + 0x0C))
#define DMC1_DIRECTCMD		*((volatile unsigned int *)(DMC1_BASE + 0x10))
#define DMC1_PRECHCONFIG	*((volatile unsigned int *)(DMC1_BASE + 0x14))
#define DMC1_PHYCONTROL0 	*((volatile unsigned int *)(DMC1_BASE + 0x18))
#define DMC1_PHYCONTROL1 	*((volatile unsigned int *)(DMC1_BASE + 0x1C))
#define DMC1_PWRDNCONFIG 	*((volatile unsigned int *)(DMC1_BASE + 0x28))
#define DMC1_TIMINGAREF 	*((volatile unsigned int *)(DMC1_BASE + 0x30))
#define DMC1_TIMINGROW 		*((volatile unsigned int *)(DMC1_BASE + 0x34))
#define DMC1_TIMINGDATA 	*((volatile unsigned int *)(DMC1_BASE + 0x38))
#define DMC1_TIMINGPOWER 	*((volatile unsigned int *)(DMC1_BASE + 0x3C))
#define DMC1_PHYSTATUS 		*((volatile unsigned int *)(DMC1_BASE + 0x40))
#define DMC1_CHIP0STATUS 	*((volatile unsigned int *)(DMC1_BASE + 0x48))
#define DMC1_CHIP1STATUS 	*((volatile unsigned int *)(DMC1_BASE + 0x4C))
#define DMC1_AREFSTATUS 	*((volatile unsigned int *)(DMC1_BASE + 0x50))
#define DMC1_MRSTATUS 		*((volatile unsigned int *)(DMC1_BASE + 0x54))


void ddr_init()
{
	/* DMC0 */
	DMC0_PHYCONTROL0 = 0x00101000;
	DMC0_PHYCONTROL0 = 0x00101002;			/* DLL on */
	DMC0_PHYCONTROL1 = 0x00000086;
	DMC0_PHYCONTROL0 = 0x00101003;			/* DLL start */

	while ((DMC0_PHYSTATUS & 0x7) != 0x7);	/* wait DLL locked */

	DMC0_CONCONTROL = 0x0FFF2350;			/* Auto Refresh Counter should be off */
	DMC0_MEMCONTROL = 0x00202430;			/* Dynamic power down should be off */
	DMC0_MEMCONFIG0 = 0x20E01323;

	DMC0_PRECHCONFIG = 0xFF000000;
	DMC0_PWRDNCONFIG = 0xFFFF00FF;

	DMC0_TIMINGAREF = 0x00000618;			/* 7.8us * 200MHz = 1560 = 0x618  */
	DMC0_TIMINGROW = 0x19233309;
	DMC0_TIMINGDATA = 0x23240204;
	DMC0_TIMINGPOWER = 0x09C80232;

	DMC0_DIRECTCMD = 0x07000000;			/* NOP */
	DMC0_DIRECTCMD = 0x01000000;			/* PALL */
	DMC0_DIRECTCMD = 0x00020000;			/* EMRS2 */
	DMC0_DIRECTCMD = 0x00030000;			/* EMRS3 */
	DMC0_DIRECTCMD = 0x00010400;			/* EMRS enable DLL*/
	DMC0_DIRECTCMD = 0x00000542;			/* DLL reset */
	DMC0_DIRECTCMD = 0x01000000; 			/* PALL */
	DMC0_DIRECTCMD = 0x05000000;			/* auto refresh */
	DMC0_DIRECTCMD = 0x05000000;			/* auto refresh */
	DMC0_DIRECTCMD = 0x00000442;			/* DLL unreset */
	DMC0_DIRECTCMD = 0x00010780;			/* OCD default */
	DMC0_DIRECTCMD = 0x00010400;			/* OCD exit */

	DMC0_CONCONTROL = 0x0FF02030;			/* auto refresh on */
	DMC0_PWRDNCONFIG = 0xFFFF00FF;
	DMC0_MEMCONTROL = 0x00202400;

	/* DMC1 */
	DMC1_PHYCONTROL0 = 0x00101000;
	DMC1_PHYCONTROL0 = 0x00101002;
	DMC1_PHYCONTROL1 = 0x86;
	DMC1_PHYCONTROL0 = 0x00101003;

	while((DMC0_PHYSTATUS&0x7) != 0x7);

	DMC1_CONCONTROL = 0x0FFF2350;			/* Auto Refresh Counter should be off */
	DMC1_MEMCONTROL = 0x00202430;			/* Dynamic power down should be off */
	DMC1_MEMCONFIG0 = 0x40E01323;

	DMC1_PRECHCONFIG = 0xFF000000;
	DMC1_PWRDNCONFIG = 0xFFFF00FF;

	DMC1_TIMINGAREF = 0x00000618;			/* 7.8us * 200MHz = 1560 = 0x618  */
	DMC1_TIMINGROW = 0x19233309;
	DMC1_TIMINGDATA = 0x23240204;
	DMC1_TIMINGPOWER = 0x09C80232;

	DMC1_DIRECTCMD = 0x07000000;
	DMC1_DIRECTCMD = 0x01000000;
	DMC1_DIRECTCMD = 0x00020000;
	DMC1_DIRECTCMD = 0x00030000;
	DMC1_DIRECTCMD = 0x00010400;
	DMC1_DIRECTCMD = 0x00000542;
	DMC1_DIRECTCMD = 0x01000000;
	DMC1_DIRECTCMD = 0x05000000;
	DMC1_DIRECTCMD = 0x05000000;
	DMC1_DIRECTCMD = 0x00000442;
	DMC1_DIRECTCMD = 0x00010780;
	DMC1_DIRECTCMD = 0x00010400;

	DMC1_CONCONTROL = 0x0FF02030;
	DMC1_PWRDNCONFIG = 0xFFFF00FF;
	DMC1_MEMCONTROL = 0x00202400;
}