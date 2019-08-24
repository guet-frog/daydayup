#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <mach/gpio-bank.h>
#include <mach/regs-gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>



#define AUTO_MAJOR		0
#define LED_MAX_DEVS	32768

#define GPJ0CON_VALUE	((0x1<<12)|(0x1<<16)|(0x1<<20))

#if 1 /* 物理地址 -- 裸机使用 */
 #define GPJ0CON	0xE0200240
 #define GPJ0DATA	0xE0200244
 #define REG_LEN    4
#endif
#if 0 /* 静态映射 */
#define GPJ0CON		S5PV210_GPJ0CON
#define GPJ0DAT		S5PV210_GPJ0DAT
#define rGPJ0CON	(*((volatile uint32_t *)GPJ0CON))
#define rGPJ0DAT	(*((volatile uint32_t *)GPJ0DAT))
#endif

static unsigned char g_auto_major = 0;

static char g_kbuf[100] = {0};
static struct class  *led_class = NULL;

static uint32_t *gpioJ0Con = NULL;
static uint32_t *gpioJ0Data = NULL;


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

ssize_t led_dev_read(struct file *file, char __user *ubuf, size_t count, loff_t *offset)
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

ssize_t led_dev_write(struct file *file, const char __user *ubuf, size_t count, loff_t *offset)
{
    int minor = MINOR(file->f_dentry->d_inode->i_rdev);  /* - file形参的作用 */
    printk("minor = %d\n", minor);
    
    gpioJ0Con = ioremap(GPJ0CON, REG_LEN);
    gpioJ0Data = ioremap(GPJ0DATA, REG_LEN);
    
    *gpioJ0Con = GPJ0CON_VALUE;
	
	if (copy_from_user(g_kbuf, ubuf, count))
	{
		printk(KERN_INFO "copy_from_user() exe error\n");
		return -1;
	}
    
    if ('0' != g_kbuf[0] && '1' != g_kbuf[0])
    {
        printk(KERN_ERR "led_dev_write() g_kbuf = %s\n", g_kbuf);
    }
    
    switch (minor)
    {
        case 0:
            if ('0' == g_kbuf[0])
            {
                *gpioJ0Data |= 0x1 << 3;
            }
            else
            {
                *gpioJ0Data &= ~(0x1 << 3);
            }
            break;
        case 1:
            if ('0' == g_kbuf[0])
            {
                *gpioJ0Data |= 0x1 << 4;
            }
            else
            {
                *gpioJ0Data &= ~(0x1 << 4);
            }
            break;
        case 2:
            if ('0' == g_kbuf[0])
            {
                *gpioJ0Data |= 0x1 << 5;
            }
            else
            {
                *gpioJ0Data &= ~(0x1 << 5);
            }
            break;
        default:
            printk(KERN_INFO "g_kbuf[0] = %c\n", g_kbuf[0]);
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
    uint8_t minor = 0;
    
	g_auto_major = register_chrdev(AUTO_MAJOR, "led", &led_dev_fops);
    if (g_auto_major < 0)
    {
        printk(KERN_ERR "register_chrdev() exe error\n");
    }
    else
    {
        printk(KERN_INFO "g_auto_major = %d\n", g_auto_major);
    }
    
	led_class = class_create(THIS_MODULE, "led_class_wei");     /* - 一个class下创建3个dev */
	if (IS_ERR(led_class))
	{
		printk(KERN_INFO "class_create() exe error\n");
		return -1;
	}
    
    for (minor = 0; minor < 3; minor++)
    {
        /* - 1个主设备号，3个次设备号 */
        device_create(led_class, NULL, MKDEV(g_auto_major, minor), NULL, "led_test%d", minor);
    }
    
	return 0;
}

static void __exit led_dev_exit(void)
{
    uint8_t minor = 0;

	printk(KERN_INFO "led_dev_exit() has exe\n");
	
	unregister_chrdev(g_auto_major, "led");
    
    for (minor = 0; minor < 3; minor++)
    {
        device_destroy(led_class, MKDEV(g_auto_major, minor));
    }
	
	class_destroy(led_class);

    iounmap(gpioJ0Con);
    iounmap(gpioJ0Data);
}



module_init(led_dev_init);
module_exit(led_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("frog");
MODULE_DESCRIPTION("module test");
MODULE_ALIAS("alias xxx");

/*
    -- switch case语句结构。
    -- break
*/


