#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>

#include <asm/irq.h>
#include <asm/io.h>

#include <linux/gpio.h>
#include <linux/interrupt.h>
//#include <mach/irqs.h>



/*
 * X210:
 *
 * POWER  -> EINT1   -> GPH0_1      // default config ??? 2019-7-19 21:13:43
 * LEFT   -> EINT2   -> GPH0_2
 * DOWN   -> EINT3   -> GPH0_3
 * UP     -> KP_COL0 -> GPH2_0
 * RIGHT  -> KP_COL1 -> GPH2_1
 * MENU   -> KP_COL3 -> GPH2_3 (KEY_A)
 * BACK   -> KP_COL2 -> GPH2_2 (KEY_B)
 */

#define BUTTON_IRQ      IRQ_EINT2


static struct input_dev *button_dev;

void gpio_init(void)
{
    int ret = -1;

    /* gph0_2 (left) */
	ret = gpio_request(S5PV210_GPH0(2), "GPH0");
	if(ret)
	{
        printk("input_button: request gpio GPH0(2) fail\n");
    }
    
	s3c_gpio_setpull(S5PV210_GPH0(2), S3C_GPIO_PULL_UP);
    s3c_gpio_cfgpin(S5PV210_GPH0(2), S3C_GPIO_SFN(0xf));        // default?
    
	//s3c_gpio_cfgpin(S5PV210_GPH0(2), S3C_GPIO_SFN(0));
	//s3c_button_history[3] = gpio_get_value(S5PV210_GPH0(2));
}

static irqreturn_t button_interrupt(int irq, void *dummy)
{
	//input_report_key(button_dev, BTN_0, inb(BUTTON_PORT) & 1);
	//input_sync(button_dev);
    static unsigned int button_cnt = 0;

    printk(KERN_INFO "button_interrupt has exe ---- %d\n", button_cnt);

    button_cnt += 1;
    
	return IRQ_HANDLED;
}

static int __init button_init(void)
{
	int error;

    //if (request_irq(BUTTON_IRQ, button_interrupt, 0, "button", NULL))
	if (request_irq(BUTTON_IRQ, button_interrupt, IRQF_TRIGGER_LOW, "button", NULL))
	{
		printk(KERN_ERR "button.c: Can't allocate irq %d\n", BUTTON_IRQ);
        return -EBUSY;
	}
	
	button_dev = input_allocate_device();
	if (!button_dev)
	{
		printk(KERN_ERR "button.c: Not enough memory\n");
		error = -ENOMEM;
		goto err_free_irq;
	}
	
	//button_dev->evbit[0] = BIT_MASK(EV_KEY);
	//button_dev->keybit[BIT_WORD(BTN_0)] = BIT_MASK(BTN_0);
	set_bit(EV_KEY, button_dev->evbit);
    set_bit(KEY_LEFT, button_dev->keybit);
	
	error = input_register_device(button_dev);
	if (error)
	{
		printk(KERN_ERR "button.c: Failed to register device\n");
		goto err_free_dev;
	}
    
    gpio_init();
	
	return 0;
	
err_free_dev:
	input_free_device(button_dev);
	
err_free_irq:
	free_irq(BUTTON_IRQ, button_interrupt);
	
	return error;
}

static void __exit button_exit(void)
{
	input_unregister_device(button_dev);
	free_irq(BUTTON_IRQ, button_interrupt);
}

module_init(button_init);
module_exit(button_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("jerry jianjun <jerryjianjun@gmail.com>");
MODULE_DESCRIPTION("Keyboard driver for s3c button.");
MODULE_ALIAS("platform:s3c-button");

