#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>


#define GLOBALMEM_SIZE 0x1000
#define GLOBALMEM_MAJOR 230
#define MEM_CLEAR 0x1
#define DEVICE_NUM 10

static int globalmem_major = GLOBALMEM_MAJOR;

struct globalmem_dev{
	struct cdev cdev;
	unsigned char mem[GLOBALMEM_SIZE];
};
struct globalmem_dev *globalmem_devp;

static int globalmem_open (struct inode *inode, struct file *flip)
{
	struct globalmem_dev *dev =container_of(inode->i_cdev, struct globalmem_dev, cdev);
	flip->private_data = dev;
	return 0;
}

	
static int globalmem_release (struct inode *inode, struct file *flip)
{
	return 0;
}
			
static long globalmem_ioctl (struct file *flip, unsigned int cmd, unsigned long arg)
{
	struct globalmem_dev *dev = flip->private_data;
	switch (cmd){
	case MEM_CLEAR:
		memset(dev->mem,0,GLOBALMEM_SIZE);
		printk(KERN_INFO "globalmem is set to zero\n");
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static loff_t globalmem_llseek (struct file *flip, loff_t offset, int orig)
{
	//struct globalmem_dev *dev = flip->private_data;
	loff_t ret = 0;
	switch(orig){
	case 0:
		if(offset<0){
			ret = -EINVAL;
			break;
		}
		if((unsigned int)offset > GLOBALMEM_SIZE){
			ret = -EINVAL;
			break;
		}
		flip->f_pos = (unsigned int)offset;
		ret = flip->f_pos;
		break;
	case 1:
		if((flip->f_pos + offset)> GLOBALMEM_SIZE){
			ret = -EINVAL;
			break;
		}
		if((flip->f_pos + offset)<0){
			ret = -EINVAL;
			break;
		}
		flip->f_pos +=offset;
		ret =flip->f_pos;
		break;
	default:
		ret = -EINVAL;
		break;
		
	}
	return ret;
}

static ssize_t globalmem_read (struct file *flip, char __user * buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret;
	struct globalmem_dev *dev = flip->private_data;
	
	if(p >= GLOBALMEM_SIZE)
		return 0;
	if(count >= GLOBALMEM_SIZE -p)
		count =GLOBALMEM_SIZE -p;
	if(copy_to_user(buf,dev->mem +p ,count)){
		ret =-EFAULT;
	}else{
		*ppos += count;
		ret = count;
		
		printk(KERN_INFO "read %u bytes(s) from %lu\n",count,p);
	}
	return ret;
}
	
static ssize_t globalmem_write (struct file *flip, const char __user * buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret;
	struct globalmem_dev *dev = flip->private_data;
	
	if(p >= GLOBALMEM_SIZE)
		return 0;
	if(count >= GLOBALMEM_SIZE -p)
		count = GLOBALMEM_SIZE -p;
	if(copy_from_user(dev->mem +p, buf, count)){
		ret = -EFAULT;
	}else{
		*ppos += count;
		ret = count;
		
		printk(KERN_INFO "written %u bytes(s) from %lu\n",count,p);
	}
	return ret;
}

static const struct file_operations globalmem_fops ={
	.owner          = THIS_MODULE,
	.llseek         = globalmem_llseek,
	.open			= globalmem_open,
	.release		= globalmem_release,
	.unlocked_ioctl = globalmem_ioctl,
	.read			= globalmem_read,
	.write			= globalmem_write,
};

static void globalmem_setup_cdev(struct globalmem_dev *dev , int index)
{
	int err,devno = MKDEV(globalmem_major, 0);
	
	cdev_init(&dev->cdev, &globalmem_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev,devno,1);
	if(err){
		printk(KERN_NOTICE "Error %d adding globalmem%d",err,index);
	}
}
static int __init globalmem_init(void)
{
	int ret;
	int i;
	dev_t devno =MKDEV(globalmem_major,0);
	if(globalmem_major){
		ret = register_chrdev_region(devno, DEVICE_NUM,"globalmem");
	}
	else{
		ret = alloc_chrdev_region(&devno,0, DEVICE_NUM, "globalmem");
		globalmem_major = MAJOR(devno);
	}
	if(ret<0){
		return ret;
	}
	globalmem_devp = kzalloc(sizeof(struct globalmem_dev)* DEVICE_NUM, GFP_KERNEL);
	if(!globalmem_devp){
		ret = -ENOMEM;
		goto fail_malloc;
	}
	for(i =0;i<DEVICE_NUM;i++)
		globalmem_setup_cdev(globalmem_devp +i,i);
	return 0;
	
	fail_malloc:
	unregister_chrdev_region(devno, 1);
	return ret;
}
static void __exit globalmem_exit(void)
{	
	int i;
	for(i =0;i<DEVICE_NUM;i++)
		cdev_del(&(globalmem_devp+i)->cdev);
	kfree(globalmem_devp);
	unregister_chrdev_region(MKDEV(globalmem_major,0),1);
}


module_init(globalmem_init);
module_exit(globalmem_exit);
MODULE_LICENSE("GPL");


