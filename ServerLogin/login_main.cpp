#include "login_server.h"
#include "../Net/Net.h"
#include "../Common/log.h"
#include "../Config/loadconfig.h"
#include "../Common/basetype.h"
#include "../Net/message.h"
#include "../Proto/PlayerInfo.pb.h"
#include <thread>
#include <unordered_map>

msg_queue g_recv_queue;
msg_queue g_send_queue;

std::unordered_map<uint32_t, uint32_t> g_connet_server;

std::unordered_map<int32_t, Proto::Unity::ServerInfo> g_serverinfo;


int main()
{
    g_recv_queue.init_queue(4096);
    g_send_queue.init_queue(4096);

    Net net;
    login_server server;

    Proto::Unity::ServerInfo serverinfo;
    message msg;
    msg.m_head.m_type = SERVER_INFO;

    if (net.net_init(&g_recv_queue, &g_send_queue)) {
        TRACER_ERROR("net init failed");
        exit(1);
    }

    server_config cfg;
    if (load_config("login_server", cfg) < 0) {
        TRACER_ERROR("load login server config failed");
        exit(1);
    }
    if (net.net_listen(cfg.ip.c_str(), cfg.port) < 0) {
        TRACER_ERROR("net listen failed addr: %s:%d\n", cfg.ip.c_str(), cfg.port);
        exit(1);
    }
    
    serverinfo.set_m_ip(cfg.ip);
    serverinfo.set_m_port(cfg.port);
    serverinfo.set_m_type(LOGIN_SERVER);

    msg.encode_pb(serverinfo); // 

    g_serverinfo.insert(std::make_pair(LOGIN_SERVER, serverinfo));

    if (load_config("db_server", cfg)) {
        TRACER_ERROR("load db_server config failed");
        exit(1);
    }
    int32_t fd = net.net_connect(cfg.ip.c_str(), cfg.port);
    if (fd < 0) {
        TRACER_ERROR("connect dbserver faild. %s:%d\n", __POSITION__);
        exit(2);
    }
    // todo 发送一条serverinfo 信息给连接的服务器，告知对方自己是什么服务器
    serverinfo.set_m_ip(cfg.ip);
    serverinfo.set_m_port(cfg.port);
    serverinfo.set_m_type(LOGIN_SERVER);
    g_serverinfo.insert(std::make_pair(LOGIN_SERVER, serverinfo));


    tcp_socket::tcp_send(fd, msg.m_data, msg.m_head.m_len + MSG_HEAD_SIZE);

    g_connet_server[DB_SERVER] = fd;

    if (load_config("gate_server_for_server", cfg)) {
        TRACER_ERROR("load gate_server_for_client config failed");
        exit(1);
    }
    fd = net.net_connect(cfg.ip.c_str(), cfg.port);
    if (fd < 0) {
        TRACER_ERROR("connect gate_server_for_server faild. %s:%d\n", __POSITION__);
        exit(2);
    }

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