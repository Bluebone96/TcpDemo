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
	TRACER ("server listen socketfd = %d\n", listenfd);
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
        
        TRACER("fd = %d is readly\n", pEvent->data.fd);

        if (pEvent->data.fd == listenfd) {
            sockaddr_in addr;
            socklen_t len = sizeof(sockaddr_in);
            int32_t acceptfd = m_MsgTrans.Accept(listenfd, &addr, &len);

            if (acceptfd < 0) {
                TRACERERRNO("Server::RecvMsg accept failed");
                continue;
            }
            
            // TRACER("new client connect. fd:%d, clientaddr: %s:%d", acceptfd, inet_ntoa(addr.sin_addr), addr.sin_port);
            TRACER("new client connect. fd:%d\n", acceptfd);

            auto sptr = new TcpSocket(acceptfd);
            // sptr->Init(acceptfd);
            m_clients[acceptfd] = sptr;
            
            if (m_epoll.Add(acceptfd) < 0) {
                TRACER("m_epoll add %d failed. %s:%d\n", acceptfd, __FILE__, __LINE__);
                return -1;
            }
            
            TRACER("go next epoll_event\n");

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

            TRACER("sleep for debug 2s start\n");
            sleep(2);
            TRACER("sleep for debug 2s end\n");

            auto psock = m_clients[fd];
            MsgRecord msg;

            int32_t headlen = sizeof(Record);
            
            psock->RecvData(msg.GetDateAddress(), headlen);
            
            msg.Decode();
            
            psock->RecvData(msg.GetDateAddress() + headlen, msg.GetSize() - headlen);

            TRACER("parsefromarray\n");
            m_data.ParseFromArray(msg.GetDateAddress() + headlen, msg.GetSize() - headlen);

            std::cout << "msg is : " << m_data.data() << std::endl;     
                   
            for(auto& pp : m_clients) {
				TRACER("sendata to %d\n", pp.first);
                pp.second->SendData(msg.GetDateAddress(), msg.GetSize());
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
