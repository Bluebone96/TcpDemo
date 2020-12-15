#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>

// #include "kbuffer.hpp"
#include "message.h"

#define SOCKET_EAGAIN  0 // 无数据可读，非阻塞下不是错误
#define SOCKET_ERROR_CLOSE -1
#define SOCKET_ERROR_BUFF_FULL -2

#define SOCKET_ERROR_UNKNOWN -10

class tcp_socket {
public:
    tcp_socket();
    ~tcp_socket();

    tcp_socket(const tcp_socket&) = delete;

    tcp_socket& operator=(const tcp_socket&) = delete;

    int8_t tcp_init_buf(uint32_t _bufsz = (1024 * 16));

    // 创建一个socket 3种方式， connect listen accept
    int32_t tcp_connect(const char* hostname, int16_t port);

    int32_t tcp_listen(const char* hostname, int16_t port);

    int32_t tcp_accept(int32_t _listenfd);


    int32_t tcp_recv(uint8_t *_usrbuf, uint32_t _len);

    int32_t tcp_send(const uint8_t *_usrbuf, uint32_t _len);
    
    // 写无缓冲，提供静态方法接口
    static int32_t tcp_send(uint32_t fd, const uint8_t *_usrbuf, uint32_t _len);

    int32_t getfd() { return m_socketfd; }
    
private:
    int32_t recv_by_len(uint8_t *_usrbuf, uint32_t _len);

    // 成功总是返回 SOCKET_ERROR_EAGAIN（将内核缓冲区的数据读干净），出错返回错误码
    int32_t recv_full();
    
    int32_t m_socketfd;
    struct sockaddr_in m_sockaddr;

    // 环形缓冲区
    // volatile uint32_t m_in; // todo 不确定是否有效，或采用原子操作
    // volatile uint32_t m_out;
    uint32_t m_in; 
    uint32_t m_out;
    uint32_t m_size;
    uint8_t *m_buffer;
};



#endif
