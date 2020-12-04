#include "Net.h"
#include "../Common/log.h"

Net::Net()
{

}

Net::~Net()
{

}

int8_t Net::init(const char* _ip, uint32_t _port)
{
    int32_t fd = m_listenfd.tcp_listen(_ip, _port);
    if (fd < 0) {
        return -1;
    }
    TRACER("listenfd = %d", m_listenfd.getfd());

    m_listenfd.tcp_init(fd);

    TRACER("listenfd = %d", fd);
    if (m_epoll.Add(fd) < 0) {
        TRACER("epoll add failed");
        sleep(2);
        return -1;
    }
    TRACER("epoll add success");
    sleep(2);
    return 0;
}


int8_t Net::product_msg()
{
    for (;;) {
        int32_t fn = m_epoll.Wait();
        for (int32_t i = 0; i < fn; ++i) {
            struct epoll_event* pEvent = m_epoll.GetEvent(i);

            if (pEvent->data.fd == (int32_t)m_listenfd.getfd()) {
                
                int32_t newclient = m_listenfd.tcp_accept(nullptr, nullptr);
                if (newclient > 0) {
                    auto socket = std::make_shared<tcp_socket>();
                    socket->tcp_init(newclient);

                    auto re = m_connections.insert(std::make_pair(newclient, socket));
                    if (re.second == false) {
                        TRACER("insert failed\n");

                    }

                    m_epoll.Add(newclient);
                    TRACER("new client\n");
                }
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
                    ret = recvmsg(socket, *msg);
                    if (ret < 0) {
                        TRACER("connect failed");
                        m_connections.erase(fd);
                        break;
                    }
                    //  else if (ret == 0) {
                    //     break;
                    // }
                    // msg->decode();
                    // ret = socket->tcp_recv(msg->m_data + MSG_HEAD_SIZE, msg->m_head.m_len);
                    // if (ret < 0 ) {
                    //     TRACER("connect failed");
                    //     m_connections.erase(fd);
                    //     break;
                    // }
                    // msg->m_fd = fd;
                    // msg->setvalid();
                }
            }
        }
    }
}


int8_t Net::consume_msg()
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
        if (sendmsg(iter->second, *msg) < 0) {
            m_connections.erase(iter);
        }
        msg->setinvalid();
    }
}


int8_t Net::recvmsg(std::shared_ptr<tcp_socket>& _socket, message& _msg)
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
    _msg.m_fd = _socket->getfd();

    _msg.setvalid();
    return 0;
}

int8_t Net::sendmsg(std::shared_ptr<tcp_socket>& _socket, message& _msg)
{
    _msg.encode();
    return _socket->tcp_send(_msg.m_data, _msg.m_head.m_len);
}


int8_t Net::connect(const char* _ip, uint32_t _port)
{
    tcp_socket *socket = new tcp_socket;
    int32_t fd = socket->tcp_connect(_ip, _port);
    
    socket->tcp_init(fd);
    return fd;
}