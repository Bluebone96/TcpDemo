#include "../Common/MsgTransmission.h"

int main()
{
    TcpSocket sock;
    int32_t fd = sock.OpenAsServer(8888);
    sockaddr_in clientaddr;
    socklen_t len;
    int32_t acceptfd = -1;
    while((acceptfd = sock.Accept(fd, &clientaddr, &len)) <= 0)
    {

    }

    TcpSocket client(acceptfd);
    std::cout << "New client " << acceptfd << std::endl;

    char msg[20] = "server default data";

    std::cout << "msg address: " << (void*) msg << std::endl;

    client.RecvData(msg, 20);  // 已修复

//    std::cout << "read data\n";
//    int cnt;
    // while ((cnt = read(acceptfd, msg, 20)) <= 0) {
    //     if (cnt < 0) {
    //         if (EINTR == errno || EAGAIN == errno) {
    //             continue;
    //         } else {
    //             printf("read  failed! %d:%s",errno, strerror(errno));
    //             close(fd);
    //             break;
    //         }
    //     } else if (cnt == 0) {
    //         printf("EOF\n");
    //         break;
    //     }
    // }

    std::cout <<"\nclient: " << msg << std::endl;
}
