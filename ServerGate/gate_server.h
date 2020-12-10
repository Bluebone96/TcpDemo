#ifndef _GATE_SERVER_H_
#define _GATE_SERVER_H_

#include <stdint.h>
#include <map>

#include "../Net/message.h"

extern msg_queue g_recv_queue;
extern msg_queue g_send_queue;

extern std::map<uint32_t, uint32_t> g_connet_server;


class gate_server
{
public:
    gate_server(/* args */);
    ~gate_server();

    int8_t init_gate();
    int8_t run();

private:
    std::map<uint32_t, uint32_t> m_usrkey; // 验证令牌
    std::map<uint32_t, uint32_t> m_usrfd;   // bug  客户端断开后无法进行有效清除
    std::map<uint32_t, uint32_t> m_usrserver; // usr所属的game服务器
};


#endif
