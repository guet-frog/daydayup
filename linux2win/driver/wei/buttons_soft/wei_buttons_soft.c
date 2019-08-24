#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <mach/gpio-bank.h>
#include <mach/regs-gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/gpio.h>


/*
 * X210:
 *
 * POWER  -> EINT1   -> GPH0_1
 * LEFT   -> EINT2   -> GPH0_2
 * DOWN   -> EINT3   -> GPH0_3
 * UP     -> KP_COL0 -> GPH2_0
 * RIGHT  -> KP_COL1 -> GPH2_1
 * MENU   -> KP_COL3 -> GPH2_3 (KEY_A)
 * BACK   -> KP_COL2 -> GPH2_2 (KEY_B)
 */


#define KEY_NUM     1

#define DEV_NAME    "buttons"
#define CLASS_NAME  "class_buttons"

static int g_auto_major = -1;
static struct class *class_buttons = NULL;


static void key_init(void)
{
    int ret = -1;

    /* - 1. 申请gpio key */
    ret = gpio_request(S5PV210_GPH0(2), "GPH0");
    if (ret < 0)
    {
        printk(KERN_INFO "gpio_request() exe error\n");
    }
    
    /* - 2. 设置gpio为输入 */
    s3c_gpio_cfgpin(S5PV210_GPH0(2), S3C_GPIO_SFN(0));
}

static void key_deinit(void)
{
    /* - 1. 释放gpio key */
    gpio_free(S5PV210_GPH0(2));
}

static int buttons_open(struct inode *tmpNode, struct file *tmpFile)
{
    //key_init();

    return 0;
}

static ssize_t buttons_read(struct file *file, char __user *ubuf, size_t count, loff_t *offset)
{
    int val = 0;

    if (count != sizeof(int))
    {
        printk(KERN_INFO "read() -- param count error\n");
        return -1;
    }
    
    val = gpio_get_value(S5PV210_GPH0(2));
    //printk(KERN_INFO "kernel val = %d\n", val);
    
    copy_to_user(ubuf, &val, sizeof(int));
    
    return count;
}

static int buttons_close(struct inode *tmpNode, struct file *tmpFile)
{
    
    return 0;
}

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = buttons_open,
    .read = buttons_read,
    .release = buttons_close,
};

static int buttons_init(void)
{
    int minor;

    key_init();

    g_auto_major = register_chrdev(0, DEV_NAME, &fops);
    if (g_auto_major < 0)
    {
        return -1;
    }
    
    class_buttons = class_create(THIS_MODULE, CLASS_NAME);

    for (minor = 0; minor < KEY_NUM; minor++)
    {
        device_create(class_buttons, NULL, MKDEV(g_auto_major, minor), NULL, "button%d", minor);
    }

    return 0;
}

static void buttons_exit(void)
{
    int minor;
    
    key_deinit();
    
    for (minor = 0; minor < KEY_NUM; minor++)
    {
         device_destroy(class_buttons, MKDEV(g_auto_major, minor));
    }
    
    class_destroy(class_buttons);
    
    unregister_chrdev(g_auto_major, DEV_NAME);
}



module_init(buttons_init);
module_exit(buttons_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("frog");
MODULE_DESCRIPTION("module test");
MODULE_ALIAS("alias xxx");




//    val = gpio_get_value(S5PV210_GPH0(2));
//    unsigned int regval;
//    regval = *gpfdat;
//    key_val[0] = (regval & (1 << 0)) ? 1 : 0;	/* - 赋值0或1 可以考虑使用三目运算符 */
