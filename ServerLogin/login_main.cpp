#include "login_server.h"
#include "../Net/Net.h"
#include "../Common/log.h"
#include "../Config/loadconfig.h"
#include "../Common/basetype.h"
#include "../Net/message.h"
#include "../Proto/PlayerInfo.pb.h"
#include <thread>
// #include <mutex>

msg_queue g_recv_queue;
msg_queue g_send_queue;

std::map<uint32_t, uint32_t> g_connet_server;

std::map<int32_t, Proto::Unity::ServerInfo> g_serverinfo;


int main()
{
    g_recv_queue.init_queue(4096);
    g_send_queue.init_queue(4096);

    Net net;
    login_server server;

    Proto::Unity::ServerInfo serverinfo;
    message msg;

    msg.m_head.m_type = SERVER_INFO;

    server_config cfg;
    load_config("login_server", cfg);
    if (net.init(cfg.ip.c_str(), cfg.port) < 0) {
        TRACER("net init failed\n");
        exit(1);
    }
    
    serverinfo.set_m_ip(cfg.ip);
    serverinfo.set_m_port(cfg.port);
    serverinfo.set_m_type(LOGIN_SERVER);

    msg.encode_pb(serverinfo); // 

    g_serverinfo.insert(std::make_pair(LOGIN_SERVER, serverinfo));

    load_config("db_server", cfg);
    int32_t fd = net.connect(cfg.ip.c_str(), cfg.port);
    if (fd < 0) {
        TRACER_ERROR("connect dbserver faild. %s:%d\n", __POSITION__);
        exit(2);
    }
    // todo ? 发送一条serverinfo 信息给连接的服务器，告知
    serverinfo.set_m_ip(cfg.ip);
    serverinfo.set_m_port(cfg.port);
    serverinfo.set_m_type(DB_SERVER);
    g_serverinfo.insert(std::make_pair(DB_SERVER, serverinfo));


    tcp_socket::tcp_send(fd, msg.m_data, msg.m_head.m_len + MSG_HEAD_SIZE);

    g_connet_server[DB_SERVER] = fd;

    load_config("gate_server", cfg);
    fd = net.connect(cfg.ip.c_str(), cfg.port);

    serverinfo.set_m_ip(cfg.ip);
    serverinfo.set_m_port(cfg.port);

    g_serverinfo.insert(std::make_pair(GATE_SERVER, serverinfo));

    g_connet_server[GATE_SERVER] = fd; 

    tcp_socket::tcp_send(fd, msg.m_data, msg.m_head.m_len + MSG_HEAD_SIZE);


    std::thread t1(&Net::product_msg, &net);

    std::thread t2(&login_server::run, &server);

    std::thread t3(&Net::consume_msg, &net);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}