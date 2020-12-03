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

    int8_t tcp_init(uint32_t fd, uint32_t _bufsz = (1024 * 1024));


    uint32_t tcp_connect(const char* hostname, int16_t port);

    uint32_t tcp_listen(const char* hostname, int16_t port);

    uint32_t tcp_accept(sockaddr_in* ps = nullptr, socklen_t* len = nullptr);

    uint32_t tcp_recv(uint8_t *_usrbuf, uint32_t _len);

    uint32_t tcp_recv(message* );

    uint32_t tcp_send(const uint8_t *_usrbuf, uint32_t _len);

    uint32_t getfd() { return m_socketfd; }

private:
    uint32_t recv(uint8_t *_usrbuf, uint32_t _len);
    uint32_t recv();
    
    uint32_t m_socketfd;
    struct sockaddr_in m_sockaddr;

    // 环形缓冲区
    volatile uint32_t m_in; // todo 不确定是否有效，或采用原子操作
    volatile uint32_t m_out;
    uint32_t m_size;
    uint8_t *m_buffer;
};



#endif
