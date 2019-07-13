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
#include <linux/gpio.h>
#include <mach/leds-gpio.h>

#include <linux/platform_device.h>
#include <linux/string.h>

/* - define led class struct */
static struct led_classdev cdev1, cdev2, cdev3;




static void led_init(void)
{
	int ret;

	ret = gpio_request(S5PV210_GPJ0(3), "GPJ0");
	if(ret)
	{
		printk(KERN_INFO "x210-led: request gpio GPJ0(3) fail\n");
	}
	s3c_gpio_setpull(S5PV210_GPJ0(3), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPJ0(3), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPJ0(3), 1);

	ret = gpio_request(S5PV210_GPJ0(4), "GPJ0");
	if(ret)
	{
		printk(KERN_INFO "x210-led: request gpio GPJ0(4) fail\n");
	}
	s3c_gpio_setpull(S5PV210_GPJ0(4), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPJ0(4), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPJ0(4), 1);

	ret = gpio_request(S5PV210_GPJ0(5), "GPJ0");
	if(ret)
	{
		printk(KERN_INFO "x210-led: request gpio GPJ0(5) fail\n");
	}
	s3c_gpio_setpull(S5PV210_GPJ0(5), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPJ0(5), S3C_GPIO_SFN(1));
	gpio_set_value(S5PV210_GPJ0(5), 1);
}

static void led_deinit(void)
{
    gpio_free(S5PV210_GPJ0(3));
    gpio_free(S5PV210_GPJ0(4));
    gpio_free(S5PV210_GPJ0(5));
}

static void s5pv210_led_set(struct led_classdev *led_dev, enum led_brightness value)
{
	printk(KERN_INFO "[%s]\n", led_dev->name);
	
	if (!strcmp("__led1__", led_dev->name))
	{
		gpio_set_value(S5PV210_GPJ0(3), ((0 == value) ? 1 : 0));
	}
	else if (!strcmp("__led2__", led_dev->name))
	{
		gpio_set_value(S5PV210_GPJ0(4), ((0 == value) ? 1 : 0));
	}
	else if (!strcmp("__led3__", led_dev->name))
	{
		gpio_set_value(S5PV210_GPJ0(5), ((0 == value) ? 1 : 0));
	}
	else
	{
		printk(KERN_INFO "%s\n", led_dev->name);
	}
}

static int s5pv210_led_remove(struct platform_device *dev)
{
    struct s5pv210_led_platdata *pdata = dev->dev.platform_data;

	printk(KERN_INFO "remove -- %s\n", pdata->name);
	
	if (!strcmp("__led1__", pdata->name))
	{
		led_classdev_unregister(&cdev1);
	}
	else if (!strcmp("__led2__", pdata->name))
	{
		led_classdev_unregister(&cdev2);
	}
	else if (!strcmp("__led3__", pdata->name))
	{
		led_classdev_unregister(&cdev3);
	}
	
	return 0;
}

static int s5pv210_led_probe(struct platform_device *dev)
{
	int ret = -1;
	struct led_classdev *cdev = NULL;
	struct s5pv210_led_platdata *pdata = dev->dev.platform_data;
	
	printk(KERN_INFO "[%s]\n", pdata->name);
	
	if (!strcmp("__led1__", pdata->name))
	{
		cdev = &cdev1;
	}
	else if (!strcmp("__led2__", pdata->name))
	{
		cdev = &cdev2;
	}
	else if (!strcmp("__led3__", pdata->name))
	{
		cdev = &cdev3;
	}
	else
	{
		printk(KERN_INFO "%s\n", pdata->name);
	}
	
	cdev->name = pdata->name;
	cdev->brightness_set = s5pv210_led_set;
	
	ret = led_classdev_register(&dev->dev, cdev);
	if (ret < 0)
	{
		printk(KERN_INFO "led_classdev_register exe error\n");
		
		return -1;
	}
	
	return 0;
}

static struct platform_driver s5pv210_led_driver =
{
	.probe = s5pv210_led_probe,
	.remove = s5pv210_led_remove,
	.driver =
	{
		.name = "s5pv210_led__",
		.owner = THIS_MODULE,
	}
};

static int __init led_dev_init(void)
{
	led_init();
	
	return platform_driver_register(&s5pv210_led_driver);
}

static void __exit led_dev_exit(void)
{
	led_deinit();
	
	platform_driver_unregister(&s5pv210_led_driver);
}



module_init(led_dev_init);
module_exit(led_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("frog");
MODULE_DESCRIPTION("module test");
MODULE_ALIAS("alias xxx");
