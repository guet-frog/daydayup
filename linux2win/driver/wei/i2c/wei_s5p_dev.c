#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
//#include <linux/regmap.h>
#include <linux/slab.h>


#ifndef AAA
static struct i2c_board_info s5p_info = {	
	I2C_BOARD_INFO("wm8976abc", 0x50),
};
#else
static unsigned short addr_list[] = {0x34, I2C_CLIENT_END};
#endif

static struct i2c_client *s5p_client;

static int s5p_dev_init(void)
{
#ifndef AAA
	struct i2c_adapter *i2c_adap;
    
    
	i2c_adap = i2c_get_adapter(2);
    if (NULL == i2c_adap)
    {
        printk("i2c_get_adapter() exe error\n");
    }
    
	s5p_client = i2c_new_device(i2c_adap, &s5p_info);
    if (NULL == s5p_client)
    {
        printk("i2c_new_device() exe error\n");
    }
    
	i2c_put_adapter(i2c_adap);


    return 0;
#else
    struct i2c_adapter *i2c_adap;
    struct i2c_board_info s5p_info;
    uint32_t i = 0;

    memset(&s5p_info, 0, sizeof(struct i2c_board_info));	
	strlcpy(s5p_info.type, "wm8976abc", I2C_NAME_SIZE);
    
	i2c_adap = i2c_get_adapter(2);

//    for (i = 0; i < 0xff; i++)
//    {
//        addr_list[0] = i;
//    
//        s5p_client = i2c_new_probed_device(i2c_adap, &s5p_info, addr_list);
//        
//        if (NULL != s5p_client)
//        {
//            printk("addr = %#x\n", i);
//            break;
//        }
//    }
//
//    if (0xff == i)
//    {
//        printk("error -------------\n");
//    }
    
	s5p_client = i2c_new_probed_device(i2c_adap, &s5p_info, addr_list);
	i2c_put_adapter(i2c_adap);
    
	if (s5p_client)
		return 0;
	else
	{
        printk("i2c_new_probed_device() exe error\n");
        return -ENODEV;
    }
		
#endif /* AAA */
}

static void s5p_dev_exit(void)
{
	i2c_unregister_device(s5p_client);
}


module_init(s5p_dev_init);
module_exit(s5p_dev_exit);
MODULE_LICENSE("GPL");


