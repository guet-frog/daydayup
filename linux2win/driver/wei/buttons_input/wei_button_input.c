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

#include <linux/input.h>



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


#define KEY_NUM     3

#define DEV_NAME    "buttons"
#define CLASS_NAME  "class_buttons"

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);       /* - wait.h */
static uint8_t key_flag = 0;
static int g_keyVal;

static struct timer_list button_timer;
static struct input_dev *button_dev;




void button_timer_func(unsigned long data)
{
    unsigned int code;
    
    if (key_flag)
    {
        key_flag = 0;
    }
    else
    {
       return;
    }
    
    if (1 == key_flag)
    {
        g_keyVal = gpio_get_value(S5PV210_GPH0(1));
        code = KEY_S;
    }
    else if (2 == key_flag)
    {
        g_keyVal = gpio_get_value(S5PV210_GPH0(2));
        code = KEY_L;
    }
    else
    {
        g_keyVal = gpio_get_value(S5PV210_GPH0(3));
        code = KEY_ENTER;
    }

    if (0 == g_keyVal)  /* - 按键按下 */
    {
        input_event(button_dev, EV_KEY, code, 1);
        //input_sync(button_dev);
    }
    else
    {
        input_event(button_dev, EV_KEY, code, 0);
        //input_sync(button_dev);
    }

    input_sync(button_dev);
    //printk(KERN_INFO "key_flag = %d\n");
    
}

/* - 提供中断服务函数 */
static irqreturn_t button_irq_handler(int irq, void *dev)
{
    if (IRQ_EINT1 == irq)
    {
        key_flag = 1;
    }
    else if (IRQ_EINT2 == irq)
    {
        key_flag = 2;
    }
    else
    {
        key_flag = 3;
    }

    //printk(KERN_INFO "key_flag = %d\n", key_flag);
    
    mod_timer(&button_timer, jiffies + HZ/25);      /* - 时间片 = 4ms ??? */
    
    return IRQ_HANDLED;
}

static void x210_key_init(void)
{
    int ret = -1;
    
    /* - 1. 申请gpio key */
    ret = gpio_request(S5PV210_GPH0(1), "GPH0");
    ret = gpio_request(S5PV210_GPH0(2), "GPH0");
    ret = gpio_request(S5PV210_GPH0(3), "GPH0");
    
    /* - 2. 设置gpio为输入 */
    s3c_gpio_cfgpin(S5PV210_GPH0(1), S3C_GPIO_SFN(0xf));
    
    /* - 2. 设置gpio为EXT_INT[2] */
}

static void key_deinit(void)
{
    /* - 1. 释放gpio key */
    gpio_free(S5PV210_GPH0(1));
    gpio_free(S5PV210_GPH0(2));
    gpio_free(S5PV210_GPH0(3));
}

static int buttons_init(void)
{
    /* - 1. 分配 input dev */
    button_dev = input_allocate_device();
    
    /* - 2. 设置 */
    /* - 2.1 能产生哪类事件 */
    set_bit(EV_KEY, button_dev->evbit);
    //set_bit(EV_REP, button_dev->evbit);
    
    /* - 2.2 这类操作能产生哪些事件 */
    set_bit(KEY_L, button_dev->keybit);
    set_bit(KEY_S, button_dev->keybit);
    set_bit(KEY_ENTER, button_dev->keybit);
    
    /* - 3. 注册 */
    input_register_device(button_dev);
    
    /* - 4. 硬件相关操作 */
    x210_key_init();
    
    /* - irq init */
    request_irq(IRQ_EINT1, button_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "power_button", "123");
    request_irq(IRQ_EINT2, button_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "left_button",  "123");
    request_irq(IRQ_EINT3, button_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "down_button",  "123");
    
    /* - 设置定时器 */
    init_timer(&button_timer);
    button_timer.function = button_timer_func;
    add_timer(&button_timer);
    
    return 0;
}

static void buttons_exit(void)
{
    key_deinit();
    free_irq(IRQ_EINT1, "123");
    free_irq(IRQ_EINT2, "123");
    free_irq(IRQ_EINT3, "123");
    del_timer(&button_timer);

    input_unregister_device(button_dev);
    input_free_device(button_dev);
}



module_init(buttons_init);
module_exit(buttons_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("frog");
MODULE_DESCRIPTION("module test");
MODULE_ALIAS("alias xxx");


