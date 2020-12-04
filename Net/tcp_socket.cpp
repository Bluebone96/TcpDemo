#include <cstdlib>
#include <arpa/inet.h>

#include "tcp_socket.h"
#include "../Common/base.h"
#include "../Common/log.h"
#include "byte_swap.hpp"

#ifndef MIN
#define MIN(A, B) (((A) < (B)) ? (A) : (B))
#endif


// static inline bool is_pow_of_two (uint32_t _n)
// {
//     return (_n != 0 && ((_n & (_n - 1)) == 0));
// }


// static inline uint32_t roundup_pow_of_two(uint32_t _s)
// {
//     if (!is_pow_of_two(_s)) {
//         int p = 0;
//         for (int i = _s; i != 0; i >>= 1) {
//             ++p;
//         }
//         return (1u << p);
//     }
//     return _s;
// }



inline static int32_t setnonblock(int32_t fd) 
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

tcp_socket::tcp_socket() 
    : m_socketfd(-1), m_in(0), m_out(0), m_size(0), m_buffer(nullptr)
{

}

tcp_socket::~tcp_socket()
{
    TRACER("tcp_socket dtor!\n");
    delete[] m_buffer;
    close(m_socketfd);
}


int8_t tcp_socket::tcp_init(int32_t _fd, uint32_t _bufsz)
{
    m_socketfd = _fd;

    m_size = roundup_pow_of_two(_bufsz);
    try
    {
        m_buffer = new uint8_t[m_size];
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}


uint32_t tcp_socket::tcp_connect(const char* hostname, int16_t port)
{
    int clientfd = -1;

    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        TRACERERRNO("TcpSocket::OpenAsClient socket failed");
        return -1;
    }

    socklen_t len = sizeof(m_sockaddr);
    if (getsockname(clientfd, (sockaddr*)&m_sockaddr, &len) < 0) {
        TRACER("getsockname failed\n");
    }

    TRACER("client addr is %d : %d\n", m_sockaddr.sin_addr.s_addr, (int)(m_sockaddr.sin_port));

    sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, hostname, &serveraddr.sin_addr) < 0) {
        TRACER("inet_pton failed addr = %s", hostname);
        return -1;
    }
    
    serveraddr.sin_port = hton_16(port);

    if (connect(clientfd, (sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        TRACER("connect failed test\n");
        return -1;
    }
    
    setnonblock(clientfd);
    
    m_socketfd = clientfd;
    return clientfd;
}

uint32_t tcp_socket::tcp_listen(const char* hostname, int16_t port)
{
    int32_t listenfd = -1;
    int32_t  optval = 1;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        TRACERERRNO("TcpSocket::OpenAsServer socket failed. %s:%d", __FILE__, __LINE__);
        return -1;
    }
    

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0) {
        TRACERERRNO("TcpSocket::OpenAsServer setsockopt resueaddr failed. %s:%d", __FILE__, __LINE__);
    }

    bzero(&m_sockaddr, sizeof(m_sockaddr));
    if (hostname) {
        if (inet_pton(AF_INET, hostname, (sockaddr*)&m_sockaddr.sin_addr) < 0) {
            TRACER("TcpSocket::OpenAsServer inet_pton failed. try INADDR_ANY. %s:%d", __FILE__, __LINE__);
            m_sockaddr.sin_addr.s_addr = hton_32(INADDR_ANY);
        } 
    } else {
        m_sockaddr.sin_addr.s_addr = hton_32(INADDR_ANY);
    }
    m_sockaddr.sin_family = AF_INET;
    m_sockaddr.sin_port = hton_16(port);

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


uint32_t tcp_socket::tcp_accept(sockaddr_in* ps, socklen_t* len)
{
    int32_t acceptfd = accept(m_socketfd, (struct sockaddr*)ps, len);
    
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


uint32_t tcp_socket::tcp_recv(uint8_t *_usrbuf, uint32_t _length)
{

    while (m_in - m_out < _length) {
        if (recv() < 0) {
            return -1;
        }
    }
    _length = MIN(_length, m_in - m_out);
    

    uint32_t len = MIN(_length, m_size -(m_out & (m_size - 1)));
    memcpy(_usrbuf, m_buffer + (m_out & (m_size - 1)), len);

    memcpy(_usrbuf + len, m_buffer, _length - len);

    m_out += _length;

    return _length;
}


uint32_t tcp_socket::recv(uint8_t *_usrbuf, uint32_t _len)
{
    int32_t cnt = 0;

    while (_len) {
        cnt = read(m_socketfd, _usrbuf, _len);
        if (cnt <= 0) {
            if (errno == EINTR || errno == EAGAIN) {
                return 0;
            } else {
                TRACERERRNO("tcpsocket::tcp_recv read failed. fd = %d. %s:%d", 
                            m_socketfd, __POSITION__);
                return -1;
            } 
        }
        _len -= cnt;
        _usrbuf += cnt;
        m_in += cnt;
    }

    return 0;
}



uint32_t tcp_socket::recv()
{
    uint32_t length = m_size - m_in + m_out;

    uint32_t left = MIN(length, m_size - (m_in & (m_size - 1)));
    uint8_t *pbuf = m_buffer + (m_in & (m_size - 1));
    
    int8_t ret = 0;
    if ((ret = recv(pbuf, left)) < 0) {
        return ret;
    }
    
    left = m_size - m_in + m_out;
    pbuf = m_buffer;

    if ((ret = recv(pbuf, left)) < 0) {
        return ret;
    }
    return 0;
}

uint32_t tcp_socket::tcp_send(const uint8_t *_usrbuf, uint32_t _len)
{
    return tcp_send(m_socketfd, _usrbuf, _len);
}

uint32_t tcp_socket::tcp_send(uint32_t _fd, const uint8_t *_usrbuf, uint32_t _len)
{
    uint32_t nleft = _len;
    int32_t nw = 0;
    char* pbuf = (char*)_usrbuf;

    // 写入 size 字节的数据
    while (nleft > 0) {
        if ((nw = write(_fd, pbuf, nleft)) <= 0) {
            if (EINTR == errno || EAGAIN == errno) {
                continue;
            } else {
                TRACERERRNO("TcpSocket::SendData failed! %s:%d", __FILE__, __LINE__);
                break;
            }
        }

        pbuf += nw;
        nleft -= nw;
    }

    return nleft;
}





