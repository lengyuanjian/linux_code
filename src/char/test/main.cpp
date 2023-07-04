#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h>
#include "ring_buff.h"

#define MY_IOCTL_MAGIC 'M'
#define MY_IOCTL_RESET _IO(MY_IOCTL_MAGIC, 0)
#define MY_IOCTL_SET_VALUE _IOW(MY_IOCTL_MAGIC, 1, int)
#define MY_IOCTL_GET_VALUE _IOR(MY_IOCTL_MAGIC, 2, int)

#define DEVICE_PATH "/dev/lyj_driver"

int main() {
    int fd;
   
    char buffer[1024];
    ssize_t bytes_read, bytes_written;

    // 打开设备节点
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        printf("Failed to open device[%s]",DEVICE_PATH);
        return -1;
    }
    printf("open device[%s] fd[0x%x]\n",DEVICE_PATH,fd);


    // 从设备读取数据
    bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        perror("Failed to read from device");
        close(fd);
        return -1;
    }
    printf("Read %zd bytes from device: %s\n", bytes_read, buffer);

    // 向设备写入数据
    bytes_written = write(fd, "Hello, device!", 14);
    if (bytes_written < 0) {
        perror("Failed to write to device");
        close(fd);
        return -1;
    }
    printf("Written %zd bytes to device\n", bytes_written);

     int value = 42;
    if (ioctl(fd, MY_IOCTL_SET_VALUE, &value) == -1) {
        perror("Failed to execute MY_IOCTL_SET_VALUE");
        return 1;
    }

#define MY_IOCTL_MMAP_MEM _IOWR(MY_IOCTL_MAGIC, 3, int)
#define MY_IOCTL_UMMAP_MEM _IOWR(MY_IOCTL_MAGIC, 4, int)
    
    char *buff = (char *) mmap (NULL, 2*1024*1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    lgz_ring_buff ring_buf;
    lgz_init( &ring_buf,2*1024*1024 , buff);
    if (ioctl(fd, MY_IOCTL_RESET, &value) == -1) {
        perror("Failed to execute MY_IOCTL_MMAP_MEM");
        return 1;
    }
  
    
    if (ioctl(fd, MY_IOCTL_MMAP_MEM, &value) == -1) {
        perror("Failed to execute MY_IOCTL_SET_VALUE");
        return 1;
    }
    int * p_r = (int *)buff;
    int * p_w = (int *)(buff + 4);
    unsigned char * p_data = (unsigned char *)(buff + 8);

    int t_count = 1000 * 2;
    unsigned char cut = 0;
    int t_l = 0;
    //while(t_count--)
    {
        
        unsigned char tempp[1024] = {};
        if(-1 == lgz_pop_data(&ring_buf, (char *)tempp, 256))
        {
            printf("r[%d] w[%d]\n", *p_r, *p_w);
        }
        else
        {
            //t_l+=256;
        }
        for(int i = 0; i < 256; ++i)
        {
            if(cut++ != tempp[i])
            {
                printf("error r[%d] w[%d] [%d]\n", *p_r, *p_w, t_l);
            }
            else
            {
                t_l++;
            }
        }
        usleep(10000);
    }
    if (ioctl(fd, MY_IOCTL_UMMAP_MEM, &value) == -1) {
        perror("Failed to execute MY_IOCTL_SET_VALUE");
        return 1;
    }

    if (munmap(buff, 2*1024*1024) == -1) {
        perror("Failed to unmap device memory");
        close(fd);
        return -1;
    }
    // 关闭设备节点
    close(fd);

    return 0;
}
