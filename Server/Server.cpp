#include "Server.h"

Server::~Server()
{
    
}

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
    return 0;
}

int32_t Server::GOGOGO()
{
    for (;;) {
        RecvMsg();
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

    auto sptr = new MsgTrans();
    sptr->Init(acceptfd, &addr, len);
    m_clients[acceptfd] = sptr;
    
    if (m_epoll.Add(acceptfd) < 0) {
        TRACER("m_epoll add %d failed. %s:%d\n", acceptfd, __FILE__, __LINE__);
        return -1;
    }

    return 0;
}

int32_t Server::RecvMsg()
{
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
                delete m_clients[fd];
                break;
            default:
                TRACER("Epoll unknowen fd = %d\n", fd);
                break;
            }

            MsgTrans* pmsgtrans = m_clients[fd];
            
            if (pmsgtrans->recvmsg() < 0) {
                TRACER("Bye, %d\n", pmsgtrans->GetSocketfd());
                delete pmsgtrans;
                continue;
            }
            
            char* data = pmsgtrans->GetDataAddress();
            int32_t len = pmsgtrans->GetDataLen();
            // std::cout << "Recordbuf len is " << pmsgtrans->GetDataLen() << std::endl;
            // std::cout << "msg is : " << tmpdata.msg() << std::endl;

            for(auto& pp : m_clients) {
                TRACER("sendata to %d\n", pp.first);
                pp.second->sendmsg(data, len + sizeof(Record));
            }

            TRACER("all clients sended, go next loop.\n");
        }
    }
    return 0;
}

int32_t Server::SendMsg()
{
    return 0;
}



