#include <unistd.h>

#include "../Common/basetype.h"
#include "../Common/log.h"
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
            usleep(100 * 1000);;
            continue;
        }

        switch (msg->m_head.m_type) {
            case USERLOGIN:
                TRACER("new player login usrid is %d, fd is %d\n", msg->m_head.m_usrID, msg->m_from);
                m_usrfd[msg->m_head.m_usrID] = msg->m_from;    // 第一次登录
                break;
            case SERVER_INFO:
                {
                    Proto::Unity::ServerInfo server;
                    msg->decode_pb(server);
                    g_connet_server[server.m_type()] = msg->m_from;
                }
                break;
            default:
                if (msg->m_from == g_connet_server[GAME_SERVER]) {
                    if (msg->m_head.m_type == USERSYNC) {
                        // 发给每个 客户端
                        for (auto iter =  m_usrfd.begin(); iter != m_usrfd.end();) {
                            // 当发送失败，应该为客户端断开连接
                            if (tcp_socket::tcp_send(iter->second, msg->m_data, msg->m_head.m_len + MSG_HEAD_SIZE) < 0) {
                                m_usrfd.erase(iter++);
                            } else {
                                ++iter;
                            }
                        }
                    } else {
                        // item operation
                        TRACER_DEBUG("item msgtype\n");
                        if (tcp_socket::tcp_send(m_usrfd[msg->m_head.m_usrID], msg->m_data, msg->m_head.m_len + MSG_HEAD_SIZE) < 0) {
                            TRACER_ERROR("send msg to usrid %d fd %d failed", msg->m_head.m_usrID, m_usrfd[msg->m_head.m_usrID]);
                        }
                    }
                } else {
                    tcp_socket::tcp_send(g_connet_server[GAME_SERVER], msg->m_data, msg->m_head.m_len + MSG_HEAD_SIZE);
                }
                break;
        }

        msg->m_flag = msg_flags::INACTIVE;
    }
}