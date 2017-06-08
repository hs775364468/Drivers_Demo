#include <linux/kernel.h>
#include <linux/module.h>

static int __init firdrv_init(void)
{
	printk("firdrv is insmod!\n");
	return 0;
}
static void __exit firdrv_exit(void)
{
	printk("firdrv is rmmod!\n");
}

module_init(firdrv_init);
module_exit(firdrv_exit);
MODULE_LICENSE("GPL");