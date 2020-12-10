#include <thread>
#include <map>

#include "game_server.h"
#include "../Net/Net.h"
#include "../Config/loadconfig.h"
#include "../Common/basetype.h"
#include "../Net/message.h"


msg_queue g_recv_queue;
msg_queue g_send_queue;

std::map<uint32_t, uint32_t> g_connet_server;

int main ()
{
    g_recv_queue.init_queue(1024);
    g_send_queue.init_queue(1024);

    game_server server;
    Net net;

    server_config cfg;
    load_config("game_server", cfg);

    if (net.init(cfg.ip.c_str(), cfg.port) < 0) {
        TRACER_ERROR("net init failed\n");
        exit(1);
    }

    load_config("db_server", cfg);
    int32_t fd = net.connect(cfg.ip.c_str(), cfg.port);
    if (fd < 0) {
        TRACER_ERROR("connect dbserver faild. %s:%d\n", __POSITION__);
        exit(2);
    }
    g_connet_server[DB_SERVER] = fd;

    // load_config("center_server", cfg);
    // int32_t fd = net.connect(cfg.ip.c_str(), cfg.port);
    // if (fd < 0) {
    //     TRACER("connect dbserver faild. %s:%d\n", __POSITION__);
    //     exit(2);
    // }

    
    

    std::thread t1(&Net::product_msg, &net);

    std::thread t2(&game_server::run, &server);

    std::thread t3(&Net::consume_msg, &net);


    t1.join();
    t2.join();
    t3.join();

}