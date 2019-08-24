#include <linux/module.h>
#include <linux/init.h>



static int __init chrdev_init(void)
{
	printk(KERN_INFO "chrdev_init helloworld init\n");
	printk(KERN_DEBUG "test cmd print\n");
	//printk("<7> chrdev_init helloworld init\n");

	return 0;
}

static void __exit chrdev_exit(void)
{
	printk(KERN_INFO "chrdev_exit helloworld exit\n");
}



module_init(chrdev_init);
module_exit(chrdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("frog");
MODULE_DESCRIPTION("module test");
MODULE_ALIAS("alias xxx");
