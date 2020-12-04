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

int8_t net::recvmsg()
{
    for (;;) {
        int32_t fn = m_epoll.Wait();
        for (int32_t i = 0; i < fn; ++i) {
            struct epoll_event* pEvent = m_epoll.GetEvent(i);

            if (pEvent->data.fd == m_listenfd.getfd()) {
                
                uint32_t newclient = m_listenfd.tcp_accept(nullptr, nullptr);
                if (newclient > 0) {
                    auto re = m_connections.insert(std::make_pair(newclient, std::make_shared<tcp_socket>(new tcp_socket())));
                    if (re.second == false) {
                        TRACER("insert failed\n");
                    }
                    m_connections[newclient]->tcp_init(newclient);
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
                    m_connections.erase(fd);
                    continue;
                default:
                    TRACER("Epoll unknowen, fd = %d, delete ptr\n", fd);
                    m_connections.erase(fd);
                    continue;
                }
                auto socket = m_connections[fd];
                int ret = 0;
                message *msg = nullptr;
                while ((msg = g_recv_queue.enqueue())) {
                    ret = socket->tcp_recv(msg->m_data, MSG_HEAD_SIZE);
                    if (ret < 0) {
                        TRACER("connect failed");
                        m_connections.erase(fd);
                        break;
                    } else if (ret == 0) {
                        break;
                    }
                    msg->decode();
                    ret = socket->tcp_recv(msg->m_data + MSG_HEAD_SIZE, msg->m_head.m_len);
                    if (ret < 0 ) {
                        TRACER("connect failed");
                        m_connections.erase(fd);
                        break;
                    }
                    msg->setvalid();
                }
            }
        }
    }
}


int8_t net::sendmsg()
{
    message *msg = nullptr;
    for (;;) {
        if ((msg = g_send_queue.dequeue()) == nullptr) {
            sleep(2);
            continue;
        }
        auto iter = m_connections.find(msg->m_head.m_to);

        if (iter == m_connections.end()) {
            return -1;
        }

        iter->second->tcp_send(msg->m_data, msg->m_head.m_len);
        msg->setinvalid();
    }
}


int8_t net::recvmsg(std::shared_ptr<tcp_socket>& _socket, message& _msg)
{
    int ret = 0;
    ret = _socket->tcp_recv(_msg.m_data, MSG_HEAD_SIZE);
    if (ret < 0) {
        TRACER("connect failed");
        return -1;
    }
    //  else if (ret == 0) {
// 
    // }

    _msg.decode();
    ret = _socket->tcp_recv(_msg.m_data + MSG_HEAD_SIZE, _msg.m_head.m_len);
    if (ret < 0 ) {
        TRACER("connect failed");
        return -1;
    }

    return 0;
}

int8_t net::sendmsg(std::shared_ptr<tcp_socket>& _socket, message& _msg)
{
    _msg.encode();
    return _socket->tcp_send(_msg.m_data, _msg.m_head.m_len);
}