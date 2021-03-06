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



int32_t tcp_socket::tcp_setsockopt()
{
    // 如果在发送数据的时，希望不经历由系统缓冲区到socket缓冲区的拷贝而影响程序的性能(UDP 直接发送)：
    // sendbuf 设置为0，fd为非阻塞，需等待发送全部成功，目前不能做到性能优化：
    // int nZero = 0;
    // if (setsockopt(m_socketfd, SOL_SOCKET, SO_SNDBUF, &nZero, sizeof(nZero)) < 0) {
    //     TRACER_ERROR("tcp_setsockopt  SO_SENDBUF falied, fd = %d\n", m_socketfd);
    // }
    // 是tcp的滑动窗口协议，还是需要的
    // int nZero = 0;
    // if (setsockopt(m_socketfd, SOL_SOCKET, SO_RCVBUF, &nZero, sizeof(nZero)) < 0) {
    //     TRACER_ERROR("tcp_setsockopt  SO_RCVBUF falied, fd = %d\n", m_socketfd);
    // }
    // 内核缓冲区一有数据就发送。
    int flag = 1;
    if (setsockopt(m_socketfd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)) < 0) {
        TRACER_ERROR("tcp_setsockopt TCP_NODELAY falied. fd = %d\n", m_socketfd);
    }

    int mode = 1;  // 1 非阻塞， 0 阻塞
    if (ioctl(m_socketfd, FIONBIO, &mode)) {
        int flag;
        if (((flag = fcntl(m_socketfd, F_GETFL, 0)) < 0) && (fcntl(m_socketfd, F_SETFL, flag | O_NONBLOCK) < 0)) {
            TRACER_ERROR("set sock nonblock faild, fd = %d\n", m_socketfd);
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


int32_t tcp_socket::tcp_init_buf(uint32_t _bufsz)
{
    m_size = roundup_pow_of_two(_bufsz);
    try
    {
        m_buffer = new uint8_t[m_size];
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return m_size;
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

    m_socketfd = clientfd;
    
    if (tcp_setsockopt() < 0) {
        TRACER_ERROR("tcp_setsockopt falied\n");
        return -1;
    }
    
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

    m_socketfd = listenfd;

    if (tcp_setsockopt() < 0) {
        TRACER_ERROR("tcp_setsockopt falied\n");
        return -1;
    }

    return listenfd;
}


int32_t tcp_socket::tcp_accept(int32_t _listenfd)
{
    socklen_t len;
    int32_t acceptfd = accept(_listenfd, (struct sockaddr*)&m_sockaddr, &len);
    
    if (acceptfd <= 0) {
        // 由于改为非阻塞，并且epoll ET 模式， 会多次accept, 错误日志改为debug等级
        TRACER_DEBUG("accept client failed.");
        return -1;
    }

    m_socketfd = acceptfd;

    if (tcp_setsockopt() < 0) {
        TRACER_ERROR("tcp_setsockopt falied\n");
        return -1;
    }

    return acceptfd;
}


int32_t tcp_socket::tcp_recv(uint8_t *_usrbuf, uint32_t _length)
{
    int ret = 0;
    if (uint32_t(m_in - m_out) < _length) {
        ret = recv_full();
        switch (ret) {
            case SOCKET_EAGAIN:
                // 正常接收完内核全部的数据
                break;
            case SOCKET_ERROR_BUFF_FULL:
                // 缓冲区满了
                break;
            default:
                TRACER_ERROR("bug bug bug fd = %d, m_in = %d, m_out = %d, _length = %d\n", m_socketfd, m_in, m_out, _length);
                return ret;
        }
    }
    // 将 while 改为 2次if, 如果还是小于则说明展时没有数据可读
    if (m_in - m_out < _length) {
        TRACER_DEBUG("m_in = %d, m_out = %d, len = %d\n", m_in, m_out, _length);
        return SOCKET_EAGAIN;
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
                // break;
                return SOCKET_EAGAIN; 
            }
            return SOCKET_ERROR_UNKNOWN;
        } else if (cnt == 0) {
                TRACERERRNO("tcpsocket::tcp_recv read failed. fd = %d.\n", m_socketfd);
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
        // should not be here。 该函数总是在缓冲区数据不够时才会调用，不应该缓冲区满
        // 读取 0 个字节，会立刻返回 0， 代码中会判定为到达文件尾部，或socket关闭
        TRACER_ERROR("DEBUG buff is full!! m_in = %d, m_out = %d\n", m_in, m_out);
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
            } else if (errno == EAGAIN) {
                //设置了O_NODELAY
                usleep(20 * 1000);
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





