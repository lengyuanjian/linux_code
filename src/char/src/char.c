#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include "define.h"

#define DEVICE_NAME "lyj_driver"
#define CLASS_NAME "mydevice_class"

static int Major;
static struct class *mydevice_class = NULL;
static struct device *mydevice_device = NULL;

#define MY_IOCTL_MAGIC 'M'
#define MY_IOCTL_RESET _IO(MY_IOCTL_MAGIC, 0)
#define MY_IOCTL_SET_VALUE _IOW(MY_IOCTL_MAGIC, 1, int)
#define MY_IOCTL_GET_VALUE _IOR(MY_IOCTL_MAGIC, 2, int)

long my_device_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int value;
    
    switch (cmd) {
        case MY_IOCTL_RESET:
            // 执行复位操作
            printk(KERN_INFO "Device my_device_ioctl MY_IOCTL_RESET\n");
            break;
            
        case MY_IOCTL_SET_VALUE:
            // 从用户空间获取整数值并执行相应操作
            printk(KERN_INFO "Device my_device_ioctl MY_IOCTL_SET_VALUE\n");
            if (copy_from_user(&value, (int *)arg, sizeof(int))) {
                return -EFAULT;
            }
            // 执行设置值操作，使用value作为参数
            break;
            
        case MY_IOCTL_GET_VALUE:
            printk(KERN_INFO "Device my_device_ioctl MY_IOCTL_GET_VALUE\n");
            // 执行获取值操作，并将结果传递回用户空间
            value = 42; // 示例，可以根据实际逻辑进行替换
            if (copy_to_user((int *)arg, &value, sizeof(int))) {
                return -EFAULT;
            }
            break;
            
        default:
        printk(KERN_INFO "Device my_device_ioctl ENOTTY\n");
            return -ENOTTY; // 不支持的命令
    }
    
    return 0; // 返回0表示成功
}



static int device_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
    // Read device data
    return 0;
}

static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
    // Write device data
    return length;
}

static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = my_device_ioctl,
};

static int __init chardev_init(void)
{
    // Register character device
    Major = register_chrdev(0, DEVICE_NAME, &fops);
    if (Major < 0) {
        printk(KERN_ALERT "Failed to register char device: %d\n", Major);
        return Major;
    }
    printk(KERN_INFO "Registered char device with major number %d\n", Major);

    // Create device class
    mydevice_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(mydevice_class)) {
        unregister_chrdev(Major, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create device class\n");
        return PTR_ERR(mydevice_class);
    }

    // Create device node
    mydevice_device = device_create(mydevice_class, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(mydevice_device)) {
        class_destroy(mydevice_class);
        unregister_chrdev(Major, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create device\n");
        return PTR_ERR(mydevice_device);
    }

    printk(KERN_INFO "Device node created\n");

    return 0;
}

static void __exit chardev_exit(void)
{
    device_destroy(mydevice_class, MKDEV(Major, 0));
    class_unregister(mydevice_class);
    class_destroy(mydevice_class);
    unregister_chrdev(Major, DEVICE_NAME);
    printk(KERN_INFO "Unregistered char device\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lyj_driver");
MODULE_DESCRIPTION("A simple character device driver");
