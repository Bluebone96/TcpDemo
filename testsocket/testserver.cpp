#include "../Common/TcpSocket.h"


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

    char msg[20];

    client.RecvData(msg, 100); // BUG 
    // std::cout << "read data\n";
    // int cnt;
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