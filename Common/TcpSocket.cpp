#include <cstdlib>
#include <arpa/inet.h>

#include "TcpSocket.h"
#include "log.h"

TcpSocket::TcpSocket(int32_t fd, uint32_t sz) : m_socketfd(fd), m_pdatabuf(nullptr)
{
    if (sz > SOCKETBUFLEN) {
        sz = SOCKETBUFLEN;
    }

    m_bufsize = sz;
    m_pdatabuf = (pSockBuf)malloc(sizeof(SockBuf) + sz);
    bzero(m_pdatabuf, sz + sizeof(SockBuf));
}

void TcpSocket::TcpSocketInit(int32_t fd, void *addr, int32_t addrlen)
{
    m_socketfd = fd;
    SetAddr(addr, addrlen);
//    bzero(m_pdatabuf, m_bufsize);
}

int32_t TcpSocket::recvdatabuf(void* usrbuf, uint32_t size)
{

    uint32_t sn = m_pdatabuf->tail - m_pdatabuf->head;

    // 先从缓冲区拷贝数据，如果够就直接返回，不够就从内核读数据到缓冲区
    sn = MIN(sn, size);
    
    TRACER("memcpy %d data to usrbuf:%p start \n", sn, usrbuf);

    memcpy(usrbuf, m_pdatabuf->buffer + m_pdatabuf->head, sn);

    TRACER("memcpy %d data to usrbuf:%p end \n", sn, usrbuf);

    m_pdatabuf->head += sn;

    if (sn == size) {
        return size;
    }
    
    // 每次直接尝试填充缓冲区
    char* pbuf = m_pdatabuf->buffer + m_pdatabuf->tail;
    TRACER("start recv data from %d to socketbuf\nsocket buf is %d left\n", m_socketfd, m_bufsize - m_pdatabuf->tail);
    int cnt = 0;
    while (m_bufsize > m_pdatabuf->tail) {
        
        // m_socketfd是非阻塞的，当没数据时返回 EWOULDBLOCK（一般等于EAGAIN)
        cnt = read(m_socketfd, pbuf, m_bufsize - m_pdatabuf->tail);
    
        if (cnt < 0) {
            if (errno == EINTR) {
                continue;                       // 系统中断继续
            } else if (errno == EAGAIN) {
                TRACER("socketfd %d no data to read, cnt = %d\n", m_socketfd, cnt);
                break;                          // 无数据退出循环
            } else {
                TRACERERRNO("TcpSocket::recvdatabuf read failed. fd = %d. %s:%d", m_socketfd, __POSITION__);
                return -1;
            }
        } else if (cnt == 0) {
            TRACERERRNO ("cnt == 0. read failed.\n");
            return -1; // 方便上层调用
//          break;                              // EOF shutdown() / close() ?
        }

        TRACER("test: read %d bytes sucess\n", cnt);

        pbuf += cnt;
        m_pdatabuf->tail += cnt;
    }
    
    TRACER("end recv data from %d, socketbufsize is %d\n", m_socketfd, sn);


    // 当数据填充到buf尾部时，尝试移动数据到buf头部
    if (m_pdatabuf->tail == m_bufsize) {
        if (m_pdatabuf->head == 0) {
            TRACERERRNO("TcpSocket::recvdatabuf sockbuf filled.");
        } else {
            int32_t size = m_pdatabuf->tail - m_pdatabuf->head;
            memmove(m_pdatabuf->buffer, m_pdatabuf->buffer + m_pdatabuf->head, size);
            m_pdatabuf->head = 0;
            m_pdatabuf->tail = size;
        }
    }
    return sn;
}

int32_t TcpSocket::RecvData(void* usrbuf, uint32_t size)
{
    TRACER("star recv %d byte data from %d\n", size, m_socketfd);
    
    int32_t nleft = size;
    int32_t cnt;
    char* pbuf = (char*)usrbuf;
    TRACER("sleep 2s for debug");
    sleep(2);

    while (nleft > 0) {
        TRACER("start call recvdatabuf(%p, %d)\n", pbuf, nleft);

        cnt = recvdatabuf(pbuf, nleft);
        
        TRACER("end call recvdatabuf(%p, %d)\n", pbuf, nleft);
        
        // recvdatabuf 错误和结束时都返回 -1
        if (cnt < 0) {
            // recvdatabuf 当 read 返回 0 时，返回 -1， 还需要考虑下
            TRACERERRNO("TcpSocket::RecvData failed cnt = %d. %s : %d\n", cnt, __POSITION__);
            return -2; // 表示关闭
        } else if (cnt == 0 && errno == EAGAIN) { // 只有当无数据，EAGAIN 才会返回0
            // recvdatabuf 当EAFAIN时返回零，这个和EOF相同了，之后再重新设计返回值，现在在这里判断下
            continue;
        }
        nleft -= cnt;
        pbuf += cnt;
    }
    TRACER("hade recved %d byte data from %d\n", size - nleft, m_socketfd);
    return size - nleft;
}

int32_t TcpSocket::SendData(void* usrbuf, uint32_t size)
{
    uint32_t nleft = size;
    int32_t nw = 0;
    char* pbuf = (char*)usrbuf;
    TRACER("start to send data to %d\n", m_socketfd);
    // 写入 size 字节的数据
    while (nleft > 0) {
        if ((nw = write(m_socketfd, pbuf, nleft)) <= 0) {
            if (EINTR == errno || EAGAIN == errno) {
                continue;
            } else {
                TRACERERRNO("TcpSocket::SendData failed! %s:%d", __FILE__, __LINE__);
            //    close(m_socketfd);  // 是否交给析构函数
                break;
            }
        }

        pbuf += nw;
        nleft -= nw;
    }
    TRACER("send %d byte data to %d end\n", size - nleft, m_socketfd);
    return nleft;
}


int32_t TcpSocket::OpenAsClient(const char* hostname, int16_t port)
{
    int clientfd = -1;
    sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, hostname, &serveraddr.sin_addr) < 0) {
        TRACER("inet_pton failed addr = %s", hostname);
        return -1;
    }
    
    serveraddr.sin_port = htons(port);

    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        TRACERERRNO("TcpSocket::OpenAsClient socket failed");
        return -1;
    }

    if (connect(clientfd, (sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        TRACER("connect failed test\n");
        return -1;
    }
    
    socklen_t len = sizeof(m_sockaddr);
    if (getsockname(clientfd, (sockaddr*)&m_sockaddr, &len) < 0) {
        TRACER("getsockname failed\n");
    }

    TRACER("client addr is %d : %d\n", m_sockaddr.sin_addr.s_addr, (int)(m_sockaddr.sin_port));

    setnonblock(clientfd);
    
    m_socketfd = clientfd;
    return clientfd;
}

int32_t TcpSocket::OpenAsServer(int16_t port, const char* hostname) 
{
    int listenfd, optval = 1;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        TRACERERRNO("TcpSocket::OpenAsServer socket failed. %s:%d", __FILE__, __LINE__);
        return -1;
    }
    this->TcpSocketInit(listenfd);
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0) {
        TRACERERRNO("TcpSocket::OpenAsServer setsockopt resueaddr failed. %s:%d", __FILE__, __LINE__);
    }

    bzero(&m_sockaddr, sizeof(m_sockaddr));
    if (hostname) {
        if (inet_pton(AF_INET, hostname, (sockaddr*)&m_sockaddr.sin_addr) < 0) {
            TRACER("TcpSocket::OpenAsServer inet_pton failed. try INADDR_ANY. %s:%d", __FILE__, __LINE__);
            m_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        } 
    } else {
        m_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    m_sockaddr.sin_family = AF_INET;
    m_sockaddr.sin_port = htons(port);

    if (bind(listenfd, (sockaddr*)&m_sockaddr, sizeof(m_sockaddr)) < 0) {
        TRACERERRNO("TcpSocket::OpenAsServer bind failed. %s:%d", __FILE__, __LINE__);
        return -1;
    }

    if (listen(listenfd, 256) < 0) {
        TRACERERRNO("TcpSocket::OpenAsServer listen failed. %s:%d", __FILE__, __LINE__);
        return -1;
    }

    if (setnonblock(listenfd)) {
        return -1;
    }

    m_socketfd = listenfd;
    return listenfd;
}

int32_t  TcpSocket::Accept(int32_t fd, sockaddr_in* ps, socklen_t* len)
{
    int32_t acceptfd = accept(fd, (struct sockaddr*)ps, len);
    
    if (acceptfd <= 0) {
        TRACERERRNO("accept client failed.");
        return -1;
    }

    if (setnonblock(acceptfd)) {
        return -1;
    }

    int on = 1;
    setsockopt(acceptfd, IPPROTO_TCP, TCP_NODELAY, (const void*)&on, sizeof(on));
    
    return acceptfd;
}

int32_t TcpSocket::setnonblock(int32_t fd) 
{
    int mode = 1;  // 1 非阻塞， 0 阻塞
    if (ioctl(fd, FIONBIO, &mode)) {
        int flag;
        if (((flag = fcntl(fd, F_GETFL, 0)) < 0) && (fcntl(fd, F_SETFL, flag | O_NONBLOCK) < 0)) {
            TRACER("set sock nonblock faild\n");
        //     close(fd); // 析构管理
            return -1;
        }
    }
    return 0;
}