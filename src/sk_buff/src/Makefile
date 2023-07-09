MY_DRIVER_NAME=lyj_sk_buff

obj-m += $(MY_DRIVER_NAME).o

$(MY_DRIVER_NAME)-objs := my_sk_buff.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules 
clean:
	make -C $(KDIR) M=$(PWD) clean