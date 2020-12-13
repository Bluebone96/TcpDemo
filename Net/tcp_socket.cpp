#include <cstdlib>
#include <arpa/inet.h>

#include "tcp_socket.h"
#include "../Common/base.h"
#include "../Common/log.h"
#include "byte_swap.hpp"

#ifndef MIN
#define MIN(A, B) (((A) < (B)) ? (A) : (B))
#endif


static inline bool is_pow_of_two (uint32_t _n)
{
    return (_n != 0 && ((_n & (_n - 1)) == 0));
}


static inline uint32_t roundup_pow_of_two(uint32_t _s)
{
    if (!is_pow_of_two(_s)) {
        int p = 0;
        for (int i = _s; i != 0; i >>= 1) {
            ++p;
        }
        return (1u << p);
    }
    return _s;
}



inline static int32_t setnonblock(int32_t fd) 
{
    int mode = 1;  // 1 非阻塞， 0 阻塞
    if (ioctl(fd, FIONBIO, &mode)) {
        int flag;
        if (((flag = fcntl(fd, F_GETFL, 0)) < 0) && (fcntl(fd, F_SETFL, flag | O_NONBLOCK) < 0)) {
            TRACER_ERROR("set sock nonblock faild\n");
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
    TRACER_DEBUG("tcp_socket dtor!\n");
    delete[] m_buffer;
    close(m_socketfd);
}


int8_t tcp_socket::tcp_init_buf(uint32_t _bufsz)
{
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


int32_t tcp_socket::tcp_connect(const char* hostname, int16_t port)
{
    int clientfd = -1;

    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        TRACERERRNO("TcpSocket::OpenAsClient socket failed");
        return -1;
    }


    sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, hostname, &serveraddr.sin_addr) < 0) {
        TRACER_ERROR("inet_pton failed addr = %s", hostname);
        return -1;
    }
    
    serveraddr.sin_port = hton_16(port);

    if (connect(clientfd, (sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        TRACER_ERROR("connect failed ip is %s, port is %d\n", hostname, port);
        return -1;
    }
    
    socklen_t len = sizeof(m_sockaddr);
    if (getsockname(clientfd, (sockaddr*)&m_sockaddr, &len) < 0) {
        TRACER_ERROR("getsockname failed\n");
    }
    TRACER_DEBUG("client addr is %d : %d\n", m_sockaddr.sin_addr.s_addr, (int)(m_sockaddr.sin_port));
    setnonblock(clientfd);
    
    m_socketfd = clientfd;
    return clientfd;
}

int32_t tcp_socket::tcp_listen(const char* hostname, int16_t port)
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
            TRACER_ERROR("TcpSocket::OpenAsServer inet_pton failed. try INADDR_ANY. %s:%d", __FILE__, __LINE__);
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


int32_t tcp_socket::tcp_accept(int32_t _listenfd)
{
    socklen_t len;
    int32_t acceptfd = accept(_listenfd, (struct sockaddr*)&m_sockaddr, &len);
    
    if (acceptfd <= 0) {
        TRACERERRNO("accept client failed.");
        return -1;
    }

    if (setnonblock(acceptfd)) {
        return -1;
    }

    int on = 1;
    setsockopt(acceptfd, IPPROTO_TCP, TCP_NODELAY, (const void*)&on, sizeof(on));

    m_socketfd = acceptfd;
    return acceptfd;
}


int32_t tcp_socket::tcp_recv(uint8_t *_usrbuf, uint32_t _length)
{

    // if (_length > 1024) {
    //     TRACER_ERROR("fd = %d, bug bug bug m_in = %d, m_out = %d, _length = %d\n", m_socketfd, m_in, m_out, _length);
    //     sleep(50);
    // }

    int ret = 0;
    if (m_in - m_out < _length) {
        if ((ret = recv_full()) < 0) {
            return ret;
        }
        TRACER_DEBUG("bug bug bug fd = %d, m_in = %d, m_out = %d, _length = %d\n", m_socketfd, m_in, m_out, _length);
    }

    // 将 while 改为 2次if, 如果还是小于则说明展时没有数据可读
    if (m_in - m_out < _length) {
        return SOCKET_ERROR_EAGAIN;
    }

    uint32_t len = MIN(_length, m_size -(m_out & (m_size - 1)));
    memcpy(_usrbuf, m_buffer + (m_out & (m_size - 1)), len);

    memcpy(_usrbuf + len, m_buffer, _length - len);

    m_out += _length;

    TRACER_DEBUG("recv %d byte data\n", _length);
    return _length;
}


int32_t tcp_socket::recv_by_len(uint8_t *_usrbuf, uint32_t _len)
{
    int32_t cnt = 0;
    int32_t left = _len;
    while (left) {
        cnt = read(m_socketfd, _usrbuf, left);
        if (cnt < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                // return SOCKET_ERROR_EAGAIN; // 非阻塞永远返回的都是这个
                break;
            }
            return SOCKET_ERROR_UNKNOWN;
        } else if (cnt == 0) {
                TRACERERRNO("tcpsocket::tcp_recv read failed. fd = %d.\n %s:%d", 
                            m_socketfd, __POSITION__);
                return SOCKET_ERROR_CLOSE;
        }

        m_in += cnt;
        left -= cnt;
        _usrbuf += cnt;
    }

    return _len - left;
}



int32_t tcp_socket::recv_full()
{
    uint32_t length = m_size - m_in + m_out;
    if (length == 0) {
        // should not be here
        TRACER_ERROR("bug buff is full!!\n");
        return SOCKET_ERROR_BUFF_FULL;
    }
    uint32_t left = MIN(length, m_size - (m_in & (m_size - 1)));
    uint8_t *pbuf = m_buffer + (m_in & (m_size - 1));
    
    int32_t ret = recv_by_len(pbuf, left);
    if (ret == static_cast<int32_t>(left)) {
        return recv_full();
    } else {
        return ret;
    }

    return 0;
}

int32_t tcp_socket::tcp_send(const uint8_t *_usrbuf, uint32_t _len)
{
    return tcp_send(m_socketfd, _usrbuf, _len);
}

int32_t tcp_socket::tcp_send(uint32_t _fd, const uint8_t *_usrbuf, uint32_t _len)
{
    TRACER_DEBUG("send data to fd %d, len is %d\n", _fd, _len);

    uint32_t nleft = _len;
    int32_t nw = 0;
    char* pbuf = (char*)_usrbuf;

    // 写入 size 字节的数据
    while (nleft > 0) {
        if ((nw = write(_fd, pbuf, nleft)) <= 0) {
            if (EINTR == errno || EAGAIN == errno) {
                continue;
            } else {
                TRACERERRNO("tcp_socket::tcp_send failed! %s:%d\n", __FILE__, __LINE__); // 可能是客户端直接断开连接
                return -1;
            }
        }

        pbuf += nw;
        nleft -= nw;
    }
    TRACER_DEBUG("send data success left should be 0: %d\n", nleft);
    return 0; // always 0
}





