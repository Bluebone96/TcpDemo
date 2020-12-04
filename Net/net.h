#ifndef _NET_H_
#define _NET_H_

#include "tcp_socket.h"
#include "Epoll.h"
#include "message.h"
#include "../Config/loadconfig.hpp"

#include <map>
#include <memory>

extern msg_queue g_recv_queue;
extern msg_queue g_send_queue;
extern std::map<uint32_t, std::shared_ptr<tcp_socket>> g_connections;

class net {
public:
    net();
    ~net();

    int8_t recvmsg();
    int8_t sendmsg();
    
    static int8_t recvmsg(std::shared_ptr<tcp_socket>&, message&);
    static int8_t sendmsg(std::shared_ptr<tcp_socket>&, message&);

public:
    Epoll m_epoll;
    tcp_socket m_listenfd;
    tcp_socket m_connectfd;
    std::map<uint32_t, std::shared_ptr<tcp_socket>> m_connections;
};



#endif
