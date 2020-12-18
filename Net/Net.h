#ifndef _NET_H_
#define _NET_H_

#include "tcp_socket.h"
#include "Epoll.h"
#include "message.h"

#include <unordered_map>
#include <memory>

// extern msg_queue g_recv_queue;
// extern msg_queue g_send_queue;

class Net {
public:
    Net();
    ~Net();

    int32_t net_init(msg_queue* _recvq, msg_queue* _sendq);

    int32_t product_msg();
    int32_t consume_msg();
    
    static int32_t recvmsg(std::shared_ptr<tcp_socket>&, message*);
    static int32_t sendmsg(std::shared_ptr<tcp_socket>&, message*);

    int32_t net_listen(const char* _ip, uint32_t _port);
    int32_t net_connect(const char* _ip, uint32_t _port);

private:
    Epoll m_epoll;
    tcp_socket m_listen_socket;
    int32_t m_listenfd;

    std::unordered_map<uint32_t, std::shared_ptr<tcp_socket>> m_connections;

    msg_queue* g_recv_queue;
    msg_queue* g_send_queue;
};



#endif
