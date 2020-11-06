#include <cstdlib>
#include <arpa/inet.h>

#include "TcpSocket.h"
#include "log.h"

TcpSocket::TcpSocket(uint32_t sz) : m_socketfd(-1), m_pdatabuf(nullptr)
{
    if (sz > SOCKETBUFLEN) {
        sz = SOCKETBUFLEN;
    }

    m_bufsize = sizeof(SockBuf) + sz;
    m_pdatabuf = (SockBuf*)malloc(m_bufsize);
    bzero(m_pdatabuf, m_bufsize);

}

int32_t TcpSocket::Init(int32_t fd)
{
    m_socketfd = fd;
    bzero(m_pdatabuf, m_bufsize);
}

int32_t TcpSocket::recvdata(void* usrbuf, uint32_t size)
{
    int32_t head = m_pdatabuf->head;
    int32_t tail = m_pdatabuf->tail;
    uint32_t nleft = (size - tail, m_bufsize - sizeof(SockBuf));
    int32_t cnt = 0;
    while (nleft > 0) {
        cnt = read(m_socketfd, m_pdatabuf->buffer + tail, nleft);
        if (cnt < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            } else {
                TRACERERRNO("TcpSocket::recvdata read failed. fd = %d.", m_socketfd);
                return -1;
            }
        } else if (cnt == 0) {
            break;
        }

        tail += cnt;
        nleft -= cnt;
    };
    
    uint32_t sn = tail - head;
    if (nleft <= 0) {
        if (head == 0) {
            TRACERERRNO("TcpSocket::recvdata sockbuf filled.");
        } else {
            memmove(m_pdatabuf->buffer, m_pdatabuf->buffer + head, sn);
            m_pdatabuf->head = 0;
            m_pdatabuf->tail = sn;
        }
    }

    sn = sn < size ? sn : size;
    
    mempcpy(usrbuf, m_pdatabuf, sn);
    m_pdatabuf->head += sn;
    return sn;
}

int32_t TcpSocket::RecvData(void* usrbuf, uint32_t size)
{
    uint32_t nleft = size;
    int32_t cnt;
    char* pbuf = (char*)usrbuf;
    while (nleft > 0) {
        cnt = recvdata(pbuf, nleft);
        if (cnt < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                TRACERERRNO("TcpSocket::RecvData failed.");
                return -1;
            }
        } else if (cnt == 0) {
            break;
        }
        nleft -= cnt;
        pbuf += cnt;
    }

    return size - nleft;
}

int32_t TcpSocket::SendData(void* usrbuf, uint32_t size)
{
    uint32_t nleft = size;
    int32_t nw = 0;
    char* bufp = (char*)usrbuf;

    while (nleft > 0) {
        if ((nw = write(GetFd(), usrbuf, nleft)) <= 0) {
            if (EINTR == errno || EAGAIN == errno) {
                continue;
            } else {
                TRACERERRNO("TcpSocket::SendData failed! %s:%d", __FILE__, __LINE__);
                close(GetFd());
                break;
            }
        }

        bufp += nw;
        nleft -= nw;
    }

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
    socklen_t len = sizeof(m_sockaddr);
    if (getsockname(clientfd, (sockaddr*)&m_sockaddr, &len) < 0) {
        TRACER("getsockname failed\n");
    }

    if (connect(clientfd, (sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        TRACER("connect failed test\n");
        return -1;
    }
    
    return clientfd;
}

int32_t TcpSocket::OpenAsServer(int16_t port, const char* hostname) 
{
    int listenfd, optval = 1;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        TRACERERRNO("TcpSocket::OpenAsServer socket failed. %s:%d", __FILE__, __LINE__);
        return -1;
    }
    this->Init(listenfd);
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

    // SET_NON_BLOCK(listenfd);
    return listenfd;
}

int32_t  TcpSocket::Accept(int32_t fd, sockaddr_in* ps, socklen_t* len)
{
    int32_t acceptfd = accept(fd, (struct sockaddr*)ps, len);
    
    if (acceptfd <= 0) {
        TRACERERRNO("accept client failed.");
        return -1;
    }

    int flags = 1;

    if (ioctl(acceptfd, FIONBIO, &flags) && ((flags = fcntl(acceptfd, F_GETFL, 0)) < 0 || fcntl(acceptfd, F_SETFL, flags | O_NONBLOCK)))
    {
        TRACER("Set acceptfd socket O_NONBLOCK failed");
        close(acceptfd);
        return -1;
    }

    int on = 1;
    setsockopt(acceptfd, IPPROTO_TCP, TCP_NODELAY, (const void*)&on, sizeof(on));
    return acceptfd;
}