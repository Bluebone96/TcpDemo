#ifndef __TCPSOCKET_H
#define __TCPSOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include "base.h"

#define SOCKETBUFLEN 4096
#define SET_NON_BLOCK(fd) \
do { \
    int flag = 0; \
    flag = fcntl(fd, F_GETFL, 0); \
    if (flag < 0) { \
        return -1; \
    } \
    flag |= O_NONBLOCK; \
    flag |= O_NDELAY; \
    if (fcntl(fd, F_SETFL, flag) < 0) { \
        return -1; \
    } \
    return 0; \
} while(0)

class TcpSocket {
public:
    TcpSocket(uint32_t size = SOCKETBUFLEN);
    ~TcpSocket() { free(m_pdatabuf); };

    TcpSocket(const TcpSocket&) =delete;
    TcpSocket& operator=(const TcpSocket&) = delete;

    int32_t Init(int32_t fd = -1, uint32_t size = SOCKETBUFLEN);
    int32_t OpenAsClient(const char* hostname, int16_t port);
    int32_t OpenAsServer(const int16_t port, const char* hostname= nullptr);
    
    int32_t TcpConnect(const char* host, const char* serv);
    int32_t RecvData(void* usrbuf, uint32_t size);
    int32_t SendData(void* usrbuf, uint32_t size);

    int32_t Accept(int32_t fd, sockaddr_in* ps, socklen_t* len); 

    int32_t GetFd() { return m_pdatabuf->fd; }
    const sockaddr* GetAddr() { return (sockaddr*)&m_sockaddr; }
    void SetAddr(struct sockaddr_in* sp) { memcpy(&m_sockaddr, sp, sizeof(sockaddr_in)); }

private:
    int32_t recvdata(void* usrbuf, uint32_t size);

protected:
    struct SockBuf {
        uint32_t head;
        uint32_t tail;
        char buffer[0];
    };

    int32_t m_socketfd;
    int32_t m_bufsize;
    struct sockaddr_in m_sockaddr;
    SockBuf* m_pdatabuf;
};

#endif
