#include "Server.h"

#define PORT 5678


Server::Server()
{
    
}


Server::~Server()
{
    
}

int32_t Server::Init()
{
    int32_t fd = m_MsgTrans.OpenAsServer(PORT);
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
    listenfd = fd;
    return 0;
}

int32_t Server::GOGOGO()
{
    for (;;) {
        RecvMsg();
    }
    return 0;
}

int32_t Server::RecvMsg()
{
    int32_t fn = m_epoll.Wait();
    for (int32_t i = 0; i < fn; ++i) {
        struct epoll_event* pEvent = m_epoll.GetEvent(i);
        if (pEvent->data.fd == listenfd) {
            sockaddr_in addr;
            socklen_t len = sizeof(sockaddr_in);
            int32_t acceptfd = m_MsgTrans.Accept(listenfd, &addr, &len);
            if (acceptfd < 0) {
                TRACERERRNO("accept failed");
                continue;
            }
            
            TRACER("new client connect: %d", acceptfd);

            auto sptr = std::make_shared<TcpSocket>();
            sptr->Init(acceptfd);
            m_clients[acceptfd] = sptr;

            if (m_epoll.Add(acceptfd) < 0) {
                TRACER("m_epoll add %d failed. %s:%d\n", acceptfd, __FILE__, __LINE__);
                return -1;
            }
        } else {
            auto psock = m_clients[pEvent->data.fd];
            char msg[MAXDATALEN];
            uint16_t headlen = sizeof(MsgRecord);
            MsgRecord head;
            
            psock->RecvData(msg, sizeof(Record));
            
            head.Decode(msg, headlen);
            
            psock->RecvData(msg + headlen, head.GetSize());


            for(auto& pp : m_clients) {
                pp.second->SendData(msg, sizeof(Record) + head.GetSize());
            }
        }
    }
    return 0;
}

int32_t Server::SendMsg()
{
    return 0;
}
