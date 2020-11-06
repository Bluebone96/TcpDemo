#include "Client.h"


int32_t Client::Init(const char* hostname, int16_t port)
{
    int32_t fd = m_MsgTrans.OpenAsClient(hostname, port);
    if (fd < 0) {
        TRACER("Client Init failed");
        return -1;
    }
    sockaddr_in* addr = (sockaddr_in*)m_MsgTrans.GetAddr();

    if (m_MsgTrans.Init(addr->sin_addr.s_addr, fd) < 0) {
        TRACER("Client::Init  TcpSocket::Init(%d) fild.", fd);
        return -1;
    }

    if (m_epoll.Init() < 0) {
        TRACER("client epoll failed\n");
        return -1;
    }

    if (m_epoll.Add(fd) < 0) {
        TRACER("epoll add failed\n");
        return -1;
    }
     
    if (m_epoll.Add(0) < 0) {
        TRACER("epoll add stdin failed\n");
        return -1;
    }
    return 0;
}


int32_t Client::GOGOGO()
{
    for (;;) {
        int32_t fn = m_epoll.Wait();
        for (int32_t i = 0; i < fn; ++i) {
            struct epoll_event *pevent = m_epoll.GetEvent(i);
            int32_t fd = pevent->data.fd;
            
            switch (fd)
            {
            case 0:
                SendMsg();
                break;
            default:
                RcvMsg();
                break;
            }
        }
    }
}


int32_t Client::SendMsg()
{
    std::string msg;
    std::cin >> msg;
    m_data.set_data(msg);
    m_MsgTrans.sendmsg(m_data);
    return 0;
}

int32_t Client::RcvMsg()
{
    m_MsgTrans.recvmsg(m_data);
    std::cout << "=====================================" << std::endl;
    char paddr[INET_ADDRSTRLEN] = {0};
    in_addr naddr;
    naddr.s_addr = m_MsgTrans.GetTag();
    inet_ntop(AF_INET, &naddr, paddr, sizeof(paddr));
    std::cout << paddr << ": " << m_data.data() << std::endl;
    return 0;
}

