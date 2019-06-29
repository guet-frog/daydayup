#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <mach/gpio-bank.h>
#include <mach/regs-gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
//#include <linux/ioport.h>
#include <linux/io.h>

#define LED_MAJOR		100
#define AUTO_MAJOR		0
#define BYTE_CNT		100
#define LED_MAX_DEVS	32768

/* ------------------led----------------------*/
#define GPJ0CON_VALUE	((0x1<<12)|(0x1<<16)|(0x1<<20))

/*----------------static io map----------------
// #define GPJ0CON	0xE0200240
// #define GPJ0DAT	0xE0200244
#define GPJ0CON		S5PV210_GPJ0CON
#define GPJ0DAT		S5PV210_GPJ0DAT

#define rGPJ0CON	(*((volatile uint32_t *)GPJ0CON))
#define rGPJ0DAT	(*((volatile uint32_t *)GPJ0DAT))
*/

/* --------------dynamic io map----------------*/
#define GPJ0CON	0xE0200240
typedef struct abcd
{
	unsigned int gpj0_con;
	unsigned int gpj0_data;
} GPJ0_t;

static struct platform_driver s5pv210_led_driver = 
{
	.
}

static int __init led_dev_init(void)
{
	return platform_driver_register(&);
}

static void __exit led_dev_exit(void)
{
	platform_driver_unregister(&);
}



module_init(led_dev_init);
module_exit(led_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("frog");
MODULE_DESCRIPTION("module test");
MODULE_ALIAS("alias xxx");
