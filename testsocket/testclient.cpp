#include "../Common/TcpSocket.h"

int main() 
{
    TcpSocket client;
    int32_t fd = client.OpenAsClient("127.0.0.1", 8888);
    std::cout << "sockfd = " << client.GetSocketfd() << std::endl;
    char msg[20] = "123456789abcdefg";
//    client.SendData(msg, 20);
    int cn;

    while ((cn = write(fd, msg, 20)) <= 0) {
        if (EINTR == errno || EAGAIN == errno) {
            printf("try rewrite\n");
            continue;
        } else {
            printf("TcpSocket::SendData failed! %s:%d", __FILE__, __LINE__);
            close(fd);
            break;
        }
    }
}

