#include <sys/time.h>

#include "Server.h"
#include "../Handler/Dispatcher.h"


int32_t Server::Init(int32_t port, const char* hostname)
{
    int32_t fd = m_MsgTrans.OpenAsServer(port, hostname);
    if (fd < 0) {
        TRACER("Server::Init OpenAsServer failed.\n");
        return -1;
    }

    if (m_epoll.Init() < 0) {
        TRACER("Server::Init Epoll::Init failed.\n");
        return -1;
    }

    if (m_epoll.Add(fd) < 0) {
        TRACER("epoll add %d failed", fd);
        return -1;
    }

    m_listenfd = fd;
	TRACER ("server listen socketfd = %d\n", m_listenfd);

    if (DISPATCHER.Init()) {
        TRACER("dispatcher init failed\n");
        return -1;
    }
    return 0;
}

int32_t Server::GOGOGO()
{
    struct timeval curTime;
    struct timeval lastTime;
    gettimeofday(&lastTime, nullptr);

    for (;;) {
        int32_t fn = m_epoll.Wait();
        for (int32_t i = 0; i < fn; ++i) {
            struct epoll_event* pEvent = m_epoll.GetEvent(i);

            TRACER("fd = %d is readly\n", pEvent->data.fd);
            if (pEvent->data.fd == m_listenfd) {
                if (AcceptNewClient() < 0) {
                    TRACER("error in Server::AcceptNewClient");
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
                    TRACER("Epoll event epollerr fd= %d\n", fd);
                    delete m_players[fd];
                    break;
                default:
                    TRACER("Epoll unknowen fd = %d\n", fd);
                    break;
                }

                if (DISPATCHER.Process(m_players[fd]) < 0) {
                    // TODO
                }
            }
        }
        gettimeofday(&curTime, nullptr);
        
        if (curTime.tv_sec - lastTime.tv_sec >= 2) { 
            // 每 2 秒 主动 同步所有玩家
            DISPATCHER.Process(EventType::SYNCCLIENT);
        }

    }
    return 0;
}

int32_t Server::AcceptNewClient()
{
    sockaddr_in addr;
    socklen_t len = sizeof(sockaddr_in);
    int32_t acceptfd = m_MsgTrans.Accept(m_listenfd, &addr, &len);

    if (acceptfd < 0) {
        TRACERERRNO("Server::RecvMsg accept failed");
        return 1;
    }
    
    TRACER("new client connect. fd:%d\n", acceptfd);

    if (m_epoll.Add(acceptfd) < 0) {
        TRACER("m_epoll add %d failed. %s:%d\n", acceptfd, __FILE__, __LINE__);
        return -1;
    }

    // 这里直接登录，初始化一个角色，没有登录验证
    auto msg = new MsgTrans();
    msg->Init(acceptfd, &addr, len);
    auto player = new Player(msg); 
    TRACER("the player id is %d", player->getId());
    m_players[acceptfd] = player;

    return 0;
}

int32_t Server::SendMsgToAll(char* _buf, int _len)
{
    for (auto& iter : m_players) {
        SendMsgToOne(iter.first, _buf, _len);
    }
    return 0;
}


int32_t Server::SendMsgToOne(int fd, char* _buf, int _len)
{
    if (TcpSocket::SendData(_len, _buf, _len) < 0) {
        TRACER("send data failed player id: %d, %s:%d ", fd, __POSITION__);
        return -1;
    }
    
    return 0;
}

int32_t Server::RecvMsg()
{
    return 0;
}

int32_t Server::SendMsg()
{
    return 0;
}



