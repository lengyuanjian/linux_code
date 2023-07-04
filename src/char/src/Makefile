MY_DRIVER_NAME=lyj_driver

obj-m += $(MY_DRIVER_NAME).o

$(MY_DRIVER_NAME)-objs := char.o define.o ring_buff.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules 
clean:
	make -C $(KDIR) M=$(PWD) clean