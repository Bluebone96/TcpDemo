#include <unistd.h>
#include <thread>


#include "../Common/basetype.h"
#include "../Common/log.h"
#include "../Net/tcp_socket.h"
#include "gate_server.h"
#include "broadcast.hpp"

gate_server::gate_server() : m_broadcast(nullptr)
{

}

gate_server::~gate_server()
{
    delete m_broadcast;
}

int8_t gate_server::init_gate()
{
    m_broadcast = new Broadcast();

    return m_broadcast->init(this);
}

int8_t gate_server::run_client()
{
    message *msg = nullptr;
    for (;;) {
        msg = g_client_queue.dequeue();
        if (msg == nullptr) {
            // TRACER_ERROR("gate_server::run_client() sleep 50ms, g_client_queue is empty\n");
            // g_client_queue.debug_info();
            usleep(50 * 1000);
            continue;
        }
        
        TRACER_DEBUG("msg type is %d\n", msg->m_head.m_type);
        switch (msg->m_head.m_type) {
            case USERLOGIN:
                TRACER("new player login usrid is %d, fd is %d\n", msg->m_head.m_usrID, msg->m_from);
                m_usrfd[msg->m_head.m_usrID] = msg->m_from;    // 第一次登录
                m_clientsfd.emplace_back(client_info(msg->m_head.m_usrID, msg->m_from));
                break;
            case USEREXIT:
                TRACER("client exit usrid = %d\n", msg->m_head.m_usrID);
                for (auto iter = m_clientsfd.begin(), end = m_clientsfd.end(); iter != end; ++iter) {
                    if (iter->usrid == msg->m_head.m_usrID) {
                        m_clientsfd.erase(iter);
                        break;
                    }
                }
                m_usrfd.erase(msg->m_head.m_usrID);

            case USERUP:
                if (tcp_socket::tcp_send(g_connet_server[GAME_SERVER], msg->m_data, msg->m_head.m_len + MSG_HEAD_SIZE) < 0) {
                    TRACER_ERROR("send  update msg to game_server failed, gameserver fd %d, usrid %d\n", g_connet_server[GAME_SERVER], msg->m_head.m_usrID);
                }
                break;
            case ITEMEVENT:
                // item operation
                if (tcp_socket::tcp_send(g_connet_server[GAME_SERVER], msg->m_data, msg->m_head.m_len + MSG_HEAD_SIZE) < 0) {
                    TRACER_ERROR("send itemevent msg to game_server %d fd %d failed\n", g_connet_server[GAME_SERVER], msg->m_head.m_usrID);
                }
                break;
            default:
                TRACER_ERROR("msg type undefined type: %d\n", msg->m_head.m_type);
                break;
        }
        msg->m_flag = msg_flags::INACTIVE;
    }
}



int8_t gate_server::run_server()
{

    message *msg = nullptr;
    for (;;) {
        msg = g_server_queue.dequeue();
        if (msg == nullptr) {
            usleep(50 * 1000);
            // TRACER_ERROR("gate_server::run_server() sleep 50ms, g_server_queue is empty\n");
            // g_server_queue.debug_info();
            continue;
        }

        switch (msg->m_head.m_type) {
            case SERVER_INFO:
                {
                    Proto::Unity::ServerInfo server;
                    msg->decode_pb(server);
                    g_connet_server[server.m_type()] = msg->m_from;
                }
                break;
            case GET_ALLINFO:
                // loginserver 发过来的包，转发给game server, 等待优化中 todo
                if (tcp_socket::tcp_send(g_connet_server[GAME_SERVER], msg->m_data, msg->m_head.m_len + MSG_HEAD_SIZE) < 0) {
                    TRACER_ERROR("send msg to game_server fd %d failed\n", g_connet_server[GAME_SERVER]);
                }
                break;
            case USEREXIT: // 服务器主动退出
                for (auto iter = m_clientsfd.begin(), end = m_clientsfd.end(); iter != end; ++iter) {
                    if (iter->usrid == msg->m_head.m_usrID) {
                        m_clientsfd.erase(iter);
                        break;
                    }
                }
                m_usrfd.erase(msg->m_head.m_usrID);
                TRACER_DEBUG("gate_server::run_server msg type is userexit.\n");
                m_broadcast->runtask(msg->m_data, msg->m_head.m_len + MSG_HEAD_SIZE);
                clear_up();
                break;
            case USERSYNC:
                TRACER_DEBUG("gate_server::run_server msg type is usrsync.\n");
                // m_broadcast->runtask(msg->m_data, msg->m_head.m_len + MSG_HEAD_SIZE);
                // clear_up();
                break;
            case ITEMEVENT:
                // item operation
                if (tcp_socket::tcp_send(m_usrfd[msg->m_head.m_usrID], msg->m_data, msg->m_head.m_len + MSG_HEAD_SIZE) < 0) {
                    TRACER_ERROR("send msg to usrid %d fd %d failed\n", msg->m_head.m_usrID, m_usrfd[msg->m_head.m_usrID]);
                    for (auto iter = m_clientsfd.begin(), end = m_clientsfd.end(); iter != end; ++iter) {
                        if (iter->usrid == msg->m_head.m_usrID) {
                            m_clientsfd.erase(iter);
                            break;
                        }
                    }
                    m_usrfd.erase(msg->m_head.m_usrID);
                }
                break;
            default:
                TRACER_ERROR("msgtype not define yet, type：%d\n", msg->m_head.m_type);
                break;
        }

        msg->m_flag = msg_flags::INACTIVE;
    }
}



void gate_server::clear_up()
{
     // todo 优化
    if (m_errorfd.size() > 0) {
        TRACER("start erase %d bad file descriptor\n", m_errorfd.size());
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

        // TODO 客户端突然掉线未发送离线包, 或者包还在队列中
        // 1. gate_server 主动通知game_server 
        // 2. 不处理 让game_server 根据心跳包来主动踢掉不活跃的客户端
        // 发送方式 
        // 1.放入队列.
        // 2.直接发送,  
        // message* msg = nullptr;
        // for (int i = 0; i < end; ++i) {
            // while ((msg = g_client_queue.enqueue()) == nullptr) {
                // usleep(10 * 1000);
            // }
            // msg->m_head.m_type = USEREXIT;
            // msg->m_head.m_len = 0;
            // msg->m_head.m_usrID = m_errorfd[i].usrid;
            // msg->m_head.m_errID = 0;
            // msg->m_to = g_connet_server[GAME_SERVER];
            // msg->encode();
            // msg->m_flag = msg_flags::ACTIVE;
        // }

        m_errorfd.clear();
        TRACER("end erase bad file descriptor\n");
    }
}


// void gate_server::broadcaster(message* _msg)
// {

//     TRACER_DEBUG("\ngate_server::broadcaster start\n");

//     int size = m_usrfd.size();

//     // 保证生命周期，不会出现悬挂指针
//     auto foo = [this, &_msg](std::vector<client_info>::iterator& start, std::vector<client_info>::iterator& end)
//     {
//         for (; start != end; ++start) {
//             if (tcp_socket::tcp_send(start->fd, _msg->m_data, _msg->m_head.m_len + MSG_HEAD_SIZE) != 0) {
//                 this->m_errorfd.add(*start);
//             }
//         }
//     };

//     TRACER("connect clients size  = %d\n", size);
//     if (size < 50) {
//         TRACER_DEBUG("normal broad start\n");
//         for (auto i = m_clientsfd.begin() /*, j = m_clientsfd.end()*/; i != m_clientsfd.end();) {
//             if (tcp_socket::tcp_send(i->fd, _msg->m_data, _msg->m_head.m_len + MSG_HEAD_SIZE) != 0) {
//                 m_usrfd.erase(i->usrid);
//                 i = m_clientsfd.erase(i);
//             } else {
//                 ++i;
//             }
//         }
//         TRACER_DEBUG("normal broad end\n");
//     } else if (size < 100) {
//         TRACER_DEBUG("2 thread broad start\n");
//         auto iter0 = m_clientsfd.begin();
//         auto iter1 = iter0 +  m_clientsfd.size() / 2;
//         auto iter2 = m_clientsfd.end();

//         std::thread t1(foo, iter0, iter1);
//         std::thread t2(foo, iter1, iter2);
//         t1.join();
//         t2.join();
//         TRACER_DEBUG("2 thread broad end\n");
//     } else {
//         TRACER_DEBUG("4 thread broad start\n");
//         auto iter0 = m_clientsfd.begin();
//         auto iter1 = iter0 +  m_clientsfd.size() / 4;
//         auto iter2 = iter1 +  m_clientsfd.size() / 4;
//         auto iter3 = iter2 +  m_clientsfd.size() / 4;
//         auto iter4 = m_clientsfd.end();

//         std::thread t1(foo, iter0, iter1);
//         std::thread t2(foo, iter1, iter2);
//         std::thread t3(foo, iter2, iter3);
//         std::thread t4(foo, iter3, iter4);

//         t1.join();
//         t2.join();
//         t3.join();
//         t4.join();
//         TRACER_DEBUG("4 thread broad end\n");
//     }


//     // todo 优化
//     if (m_errorfd.size() > 0) {
//         TRACER("start erase %d bad file descriptor\n", m_errorfd.size());
//         for (int i = 0, j = m_errorfd.size(); i < j; ++i) {
//             m_usrfd.erase(m_errorfd[i].usrid);
//             // m_clientsfd.erase(std::remove(m_clientsfd.begin(), m_clientsfd.end(), m_errorfd[i]), m_clientsfd.end());
//         }
//         int end = m_clientsfd.size();
//         for (int i = 0; i < end; ++i) {
//             for (int n = 0, m = m_errorfd.size(); n < m; ++n) {
//                 if (m_errorfd[n] == m_clientsfd[i]) {
//                     m_clientsfd[i] = m_clientsfd[--end];
//                 }
//             }
//         }
//         m_clientsfd.erase(m_clientsfd.begin() + end, m_clientsfd.end());

//         // TODO 客户端突然掉线未发送登出包， gate_server 主动通知game_server 
//         // 或者 让game_server 根据心跳包来主动踢掉不活跃的客户端
//         // message* msg = nullptr;
//         // for (int i = 0; i < end; ++i) {
//         //     while ((msg = g_client_queue.enqueue()) == nullptr) {
//         //         usleep(10 * 1000);
//         //     }
//         //     msg->m_head.m_type = USEREXIT;
//         //     msg->m_head.m_len = 0;
//         //     msg->m_head.m_usrID = m_errorfd[i].usrid;
//         //     msg->m_head.m_errID = 0;
//         //     msg->m_to = g_connet_server[GAME_SERVER];
//         //     msg->encode();
//         //     msg->m_flag = msg_flags::ACTIVE;
//         // }

//         m_errorfd.clear();
//         TRACER("end erase bad file descriptor\n");
//     }

//     TRACER_DEBUG("gate_server::broadcaster end\n");

//     return;
// }
