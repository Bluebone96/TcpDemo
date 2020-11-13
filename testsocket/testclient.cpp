#include "../Common/MsgTransmission.hpp"

int main() 
{

    TcpSocket client;
    int32_t fd = client.OpenAsClient("127.0.0.1", 8888);
    std::cout << "sockfd = " << client.GetSocketfd() << std::endl;
    char msg[20] = "123456789abcdefghi";
    
    
    client.SendData(msg, 20); // 没问题，和下面未封装的write函数直接写。服务端都可收到数据
    // int cn;
    sleep(10);
    std::cout << "close client\n";
    // while ((cn = write(fd, msg, 20)) <= 0) {
    //     if (EINTR == errno || EAGAIN == errno) {
    //         printf("try rewrite\n");
    //         continue;
    //     } else {
    //         printf("TcpSocket::SendData failed! %s:%d", __FILE__, __LINE__);
    //         close(fd);
    //         break;
    //     }
    // }
}



