#include "Net.h"
#include "../Common/log.h"

Net::Net() : m_listenfd(-1), g_recv_queue(nullptr), g_send_queue(nullptr)
{

}

Net::~Net()
{

}

int8_t Net::net_init(msg_queue* _recvq, msg_queue* _sendq)
{
    TRACER("init network start\n");


    g_recv_queue = _recvq;
    g_send_queue = _sendq;

    if (m_epoll.Init() < 0) {
        TRACER_ERROR("Server::Init Epoll::Init failed.\n");
        return -1;
    }

    return 0;
}

int8_t Net::net_listen(const char* _ip, uint32_t _port)
{
    TRACER("init network start\n");
    int32_t fd = m_listen_socket.tcp_listen(_ip, _port);
    if (fd < 0) {
        TRACER_ERROR("net_listen failed , fd = %d\n", fd);
        return -1;
    }

    // listen 不需要缓冲区
    // m_listen_socket.tcp_init_buf(fd);

    if (m_epoll.Add(fd) < 0) {
        TRACER_ERROR("epoll add failed\n");
        return -1;
    }
    m_listenfd = fd;

    return 0;
}

int8_t Net::net_connect(const char* _ip, uint32_t _port)
{
    auto socket = std::make_shared<tcp_socket>();
    int32_t fd = socket->tcp_connect(_ip, _port);
    socket->tcp_init_buf();
    auto re = m_connections.insert(std::make_pair(fd, socket));
    if (re.second == false) {
        return -1;
    }
    if (m_epoll.Add(fd) < 0) {
        TRACER_ERROR("epoll add failed\n");
        return -1;
    }
    
    return fd;
}

int8_t Net::product_msg()
{
    if (g_recv_queue == nullptr) {
        TRACER_ERROR("g_recv_queue is nullptr\n");
        return -1;
    }
    TRACER_DEBUG("debug: m_listenfd is %d\n", m_listenfd);
    for (;;) {

        int32_t fn = m_epoll.Wait();
        for (int32_t i = 0; i < fn; ++i) {
            struct epoll_event* pEvent = m_epoll.GetEvent(i);
            
            if (pEvent->data.fd == m_listenfd) {
                
                auto socket = std::make_shared<tcp_socket>();
                int32_t newclient = socket->tcp_accept(m_listenfd);
                if (newclient < 0) {
                    TRACER_ERROR("net::product_msg tcp_accept failed, fd = %d", newclient);
                    continue;
                }
                socket->tcp_init_buf();

                auto re = m_connections.insert(std::make_pair(newclient, socket));
                if (re.second == false) {
                    TRACER_ERROR("insert failed\n");
                }

                m_epoll.Add(newclient);

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
                        continue;
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
                auto& socket = m_connections[fd];
                int ret = 0;
                message *msg = nullptr;

                while ((msg = g_recv_queue->enqueue()) == nullptr)
                {
                    // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
                    TRACER_ERROR("sleep 50ms, g_recv_queue is full, fd is %d\n", fd);
                    g_recv_queue->debug_info();
                    usleep(50 * 1000);
                }

                do  {
                    TRACER_DEBUG("net::product ---- check the g_recv_queue\n");
                    g_recv_queue->debug_info();
                    
                    ret = recvmsg(socket, *msg);
                    if (ret < 0) {
                        // 读取失败将 msg->m_flag  设置为 invalied
                        // 如果不是 eagain 则 erase fd
                        msg->m_flag = msg_flags::INVALID;
                        if (ret != SOCKET_EAGAIN) {
                            TRACER_ERROR("socket recv msg failed errorid: %d\n", ret);
                            m_connections.erase(fd);
                        }
                        break;
                    }

                    msg->m_flag = msg_flags::ACTIVE;
                } while ((msg = g_recv_queue->enqueue()));

                // if (msg == nullptr) {
                //     // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
                //     TRACER_ERROR("g_recv_queue is full, fd is %d\n", fd);
                //     g_recv_queue->debug_info();
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
    if (g_recv_queue == nullptr) {
        TRACER_ERROR("g_recv_queue is nullptr\n");
        return -1;
    }

    message *msg = nullptr;
    for (;;) {
        if ((msg = g_send_queue->dequeue()) == nullptr) {
            usleep(100 * 10000);
            // TRACER("Net::consume_msg() : g_send_queue msg is empty\n");
            // g_send_queue->debug_info();
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


