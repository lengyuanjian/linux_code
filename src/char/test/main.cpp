#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

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
        perror("Failed to open device");
        return -1;
    }

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

    // 关闭设备节点
    close(fd);

    return 0;
}
