#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <mach/gpio-bank.h>
#include <mach/regs-gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define LED_MAJOR		100
#define AUTO_MAJOR		0
#define BYTE_CNT		100
#define LED_MAX_DEVS	32768

/* ------------------led----------------------*/
#define GPJ0CON_VALUE	((0x1<<12)|(0x1<<16)|(0x1<<20))

// #define GPJ0CON	0xE0200240
// #define GPJ0DAT	0xE0200244
#define GPJ0CON		S5PV210_GPJ0CON
#define GPJ0DAT		S5PV210_GPJ0DAT

#define rGPJ0CON	(*((volatile uint32_t *)GPJ0CON))
#define rGPJ0DAT	(*((volatile uint32_t *)GPJ0DAT))

#define NEW_INTERFACE
#define AUTO_MKNOD

#ifndef NEW_INTERFACE
static unsigned char g_auto_major = 0;
#else
static dev_t g_auto_major = 0;
#endif /* NEW_INTERFACE */

static char g_kbuf[BYTE_CNT] = {0};
static struct class  *led_class = NULL;

int led_dev_open(struct inode *tmpNode, struct file *tmpFile)
{
	printk(KERN_INFO "led_dev_open() has exe\n");
	
	return 0;
}

int led_dev_release(struct inode *tmpNode, struct file *tmpFile)
{
	printk(KERN_INFO "led_dev_release() has exe\n");
	
	return 0;
}

ssize_t led_dev_read(struct file *file, char __user *ubuf, size_t count,
						loff_t *offset)
{
	printk(KERN_INFO "led_dev_read() has exe\n");
	
	if (copy_to_user(ubuf, g_kbuf, count))
	{
		return -1;
	}
	else
	{
		printk("ubuf = %s\n", ubuf);
	}
	
	return 0;
}

ssize_t led_dev_write(struct file *file, const char __user *ubuf, size_t count,
						loff_t *offset)
{
	printk(KERN_INFO "led_dev_write() has exe\n");
	
	if (copy_from_user(g_kbuf, ubuf, count))
	{
		printk(KERN_INFO "copy_from_user() exe error\n");
		return -1;
	}
	else
	{
		//printk("g_kbuf = %s\n", g_kbuf);
		
		// printk("\ncount = %d\n", count);
		// int i;
		// for (i = 0; i < count; i++)
		// {
			// printk("g_kbuf[%d] = %c\n", i, g_kbuf[i]);
		// }
		
		rGPJ0CON = GPJ0CON_VALUE;
		if ('1' == g_kbuf[0])
		{
			rGPJ0DAT &= ~((0x1<<3)|(0x1<<4)|(0x1<<5));
		}
		else if ('0' == g_kbuf[0])
		{
			rGPJ0DAT |= ((0x1<<3)|(0x1<<4)|(0x1<<5));
		}
		else
		{
			printk("led_dev_write() cmd not found\n");
		}
	}
	
	return 0;
}

static const struct file_operations led_dev_fops =
{
	.owner		= THIS_MODULE,
	.read		= led_dev_read,
	.write		= led_dev_write,
	.open		= led_dev_open,
	.release	= led_dev_release
};

static int __init led_dev_init(void)
{
#ifdef NEW_INTERFACE
	int ret = 0;
	struct cdev *cdev = NULL;
#endif /* NEW_INTERFACE */
	
	printk(KERN_INFO "led dev driver register\n");
	
#ifndef NEW_INTERFACE
	g_auto_major = register_chrdev(AUTO_MAJOR, "led", &led_dev_fops);
#else
	ret = alloc_chrdev_region(&g_auto_major, 0, LED_MAX_DEVS, "led");
	if (ret)
	{
		printk(KERN_INFO "alloc_chrdev_region() exe error\n");
		return -1;
	}
	printk(KERN_INFO "g_auto_major = %d\n", MAJOR(g_auto_major));
	
	cdev = cdev_alloc();
	if (!cdev)
	{
		printk(KERN_INFO "cdev_alloc() exe error\n");
		return -1;
	}
	
	cdev_init(cdev, &led_dev_fops);
	
	ret = cdev_add(cdev, g_auto_major, LED_MAX_DEVS);
	//ret = cdev_add(cdev, MKDEV(g_auto_major, 0), LED_MAX_DEVS);	// 次设备号error 2019-6-25 15:24:14？
	if (ret)
	{
		printk(KERN_INFO "cdev_add() exe error\n");
	}
#endif /* NEW_INTERFACE */

#ifdef AUTO_MKNOD
	led_class = class_create(THIS_MODULE, "led_class_test");
	if (IS_ERR(led_class))
	{
		printk(KERN_INFO "class_create() exe error\n");
		return -1;
	}
	
	device_create(led_class, NULL, g_auto_major, NULL, "led_test");
#endif /* AUTO_MKNOD */

	return 0;
}

static void __exit led_dev_exit(void)
{
	printk(KERN_INFO "led_dev_exit() has exe\n");
	
#ifndef NEW_INTERFACE
	unregister_chrdev(g_auto_major, "led");
#else
	unregister_chrdev_region(g_auto_major, LED_MAX_DEVS);
	
	device_destroy(led_class, g_auto_major);
	class_destroy(led_class);
#endif /* NEW_INTERFACE */
}



module_init(led_dev_init);
module_exit(led_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("frog");
MODULE_DESCRIPTION("module test");
MODULE_ALIAS("alias xxx");
