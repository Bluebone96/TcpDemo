#ifndef _NET_H_
#define _NET_H_

#include "tcp_socket.h"
#include "Epoll.h"
#include "message.h"

#include <map>
#include <memory>

extern msg_queue g_recv_queue;
extern msg_queue g_send_queue;

class Net {
public:
    Net();
    ~Net();

    int8_t init(const char* _ip, uint32_t _port);

    int8_t product_msg();
    int8_t consume_msg();
    
    static int8_t recvmsg(std::shared_ptr<tcp_socket>&, message&);
    static int8_t sendmsg(std::shared_ptr<tcp_socket>&, message&);

    int8_t connect(const char* _ip, uint32_t _port);
public:
    Epoll m_epoll;
    tcp_socket m_listenfd;
    std::map<uint32_t, std::shared_ptr<tcp_socket>> m_connections;
};



#endif
