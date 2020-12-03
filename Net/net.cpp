#include "net.h"
#include "../Common/log.h"

net::net()
{

}

net::~net()
{

}

// int8_t net::init()
// {

// }

int8_t net::run()
{
    for (;;) {
        int32_t fn = m_epoll.Wait();
        for (int32_t i = 0; i < fn; ++i) {
            struct epoll_event* pEvent = m_epoll.GetEvent(i);

            if (pEvent->data.fd == m_listenfd.getfd()) {
                
                uint32_t newclient = m_listenfd.tcp_accept(nullptr, nullptr);
                if (newclient > 0) {
                    auto re = m_clients.insert(std::make_pair(newclient, std::make_shared<tcp_socket>(new tcp_socket())));
                    if (re.second == false) {
                        TRACER("insert failed\n");
                    }
                    m_clients[newclient]->tcp_init(newclient);
                }
                TRACER("new client\n");
            } else {
                uint32_t event = pEvent->events;
                int32_t fd = pEvent->data.fd;
                switch (event)
                {
                case EPOLLIN:
                    TRACER("Epoll event EPOLLIN fd = %d\n", fd);
                    break;
                case EPOLLHUP:
                    TRACER("Epoll event epollhup fd = %d\n", fd);
                    break;
                case EPOLLERR:
                    TRACER("Epoll event epollerr fd= %d, delete ptr\n", fd);
                    m_clients.erase(fd);
                    continue;
                default:
                    TRACER("Epoll unknowen, fd = %d, delete ptr\n", fd);
                    m_clients.erase(fd);
                    continue;
                }
                auto socket = m_clients[fd];
                int ret = 0;
                message *msg = nullptr;
                while((msg = m_pmsg->enqueue()) ) {
                    ret = socket->tcp_recv(msg->m_data, MSG_HEAD_SIZE);
                    if (ret < 0) {
                        TRACER("connect failed");
                        m_clients.erase(fd);
                        break;
                    } else if (ret == 0) {
                        break;
                    }
                    msg->decode();
                    ret = socket->tcp_recv(msg->m_data + MSG_HEAD_SIZE, msg->m_head.m_len);
                    if (ret < 0 ) {
                        TRACER("connect failed");
                        m_clients.erase(fd);
                        break;
                    }
                    msg->setvalid();
                }
            }
        }
    }
}