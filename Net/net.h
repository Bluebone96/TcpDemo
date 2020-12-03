#ifndef _NET_H_
#define _NET_H_

#include "tcp_socket.h"
#include "Epoll.h"
#include "message.h"
#include "../config/loadconfig.hpp"

#include <map>
#include <memory>

class net {
public:
    net();
    ~net();

    int8_t run();

public:
    msg_queue* m_pmsg;

    Epoll m_epoll;
    tcp_socket m_listenfd;
    tcp_socket m_connectfd;
    std::map<uint32_t, std::shared_ptr<tcp_socket>> m_clients;
};



#endif
