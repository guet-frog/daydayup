#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <mach/gpio-bank.h>
#include <mach/regs-gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/leds.h>
//#include <include/asm/gpio.h>
#include <linux/gpio.h>

#define LED_MAJOR		100
#define AUTO_MAJOR		0
#define BYTE_CNT		100
#define LED_MAX_DEVS	32768

/* ------------------led----------------------*/
#define GPJ0CON_VALUE	((0x1<<12)|(0x1<<16)|(0x1<<20))

//-----------------static io map----------------
// #define GPJ0CON	0xE0200240
// #define GPJ0DAT	0xE0200244
#define GPJ0CON		S5PV210_GPJ0CON
#define GPJ0DAT		S5PV210_GPJ0DAT

#define rGPJ0CON	(*((volatile uint32_t *)GPJ0CON))
#define rGPJ0DAT	(*((volatile uint32_t *)GPJ0DAT))


/* --------------dynamic io map----------------*/
// #define GPJ0CON	0xE0200240
// typedef struct abcd
// {
	// unsigned int gpj0_con;
	// unsigned int gpj0_data;
// } GPJ0_t;


//--------------------gpiolib------------------------
#define GPIO_J3		175
#define LABEL		"GPJ0"

static void s5pv210_led_set(struct led_classdev *led_dev,
								enum led_brightness value);

static struct led_classdev led_dev =
{
	.name = "led_test_4",
	.brightness = 10,
	.brightness_set = s5pv210_led_set,
};



static void s5pv210_led_set(struct led_classdev *led_dev,
								enum led_brightness value)
{
	//unsigned int tmpValue = 0;
	int ret = -1;
	
	//printk(KERN_INFO "s5pv210_led_set has exe\n");
	rGPJ0CON |= GPJ0CON_VALUE;
	if (0 == value)
	{
		// close led
		// rGPJ0DAT |= ((0x1<<3) | (0x1<<4) | (0x1<<5));
		
		// use io interface
		// tmpValue = readl(GPJ0DAT);
		// tmpValue |= ((0x1<<3) | (0x1<<4) | (0x1<<5));
		// writel(tmpValue, GPJ0DAT);
		
		// use gpiolib
		ret = gpio_request_one(GPIO_J3, GPIOF_DIR_OUT, LABEL);
		// if (0 != ret)
		// {
			// printk(KERN_INFO "gpio_request_one() exe error\n");
			// return;
		// }
		
		printk(KERN_INFO "ret = %d\n", ret);
		
		__gpio_set_value(GPIO_J3, 1);
		//gpio_free(GPIO_J3);
	}
	else
	{
		// tmpValue = readl(GPJ0DAT);
		// tmpValue &= ~((0x1<<3) | (0x1<<4) | (0x1<<5));
		// writel(tmpValue, GPJ0DAT);
		
		ret = gpio_request_one(GPIO_J3, GPIOF_DIR_OUT, LABEL);
		// if (0 != ret)
		// {
			// printk(KERN_INFO "gpio_request_one() exe error\n");
			// return;
		// }
		
		printk(KERN_INFO "ret = %d\n", ret);
		
		__gpio_set_value(GPIO_J3, 0);
		//gpio_free(GPIO_J3);
	}
}

static int __init led_dev_init(void)
{
	int ret = -1;
	
	// led_classdev_register(struct device *parent, struct led_classdev *dev_cdev)
	ret = led_classdev_register(NULL, &led_dev);
	
	return ret;
}

static void __exit led_dev_exit(void)
{
	led_classdev_unregister(&led_dev);
}



module_init(led_dev_init);
module_exit(led_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("frog");
MODULE_DESCRIPTION("module test");
MODULE_ALIAS("alias xxx");
