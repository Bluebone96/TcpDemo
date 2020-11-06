#include <cstdio>
#include <cstring>

#include "Epoll.h"
#include "log.h"

Epoll::Epoll()
{

}

Epoll::~Epoll()
{
    close(m_epoll_fd);
    delete[] m_pevents;
}

int32_t Epoll::Init(int32_t size)
{
    m_size = size;
    bzero(&m_event, sizeof(m_event));

    m_event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    m_event.data.ptr = nullptr;
    m_event.data.fd = -1;

    m_pevents = (struct epoll_event*)malloc(m_size * sizeof(epoll_event));
    if (nullptr == m_pevents) {
        TRACER("EpollInit failed! : %s:%d", __FILE__, __LINE__);
        return -1;
    }

    m_epoll_fd = ::epoll_create(EPOLL_CLOEXEC);
    if (m_epoll_fd < 0) {
        TRACER("EpollInit failed! : %s:%d", __FILE__, __LINE__);
        return -1;
    }
    
    return 0;
}

int32_t Epoll::Add(int32_t fd)
{
    if (fd < 0) {
        return -1;
    }
    m_event.data.fd = fd;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &m_event) < 0) {
        TRACERERRNO("Epoll::Add epoll_ctl failed, %s:%d\n", __FILE__, __LINE__);
        return -1;
    };
    return 0;
}

int32_t Epoll::Wait(int32_t timeout)
{
    int  rn = epoll_wait(m_epoll_fd, m_pevents, m_size, timeout);
    switch (rn)
    {
    case 0:
        // TRACER("Epoll::Wait epoll_wait timeout.\n");
        break;
    case -1:
        TRACERERRNO("Epoll::Wait epoll_wait failed. %s:%d\n", __FILE__, __LINE__);    
        break;
    default:
        break;
    }
    return rn;
}

epoll_event* Epoll::GetEvent(int32_t n)
{
    if (n > m_size) {
        return nullptr;
    }
    return &m_pevents[n];
}