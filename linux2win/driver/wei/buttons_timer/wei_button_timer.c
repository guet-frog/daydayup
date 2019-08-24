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

#include <asm/io.h>



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

#define POWER_KEY
#define GPH0_1_ADDR     0xE0200C00

#define KEY_NUM     1

#define DEV_NAME    "buttons"
#define CLASS_NAME  "class_buttons"

static int g_auto_major = -1;
static struct class *class_buttons = NULL;

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);       /* - wait.h */
static uint32_t key_action = 0;
static uint8_t key_flag = 0;
static int g_keyVal;

static struct timer_list button_timer;

static uint32_t *g_addr;



void button_timer_func(unsigned long data)
{
    if (key_flag)
    {
        key_flag = 0;
    }
    else
    {
       return; 
    }

#ifndef POWER_KEY
    g_keyVal = gpio_get_value(S5PV210_GPH0(2));
#else
    g_keyVal = gpio_get_value(S5PV210_GPH0(1));
#endif /* POWER_KEY */
    
    wake_up_interruptible(&button_waitq); /* - 把挂在队列(button_waitq)的进程唤醒 */
    key_action = 1;
}

/* - 提供中断服务函数 */
static irqreturn_t button_irq_handler(int irq, void *dev)
{
//    static uint32_t irq_cnt = 0;
//    printk(KERN_INFO "irq_cnt = %d\n", irq_cnt);
//    irq_cnt += 1;
//    
//    g_keyVal = gpio_get_value(S5PV210_GPH0(2));
//    
//    wake_up_interruptible(&button_waitq); /* - 把挂在队列(button_waitq)的进程唤醒 */
//    key_action = 1;

    key_flag = 1;
    mod_timer(&button_timer, jiffies + HZ/25);      /* - 时间片 = 4ms ??? */
    
    return IRQ_HANDLED;
}

static void x210_key_init(void)
{
    int ret = -1;
    
    /* - 1. 申请gpio key */
#ifndef POWER_KEY
    ret = gpio_request(S5PV210_GPH0(2), "GPH0");
#else
    ret = gpio_request(S5PV210_GPH0(1), "GPH0");
#endif
    if (ret < 0)
    {
        printk(KERN_INFO "gpio_request() exe error\n");
    }
    
    /* - 2. 设置gpio为输入 */
    //s3c_gpio_cfgpin(S5PV210_GPH0(2), S3C_GPIO_SFN(0));
    
    /* - 2. 设置gpio为EXT_INT[2] */
    g_addr = ioremap(GPH0_1_ADDR, 4);
    if (NULL == g_addr)
    {
        printk(KERN_ERR "ioremap error \n");
    }

    *g_addr |= (0xf << 4);
    
}

static void key_deinit(void)
{
    /* - 1. 释放gpio key */
#ifndef POWER_KEY
    gpio_free(S5PV210_GPH0(2));
#else
    gpio_free(S5PV210_GPH0(1));
#endif
}

static int buttons_open(struct inode *tmpNode, struct file *tmpFile)
{
    /* - 打开设备文件的时候，申请中断资源 */
    /* - 对应文件关闭，释放中断资源 */
#ifndef POWER_KEY
    request_irq(IRQ_EINT2, button_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "left_button", "123");
#else
    request_irq(IRQ_EINT1, button_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "power_button", "123");
#endif

//    /* - 设置定时器 */
//    init_timer(&button_timer);
//    button_timer.function = button_timer_func;
//    add_timer(&button_timer);

    /* - 定时器放在这里初始化，安装该模块，执行app，内核崩溃重启 */
    /* - 放在init中，可以正常运行 -- 2019-8-8 18:02:22 */
    
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
#ifndef POWER_KEY
    free_irq(IRQ_EINT2, "123");
#else
    free_irq(IRQ_EINT1, "123");
#endif
    
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
    
    x210_key_init();

    /* - 设置定时器 */
    init_timer(&button_timer);
    button_timer.function = button_timer_func;
    add_timer(&button_timer);
    
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
    
    //del_timer(&button_timer);
    
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


