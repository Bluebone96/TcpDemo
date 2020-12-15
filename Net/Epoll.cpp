#include <cstdio>
#include <cstring>

#include "Epoll.h"
#include "../Common/log.h"

Epoll::Epoll()
{

}

Epoll::~Epoll()
{
    close(m_epoll_fd);
    delete[] m_pevents;
    m_pevents = nullptr;
}

int32_t Epoll::Init(int32_t size)
{
    m_size = size;
    bzero(&m_event, sizeof(m_event));

    // EPOLLRDHUP 新增对于断开连接的判断，不用再去读socket判断
    m_event.events = EPOLL_EVENTS::EPOLLIN | EPOLL_EVENTS::EPOLLRDHUP /*| EPOLL_EVENTS::EPOLLET*/;
    m_event.data.ptr = nullptr;
    m_event.data.fd = -1;

    m_pevents = new epoll_event[size];
    if (nullptr == m_pevents) {
        TRACER_ERROR("EpollInit failed! : %s:%d", __FILE__, __LINE__);
        return -1;
    }
    
    // EPOLL_CLOEXEC exec 时关闭该描述符, fork出子进程， 子进程获取父进程的数据空间， exec执行另一个程序时不用再管理该fd, 自动关闭
    m_epoll_fd = ::epoll_create1(EPOLL_CLOEXEC);
    if (m_epoll_fd < 0) {
        TRACER_ERROR("EpollInit failed! : %s:%d", __FILE__, __LINE__);
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
        TRACERERRNO("Epoll::Add epoll_ctl add fd = %d failed, %s:%d\n", fd, __FILE__, __LINE__);
        return -1;
    };
    TRACER_DEBUG("epoll add new socket %d\n", fd);
    return 0;
}

int32_t Epoll::Del(int32_t fd)
{
    if (fd < 0) {
        TRACER_ERROR("Epoll:Del fd = %d", -1);
        return -1;
    }

    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr) < 0) {
        TRACER_ERROR("Epoll::Del epollctl del fd = %d failed!\n", fd);
        return -1;
    }
    return 0;
}

int32_t Epoll::Wait(int32_t timeout)
{
    int  rn = epoll_wait(m_epoll_fd, m_pevents, m_size, timeout);
    switch (rn) {
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