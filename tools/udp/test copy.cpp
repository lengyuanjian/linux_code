#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int main1() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); // 创建UDP socket
    if (sockfd == -1) {
        perror("socket() error");
        return -1;
    }

    struct sockaddr_in localaddr;
    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = inet_addr("224.0.0.1"); // 绑定到组播组地址上
    localaddr.sin_port = htons(10000); // 绑定到指定端口

    int ret = bind(sockfd, (struct sockaddr*)&localaddr, sizeof(localaddr)); // 绑定socket到指定IP地址和端口
    if (ret == -1) {
        perror("bind() error");
        close(sockfd);
        return -1;
    }

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr("224.0.0.1"); // 设置组播组地址
    mreq.imr_interface.s_addr = inet_addr("192.168.118.138"); // 设置网卡IP地址
    ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)); // 启用组播功能并绑定到指定网卡
    if (ret == -1) {
        perror("setsockopt() error");
        close(sockfd);
        return -1;
    }

    char buf[1024];
    struct sockaddr_in srcaddr;
    socklen_t addrlen = sizeof(srcaddr);
    ret = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&srcaddr, &addrlen); // 接收数据
    if (ret == -1) {
        perror("recvfrom() error");
        close(sockfd);
        return -1;
    }

    std::cout << "Received multicast message: " << buf << std::endl;

    close(sockfd);
    return 0;
}
