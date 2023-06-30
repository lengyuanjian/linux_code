# 编译器设置
CC = gcc

# 编译选项设置
CFLAGS = -Wall

# 驱动程序名称
MODULE_NAME = mydriver

# 驱动程序源文件
SRCS = $(wildcard *.c)

# 驱动程序目标文件
OBJS = $(SRCS:.c=.o)

# 默认目标
all: $(MODULE_NAME)

# 生成驱动程序
$(MODULE_NAME): $(OBJS)
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# 生成目标文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f $(OBJS)

# 安装驱动程序
install:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install

# 卸载驱动程序
uninstall:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f /lib/modules/$(shell uname -r)/kernel/drivers/$(MODULE_NAME).ko
