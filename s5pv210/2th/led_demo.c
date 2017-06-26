#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/string.h>

#include <linux/major.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <linux/kobject.h>
#include <linux/cdev.h>

static dev_t led_dev;

static struct leddev{
	struct cdev cdev;
	unsigned int state;
};

static struct leddev *led_devp;

static int led_open (struct inode *inode, struct file *filp)
{
	printk("this is led_open!\n");
	return 0;
}

static ssize_t led_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	printk("this is led_read!\n");
	return 1;
}

static ssize_t led_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk("this is led_write!\n");
	return 1;

}
static long led_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk("this is led_ioctl!\n");
	return 0;
}
	
static int led_close (struct inode *inode, struct file *filp)
{
	printk("this is led_close!\n");
	return 0;
}
	
static struct file_operations led_ops ={
	.owner			= THIS_MODULE,
	.open           = led_open,
	.read           = led_read,
	.write          = led_write,
	.unlocked_ioctl = led_ioctl,
	.release        = led_close,	
};
static int __init leddemo_init(void)
{
	int iret;
	printk("this is leddemo_init!\n");
	//1.alloc dev_t for led
	iret = alloc_chrdev_region(&led_dev, 0, 1, "led");
	if(iret <0){
		printk("alloc dev_t for led failed!\n");
		return -1;
	}
	//2.alloc memory for led struct
	
	led_devp = kmalloc(sizeof(struct leddev), GFP_KERNEL);
	if(!led_devp){
		printk("alloc memory for led struct failed!\n");
		unregister_chrdev_region(led_dev,1);
		return -1;
	}
	memset(led_devp, 0, sizeof(struct leddev));
	
	//3.cdev_init
	cdev_init(&led_devp->cdev, &led_ops);
	led_devp->cdev.owner = THIS_MODULE;
	led_devp->cdev.ops   = &led_ops;
	
	//4.cdev_add
	cdev_add(&led_devp->cdev,led_dev,1);
	
	return 0;
}
static void __exit leddemo_exit(void)
{
	printk("this is leddemo_exit!\n");
	cdev_del(&led_devp->cdev);
	kfree(led_devp);
	unregister_chrdev_region(led_dev,1);
}


module_init(leddemo_init);
module_exit(leddemo_exit);
MODULE_LICENSE("GPL");