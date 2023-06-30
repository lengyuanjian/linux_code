#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    // 组播地址和端口号
    const char* multicast_group = "224.0.0.1";
    const int server_port = 10000;

    // 创建套接字
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        cerr << "Failed to create socket" << endl;
        return 1;
    }

    // 绑定端口
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(multicast_group);
    server_address.sin_port = htons(server_port);
    if (bind(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        cerr << "Failed to bind socket" << endl;
        return 1;
    }

    // 加入组播组
    struct ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.imr_multiaddr.s_addr = inet_addr(multicast_group);
    mreq.imr_interface.s_addr = inet_addr("192.168.118.138");
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0) {
        cerr << "Failed to join multicast group" << endl;
        return 1;
    }
    cout << "Received " <<"\n";
    cout << "multicast_group"<<multicast_group<<"\n";
    cout << "server_port"<<server_port<<"\n";
    // 接收数据并打印
    while (true) {
        char buffer[2048];
        memset(buffer, 0, sizeof(buffer));
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        int bytes_received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_address, &client_address_len);
        if (bytes_received < 0) {
            cerr << "Failed to receive data" << endl;
            break;
        }
        cout << "Received " << bytes_received << " bytes from " << inet_ntoa(client_address.sin_addr) << ": " << buffer << endl;
    }

    // 关闭套接字
    close(sock);

    return 0;
}
