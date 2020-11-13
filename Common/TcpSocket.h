#ifndef __TCPSOCKET_H
#define __TCPSOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include "base.h"

#define SOCKETBUFLEN 4096


class TcpSocket {
public:
    explicit TcpSocket(int32_t fd = -1, uint32_t size = SOCKETBUFLEN);
    ~TcpSocket() 
    { 
        free(m_pdatabuf); 
        close(m_socketfd); // ? TcpSocket::SendData() 中是否调用close()
    };

    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;


    void TcpSocketInit(int32_t fd = -1, void* addr = nullptr, int32_t addrlen = 0);

    int32_t OpenAsClient(const char* hostname, int16_t port);

    int32_t OpenAsServer(const int16_t port, const char* hostname= nullptr);
    
    int32_t TcpConnect(const char* host, const char* serv);

    // 从缓冲区读size个字节的数据
    // 成功返回0， 失败返回已接收的数据字节数
    int32_t RecvData(void* usrbuf, uint32_t size);

    // 成功返回0， 失败返回剩余未发送字节
    int32_t SendData(void* usrbuf, uint32_t size);

    int32_t Accept(int32_t fd, sockaddr_in* ps, socklen_t* len);
    int32_t Accept(sockaddr_in* ps = nullptr, socklen_t* len = nullptr) 
    { 
        return Accept(m_socketfd, ps, len); 
    };

    int32_t GetSocketfd() { return m_socketfd; }

    const sockaddr* GetAddr() { return (sockaddr*)&m_sockaddr; }

    void SetAddr(void *addr, int32_t len) { memcpy(&m_sockaddr, addr, len); }

private:
    // 从内核中读取数据到SockBuf缓冲区
    // 复制min(tail - head, size)个字节到用户缓冲区
    // 返回：成功 返回拷贝到用户缓冲区的字节数，EOF为0，出错返回-1
    int32_t recvdatabuf(void* usrbuf, uint32_t size);

    // 设置非阻塞模式
    static  int32_t setnonblock(int32_t fd);

protected:
    typedef struct SockBuf {
        // 有效区间表示为左闭右开 [head, tail)
        uint32_t head;
        uint32_t tail;
        char buffer[0];
    } *pSockBuf;
    uint32_t m_bufsize; // m_pdatabuf->buffer 大小 构造时确定

    int32_t m_socketfd;
    struct sockaddr_in m_sockaddr;
    pSockBuf m_pdatabuf;
};

#endif
