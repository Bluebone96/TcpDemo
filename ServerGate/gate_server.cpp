#include <unistd.h>
#include <thread>


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
            usleep(50 * 1000);;
            continue;
        }

        switch (msg->m_head.m_type) {
            case USERLOGIN:
                TRACER("new player login usrid is %d, fd is %d\n", msg->m_head.m_usrID, msg->m_from);
                m_usrfd[msg->m_head.m_usrID] = msg->m_from;    // 第一次登录
                // m_clientsfd.emplace_back(client_info(msg->m_head.m_usrID, msg->m_from));
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

                    switch (msg->m_head.m_type)
                    {
                        case USEREXIT: // 服务器主动退出
                            for (auto iter = m_clientsfd.begin(), end = m_clientsfd.end(); iter != end; ++iter) {
                                if (iter->usrid == msg->m_head.m_usrID) {
                                    m_clientsfd.erase(iter);
                                    break;
                                }
                            }
                            m_usrfd.erase(msg->m_head.m_usrID);
                            broadcaster(msg);
                            break;
                        case USERSYNC:
                            broadcaster(msg);
                            break;
                        case ITEMEVENT:
                            // item operation
                            if (tcp_socket::tcp_send(m_usrfd[msg->m_head.m_usrID], msg->m_data, msg->m_head.m_len + MSG_HEAD_SIZE) < 0) {
                               TRACER_ERROR("send msg to usrid %d fd %d failed", msg->m_head.m_usrID, m_usrfd[msg->m_head.m_usrID]);
                            }
                            break;
                        default:
                            TRACER_ERROR("msgtype not define yet, type：%d", msg->m_head.m_type);
                            break;
                    }

                } else {
                    if (msg->m_head.m_type == USEREXIT) {
                        m_usrfd.erase(msg->m_head.m_usrID);
                        for (auto iter = m_clientsfd.begin(), end = m_clientsfd.end(); iter != end; ++iter) {
                            if (iter->usrid == msg->m_head.m_type) {
                                m_clientsfd.erase(iter);
                                break;
                            }
                        }
                    }

                    tcp_socket::tcp_send(g_connet_server[GAME_SERVER], msg->m_data, msg->m_head.m_len + MSG_HEAD_SIZE);
                }
                break;
        }

        msg->m_flag = msg_flags::INACTIVE;
    }
}



void gate_server::broadcaster(message* _msg)
{

    int size = m_usrfd.size();

    // 保证生命周期，不会出现悬挂指针
    auto foo = [this, &_msg](std::vector<client_info>::iterator& start, std::vector<client_info>::iterator& end)
    {
        for (; start != end; ++start) {
            if (tcp_socket::tcp_send(start->fd, _msg->m_data, _msg->m_head.m_len + MSG_HEAD_SIZE)) {
                this->m_errorfd.add(*start);
            }
        }
    };

    if (size < 10) {
        for (auto i = m_clientsfd.begin(), j = m_clientsfd.end(); i != j;) {
            if (tcp_socket::tcp_send(i->fd, _msg->m_data, _msg->m_head.m_len + MSG_HEAD_SIZE)) {
                m_usrfd.erase(i->usrid);
                i = m_clientsfd.erase(i);
            } else {
                ++i;
            }
        }

    } else if (size < 100) {
        auto iter0 = m_clientsfd.begin();
        auto iter1 = iter0 +  m_clientsfd.size() / 2;
        auto iter2 = m_clientsfd.end();

        std::thread t1(foo, std::ref(iter0), std::ref(iter1));
        std::thread t2(foo, std::ref(iter1), std::ref(iter2));
        t1.join();
        t2.join();
    } else {
        auto iter0 = m_clientsfd.begin();
        auto iter1 = iter0 +  m_clientsfd.size() / 4;
        auto iter2 = iter1 +  m_clientsfd.size() / 4;
        auto iter3 = iter2 +  m_clientsfd.size() / 4;
        auto iter4 = m_clientsfd.end();

        std::thread t1(foo, std::ref(iter0), std::ref(iter1));
        std::thread t2(foo, std::ref(iter1), std::ref(iter2));
        std::thread t3(foo, std::ref(iter2), std::ref(iter3));
        std::thread t4(foo, std::ref(iter3), std::ref(iter4));

        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }


    // todo 优化
    if (m_errorfd.size() > 0) {
        for (int i = 0, j = m_errorfd.size(); i < j; ++i) {
            m_usrfd.erase(m_errorfd[i].usrid);
            // m_clientsfd.erase(std::remove(m_clientsfd.begin(), m_clientsfd.end(), m_errorfd[i]), m_clientsfd.end());
        }
        int end = m_clientsfd.size();
        for (int i = 0; i < end; ++i) {
            for (int n = 0, m = m_errorfd.size(); n < m; ++n) {
                if (m_errorfd[n] == m_clientsfd[i]) {
                    m_clientsfd[i] = m_clientsfd[--end];
                }
            }
        }
        m_clientsfd.erase(m_clientsfd.begin() + end, m_clientsfd.end());
        m_errorfd.clear();
    }


    return;
}
