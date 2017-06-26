KERN_DIR = /s5pv210/linux/kernel/linux-3.0.8
all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= led_demo.o
