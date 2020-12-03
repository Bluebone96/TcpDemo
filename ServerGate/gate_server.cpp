#include "gate_server.h"

gate_server::gate_server()
{

}

gate_server::~gate_server()
{

}

int8_t gate_server::init_gate()
{
    m_net.m_pmsg = &m_msgq;
    server_config cfg;

    load_config("gate_server", cfg);
    uint32_t fd = m_net.m_listenfd.tcp_listen(cfg.ip.c_str(), cfg.port);
    if (fd < 0) {
        return -1;
    }
    m_net.m_epoll.Add(fd);

    load_config("game_server", cfg);
    fd = m_net.m_connectfd.tcp_connect(cfg.ip.c_str(), cfg.port);
    if (fd < 0) {
        return -1;
    }
    m_net.m_epoll.Add(fd);

    return 0;
}

int8_t gate_server::run()
{
    message *pmsg = nullptr;
    for (;;) {
        pmsg = m_msgq.dequeue();
        if (pmsg) {
            // todo
            m_net.m_connectfd.tcp_send(pmsg->m_data, pmsg->m_head.m_len);
            pmsg->setinvalid();
        } else {
            sleep(1);
        }
    }
}