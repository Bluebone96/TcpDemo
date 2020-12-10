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
    TRACER("init network start\n");
    int32_t fd = m_listenfd.tcp_listen(_ip, _port);
    if (fd < 0) {
        return -1;
    }
    TRACER("listenfd = %d\n", m_listenfd.getfd());

    m_listenfd.tcp_init(fd);

    if (m_epoll.Init() < 0) {
        TRACER_ERROR("Server::Init Epoll::Init failed.\n");
        return -1;
    }
    
    if (m_epoll.Add(fd) < 0) {
        TRACER_ERROR("epoll add failed\n");
        return -1;
    }
    TRACER("epoll add success\n");
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
                        TRACER_ERROR("insert failed\n");
                    }

                    m_epoll.Add(newclient);
                    TRACER_DEBUG("new client fd is %d\n", newclient);
                }
            } else {
                uint32_t event = pEvent->events;
                int32_t fd = pEvent->data.fd;
                switch (event) {
                    case EPOLLIN:
                        TRACER_DEBUG("Epoll event EPOLLIN fd = %d\n", fd);
                        break;
                    case EPOLLIN | EPOLLRDHUP:
                        TRACER_ERROR("Epoll event EPOLLIN | EPOLLRDHUP fd = %d\n", fd);
                        m_connections.erase(fd);
                        continue;
                    case EPOLLHUP:
                        TRACER_ERROR("Epoll event epollhup fd = %d\n", fd);
                        break;
                    case EPOLLERR:
                        TRACER_ERROR("Epoll event epollerr fd= %d, delete ptr\n", fd);
                        m_connections.erase(fd);
                        continue;
                    default:
                        TRACER_ERROR("Epoll unknowen events %d, fd = %d, delete ptr\n", event, fd);
                        m_connections.erase(fd);
                        continue;
                }

                TRACER_DEBUG("new message, fd is %d\n", fd);
                auto socket = m_connections[fd];
                int ret = 0;
                message *msg = nullptr;

                while ((msg = g_recv_queue.enqueue()) == nullptr)
                {
                    // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
                    TRACER_ERROR("sleep 50ms, g_recv_queue is full, fd is %d\n", fd);
                    g_recv_queue.debug_info();
                    usleep(50 * 1000);
                }

                do  {
                    ret = recvmsg(socket, *msg);
                    if (ret < 0) {
                        // 读取失败将 msg->m_flag  设置为 invalied
                        // 如果不是 eagain 则 erase fd
                        msg->m_flag = msg_flags::INVALID;
                        if (ret != SOCKET_ERROR_EAGAIN) {
                            TRACER_ERROR("socket recv msg failed errorid: %d\n", ret);
                            m_connections.erase(fd);
                        }
                        break;
                    }

                    msg->m_flag = msg_flags::ACTIVE;
                } while ((msg = g_recv_queue.enqueue()));

                // if (msg == nullptr) {
                //     // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
                //     TRACER_ERROR("g_recv_queue is full, fd is %d\n", fd);
                //     g_recv_queue.debug_info();
                //     usleep(100 * 1000);
                // }
            }
            TRACER_DEBUG("go next epoll\n");
            usleep(10 * 1000);
        }
        
    }
}


int8_t Net::consume_msg()
{
    message *msg = nullptr;
    for (;;) {
        if ((msg = g_send_queue.dequeue()) == nullptr) {
            usleep(100 * 1000);;
            // TRACER("Net::consume_msg() : g_send_queue msg is empty\n");
            continue;
        }

        auto iter = m_connections.find(msg->m_to);

        if (iter == m_connections.end()) {
            return -1;
        }

        if (sendmsg(iter->second, *msg) < 0) {
            m_connections.erase(iter);
        }
        msg->m_flag = msg_flags::INACTIVE;
    }
}


int8_t Net::recvmsg(std::shared_ptr<tcp_socket>& _socket, message& _msg)
{
    TRACER_DEBUG("start recv msg data head\n");
    int ret = 0;
    ret = _socket->tcp_recv(_msg.m_data, MSG_HEAD_SIZE);
    
    if (ret < 0) {
        TRACER_DEBUG("Net::recvmsg errorid = %d\n, %s:%d\n", ret, __POSITION__);
        return ret;
    }

    _msg.decode();
    TRACER_DEBUG("msg data body len is %d\n", _msg.m_head.m_len);
    ret = _socket->tcp_recv(_msg.m_data + MSG_HEAD_SIZE, _msg.m_head.m_len);
    if (ret < 0 ) {
        TRACER_DEBUG("Net::recvmsg errorid = %d\n %s:%d\n", ret, __POSITION__);
        return -1;
    }

    _msg.m_from = _socket->getfd();

    TRACER_DEBUG("recv msg complete\n");
    return 0;
}

int8_t Net::sendmsg(std::shared_ptr<tcp_socket>& _socket, message& _msg)
{
    _msg.encode();
    return _socket->tcp_send(_msg.m_data, _msg.m_head.m_len + MSG_HEAD_SIZE);
}


int8_t Net::connect(const char* _ip, uint32_t _port)
{
    auto socket = std::make_shared<tcp_socket>();
    int32_t fd = socket->tcp_connect(_ip, _port);
    socket->tcp_init(fd);
    auto re = m_connections.insert(std::make_pair(fd, socket));
    if (re.second == false) {
        return -1;
    }
    m_epoll.Add(fd);

    return fd;
}