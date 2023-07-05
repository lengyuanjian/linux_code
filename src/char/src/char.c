#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include "define.h"
#include <linux/mm.h>
#include <linux/slab.h>

#include <linux/types.h>
#include <asm/io.h>

#include <linux/kthread.h>
#include <linux/delay.h>

#include <linux/random.h>

#include "ring_buff.h"
#define DEVICE_NAME "lyj_driver"
#define CLASS_NAME "lyj_device_class"

static int Major;
static struct class *mydevice_class = NULL;
static struct device *mydevice_device = NULL;

#define MY_IOCTL_MAGIC 'M'
#define MY_IOCTL_RESET _IO(MY_IOCTL_MAGIC, 0)
#define MY_IOCTL_SET_VALUE _IOW(MY_IOCTL_MAGIC, 1, int)
#define MY_IOCTL_GET_VALUE _IOR(MY_IOCTL_MAGIC, 2, int)

#define MY_IOCTL_MMAP_MEM _IOWR(MY_IOCTL_MAGIC, 3, int)
#define MY_IOCTL_UMMAP_MEM _IOWR(MY_IOCTL_MAGIC, 4, int)

static void *my_driver_memory;
static int kmalloc_size = 2 * 1024 * 1024;



static struct lgz_ring_buff g_buff;

static struct task_struct *my_thread = NULL;

static unsigned int get_rand(unsigned int min, unsigned int max)
{
    unsigned int range = max - min + 1;
    unsigned int random_value;

    // 获取随机数
    get_random_bytes(&random_value, sizeof(random_value));

    // 计算在指定范围内的随机数
    return min + (random_value % range);
}

static void push_data(void)
{
    static unsigned char cout = 0;
    static unsigned int len = 0;
    static unsigned char buff[1501] = {};
    static unsigned int i = 0;

    len = get_rand(20, 1500);
    for(i = 0; i < len; ++i)
    {
        buff[i] = cout++;
    }
    if(-1 == lgz_push_data(&g_buff, buff, len))
    {
        cout -= len;
    }
    printk(KERN_ERR "push len[%d] cut[%d] r[%d] w[%d]\n",len, cout,*(g_buff.m_r),*(g_buff.m_w));
}

static int my_thread_fn(void *data)
{
   
    while (!kthread_should_stop()) {
        // 打印日志
        
        //if(cout >= 1024 * 10)
            //break;
        // 休眠 100 毫秒
        msleep(1000);
    }

    return 0;
}

static int my_mmap(struct file *filp, struct vm_area_struct *vma)
{
    unsigned long pfn;
    unsigned long size;
    pgprot_t prot;

    // 分配物理页帧号
    pfn = __pa(my_driver_memory) >> PAGE_SHIFT;

    // 设置映射的大小
    size = vma->vm_end - vma->vm_start;

    // 设置映射的访问权限
    prot = pgprot_writecombine(vma->vm_page_prot);

    // 进行映射
    if (remap_pfn_range(vma, vma->vm_start, pfn, size, prot)) {
        printk(KERN_ERR "Failed to remap_pfn_range\n");
        return -EIO;
    }
    printk(KERN_INFO "my_mmap\n");
    return 0;
}


static long my_device_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int value;
    const int * p = NULL;
    switch (cmd) 
    {
        case MY_IOCTL_RESET:
            // 执行复位操作
            push_data();
            p = (const int *)my_driver_memory;
            printk(KERN_INFO "Device my_device_ioctl MY_IOCTL_RESET r[%u] w[%u][%u][%u]\n", p[0],p[1],p[2],p[3]);
            
            break;
            
        case MY_IOCTL_SET_VALUE:
            // 从用户空间获取整数值并执行相应操作
            // 执行设置值操作，使用value作为参数
            break;
            
        case MY_IOCTL_GET_VALUE:
            printk(KERN_INFO "Device my_device_ioctl MY_IOCTL_GET_VALUE\n");
             
            break;
        case MY_IOCTL_MMAP_MEM:
            printk(KERN_INFO "Device my_device_ioctl MY_IOCTL_UMMAP_MEM addr[0x%lx]\n",arg);
            if (my_thread == NULL) 
            {
                my_thread = kthread_run(my_thread_fn, NULL, "my_thread");
                printk(KERN_INFO "kthread_start\n");
            }
            break;
        case MY_IOCTL_UMMAP_MEM:
            printk(KERN_INFO "Device my_device_ioctl MY_IOCTL_UMMAP_MEM addr[0x%lx]\n",arg);
            if (my_thread) 
            {
                kthread_stop(my_thread);
                my_thread = NULL;
                printk(KERN_INFO "kthread_stop\n");
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
    .mmap = my_mmap,
};

static int __init chardev_init(void)
{
    lgz_init(&g_buff, kmalloc_size);
    my_driver_memory = g_buff.m_p_buff;
    if (!my_driver_memory) 
    {
        // 内存分配失败
        printk(KERN_ALERT "error kmalloc size[%d]\n", kmalloc_size);
        return -ENOMEM;
    }
     printk(KERN_ALERT "kmalloc size[%d]\n", kmalloc_size);

    // Register character device
    Major = register_chrdev(0, DEVICE_NAME, &fops);
    if (Major < 0) 
    {
        printk(KERN_ALERT "Failed to register char device: %d\n", Major);
        return Major;
    }
    printk(KERN_INFO "Registered char device with major number %d\n", Major);

    // Create device class
    mydevice_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(mydevice_class)) 
    {
        unregister_chrdev(Major, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create device class\n");
        return PTR_ERR(mydevice_class);
    }

    // Create device node
    mydevice_device = device_create(mydevice_class, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(mydevice_device)) 
    {
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
    if (my_thread) 
    {
        kthread_stop(my_thread);
        my_thread = NULL;
        printk(KERN_INFO "kthread_stop\n");
    }
    lgz_close(&g_buff);
    printk(KERN_INFO "kfree mem[%d]\n", kmalloc_size);
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lyj_driver");
MODULE_DESCRIPTION("A simple character device driver");
