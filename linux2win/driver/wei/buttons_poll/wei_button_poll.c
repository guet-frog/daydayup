#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <mach/gpio-bank.h>
#include <mach/regs-gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/gpio.h>
#include <linux/interrupt.h>

#include <linux/poll.h>



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

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);       /* - wait.h */
static uint32_t key_action = 0;
static int g_keyVal;



unsigned int buttons_poll(struct file *tmpFile, struct poll_table_struct *wait)
{
    unsigned int mask = 0;
    
    poll_wait(tmpFile, &button_waitq, wait); /* - 同样挂在队列中，但是不会立即休眠 */
    
    if (key_action)
    {
        mask |= POLLIN | POLLRDNORM;
    }
    
    //key_action = 0;           /* - not need */
    
    return mask;
}

/* - 提供中断服务函数 */
static irqreturn_t button_irq_handler(int irq, void *dev)
{
    static uint32_t irq_cnt = 0;
    //printk(KERN_INFO "irq_cnt = %d\n", irq_cnt);
    irq_cnt += 1;
    
    g_keyVal = gpio_get_value(S5PV210_GPH0(2));
    
    wake_up_interruptible(&button_waitq); /* - 把挂在队列(button_waitq)的进程唤醒 */
    key_action = 1;
    
    return IRQ_HANDLED;
}

static void x210_key_init(void)
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
    
    /* - 2. 设置gpio为EXT_INT[2] */
    
}

static void key_deinit(void)
{
    /* - 1. 释放gpio key */
    gpio_free(S5PV210_GPH0(2));
}

static int buttons_open(struct inode *tmpNode, struct file *tmpFile)
{
    /* - 打开设备文件的时候，申请中断资源 */
    /* - 对应文件关闭，释放中断资源 */
    request_irq(IRQ_EINT2, button_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "left_button", "123");
    
    return 0;
}

static ssize_t buttons_read(struct file *file, char __user *ubuf, size_t count, loff_t *offset)
{
    if (count != sizeof(int))
    {
        printk(KERN_INFO "read() -- param count error\n");
        return -1;
    }
    
    wait_event_interruptible(button_waitq, key_action);
    key_action = 0;
    
    copy_to_user(ubuf, &g_keyVal, sizeof(int));
    
    return count;
}

static int buttons_close(struct inode *tmpNode, struct file *tmpFile)
{
    free_irq(IRQ_EINT2, "123");
    
    return 0;
}

static struct file_operations fops =
{
    .owner      = THIS_MODULE,
    .open       = buttons_open,
    .read       = buttons_read,
    .release    = buttons_close,
    .poll       = buttons_poll,
};

static int buttons_init(void)
{
    int minor;
    
    x210_key_init();
    
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


