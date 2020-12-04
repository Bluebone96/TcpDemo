#ifndef _GATE_SERVER_H_
#define _GATE_SERVER_H_

#include <stdint.h>


#include "../Common/Epoll.h"
#include "../Net/Net.h"
#include "../config/loadconfig.hpp"

class gate_server
{
public:
    gate_server(/* args */);
    ~gate_server();

    int8_t init_gate();
    int8_t run();

private:
    Net m_net;
    msg_queue m_msgq;
};


#endif
