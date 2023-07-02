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

static int memory_drv_mmap(struct file *pfile, struct vm_area_struct *vma)  
{  
	int ret = 0;
	// struct memory_device *p;
	// p = pfile->private_data;
	// vma->vm_flags |= (VM_IO | VM_LOCKED | VM_DONTEXPAND | VM_DONTDUMP);
    // ret = remap_pfn_range(vma,		/* 映射虚拟内存空间 */  
    //                    vma->vm_start,/* 映射虚拟内存空间起始地址 */   
    //                    virt_to_phys(p->mem_buf)>>PAGE_SHIFT,/* 与物理内存对应的页帧号，物理地址右移12位 */
    //                    (vma->vm_end - vma->vm_start),/* 映射虚拟内存空间大小,页大小的整数倍 */  
    //                    vma->vm_page_prot);/* 保护属性 */  
    printk(KERN_INFO "memory_drv_mmap\n");
    return ret;  
} 


static bool mmap(unsigned long p_virt_addr)
{
    // int ret = 0;
   
    // // 获取物理地址
    // unsigned long phys_addr = virt_to_phys(my_driver_memory);

    // // 获取当前进程的 mm 结构体
    // struct mm_struct *mm = current->mm;

    // // 计算映射的页数
    // unsigned long npages = kmalloc_size / PAGE_SIZE;

    // // 获取用户空间虚拟地址
    // unsigned long virt_addr = p_virt_addr;
    // if(p_virt_addr == 0)
    // {
    //     return false;
    // }
    // // 映射物理页面到用户空间
    // ret = remap_pfn_range(mm, virt_addr, phys_addr >> PAGE_SHIFT, npages * PAGE_SIZE, PAGE_SHARED);
    // if (ret < 0) {
    //     // 内存映射失败
    //     printk(KERN_INFO "mem error \n");
    //     return false;
    // }
    printk(KERN_INFO "mem ok \n");
    return true;
}

static bool ummap(unsigned long p_virt_addr)
{
    //int ret = 0;
    // unsigned long virt_addr = p_virt_addr;
    // unsigned long npages = kmalloc_size / PAGE_SIZE;
    // struct mm_struct *mm = current->mm;
    
    // if(p_virt_addr == 0)
    // {
    //     return false;
    // }
    // ret = remap_pfn_range(mm, virt_addr, 0, npages * PAGE_SIZE, PAGE_NONE);
    // if (ret < 0) {
    //     // 解除内存映射失败
    //     // 错误处理
    //     printk(KERN_INFO "unmem error \n");
    //     return false;
    // }
    printk(KERN_INFO "unmem ok \n");
    return true;
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
    const char * p = NULL;
    switch (cmd) 
    {
        case MY_IOCTL_RESET:
            // 执行复位操作
            p = (const char *)my_driver_memory;
            printk(KERN_INFO "Device my_device_ioctl MY_IOCTL_RESET [%u][%u][%u][%u]\n", p[0],p[1],p[2],p[3]);
            break;
            
        case MY_IOCTL_SET_VALUE:
            // 从用户空间获取整数值并执行相应操作
            printk(KERN_INFO "Device my_device_ioctl MY_IOCTL_SET_VALUE\n");
            if (copy_from_user(&value, (int *)arg, sizeof(int))) 
            {
                return -EFAULT;
            }
            // 执行设置值操作，使用value作为参数
            break;
            
        case MY_IOCTL_GET_VALUE:
            printk(KERN_INFO "Device my_device_ioctl MY_IOCTL_GET_VALUE\n");
            // 执行获取值操作，并将结果传递回用户空间
            value = 42; // 示例，可以根据实际逻辑进行替换
            if (copy_to_user((int *)arg, &value, sizeof(int))) 
            {
                return -EFAULT;
            }
            break;
        case MY_IOCTL_MMAP_MEM:
            printk(KERN_INFO "Device my_device_ioctl MY_IOCTL_UMMAP_MEM addr[0x%lx]\n",arg);
            // 执行获取值操作，并将结果传递回用户空间
            if (!mmap(arg)) 
            {
                return -EFAULT;
            }
            break;
        case MY_IOCTL_UMMAP_MEM:
            printk(KERN_INFO "Device my_device_ioctl MY_IOCTL_UMMAP_MEM addr[0x%lx]\n",arg);
            // 执行获取值操作，并将结果传递回用户空间
            if (!ummap(arg)) 
            {
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
    .mmap = my_mmap,
};

static int __init chardev_init(void)
{
    my_driver_memory = kmalloc(kmalloc_size, GFP_KERNEL);
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
    kfree(my_driver_memory);
    printk(KERN_INFO "kfree mem[%d]\n", kmalloc_size);
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lyj_driver");
MODULE_DESCRIPTION("A simple character device driver");
