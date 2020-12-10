#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>

#include "kbuffer.hpp"
#include "message.h"

class tcp_socket {
public:
    tcp_socket();
    ~tcp_socket();

    tcp_socket(const tcp_socket&) = delete;

    tcp_socket& operator=(const tcp_socket&) = delete;

    int8_t tcp_init(int32_t fd, uint32_t _bufsz = (1024 * 512));


    int32_t tcp_connect(const char* hostname, int16_t port);

    int32_t tcp_listen(const char* hostname, int16_t port);

    int32_t tcp_accept(sockaddr_in* ps = nullptr, socklen_t* len = nullptr);

    int32_t tcp_recv(uint8_t *_usrbuf, uint32_t _len);

    int32_t tcp_send(const uint8_t *_usrbuf, uint32_t _len);

    static int32_t tcp_send(uint32_t fd, const uint8_t *_usrbuf, uint32_t _len);

    int32_t getfd() { return m_socketfd; }
    
    // // 不要使用, 测试机器人用的
    // int32_t tcp_close() { return close(m_socketfd); }
private:
    int32_t recv_by_len(uint8_t *_usrbuf, uint32_t _len);
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
