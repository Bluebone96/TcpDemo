#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../Common/log.h"

#define MAXEPOLLEVENTS 1024

class Epoll {
public:
    Epoll();
    ~Epoll();
    int32_t Init(int32_t n = MAXEPOLLEVENTS);
    int32_t Wait(int32_t time = 10);
    int32_t Add(int32_t);

    epoll_event* GetEvent(int32_t);
private:
    int32_t m_size;
    int32_t m_epoll_fd;
    epoll_event m_event;
    epoll_event* m_pevents;
};

#endif
