#include <unistd.h>

#include "../Common/basetype.h"
#include "../Net/tcp_socket.h"
#include "gate_server.h"


gate_server::gate_server()
{

}

gate_server::~gate_server()
{

}

int8_t gate_server::init_gate()
{
    return 0;
}

int8_t gate_server::run()
{
    message *msg = nullptr;
    for (;;) {
        msg = g_recv_queue.dequeue();
        if (msg == nullptr) {
            sleep(1);
        }

        if (msg->m_from == g_connet_server[GAME_SERVER]) {
            tcp_socket::tcp_send(m_usrfd[msg->m_head.m_usrID], msg->m_data, msg->m_head.m_len + MSG_HEAD_SIZE);
        } else {
            tcp_socket::tcp_send(g_connet_server[GAME_SERVER], msg->m_data, msg->m_head.m_len + MSG_HEAD_SIZE);
        }
        msg->setinvalid();
    }
}