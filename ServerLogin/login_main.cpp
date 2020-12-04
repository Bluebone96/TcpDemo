#include "login_server.h"
#include "../Net/Net.h"
#include "../Common/log.h"
#include "../Config/loadconfig.h"
#include "../Common/basetype.h"
#include "../Net/message.h"

#include <thread>
#include <mutex>

msg_queue g_recv_queue;
msg_queue g_send_queue;

std::map<uint32_t, uint32_t> g_connet_server;


int main()
{
    g_recv_queue.init_queue(1024);
    g_send_queue.init_queue(1024);

    Net net;
    login_server server;

    server_config cfg;
    load_config("login_server", cfg);
    if (net.init(cfg.ip.c_str(), cfg.port) < 0) {
        TRACER("net init failed\n");
        exit(1);
    }
    load_config("db_server", cfg);
    int32_t fd = net.connect(cfg.ip.c_str(), cfg.port);
    if (fd < 0) {
        TRACER("connect dbserver faild. %s:%d\n", __POSITION__);
        exit(2);
    }
    
    g_connet_server[DB_SERVER] = fd;


    std::thread t1(&Net::product_msg, &net);

    std::thread t2(&login_server::run, &server);

    std::thread t3(&Net::consume_msg, &net);


    t1.join();
    t2.join();
    t3.join();

    return 0;
}