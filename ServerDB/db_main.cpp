#include "db_server.h"
#include "../Config/loadconfig.h"
#include "../Net/Net.h"
#include "../Net/message.h"
#include "../Common/log.h"

#include <iostream>
#include <thread>


msg_queue g_recv_queue;
msg_queue g_send_queue;



int main()
{
    printf("db main start\n");
    TRACER("db main start\n");
    g_recv_queue.init_queue(4096);
    g_send_queue.init_queue(4096);
    
    db_server server;
    server.init();

    Net net;
    
    if (net.net_init(&g_recv_queue, &g_send_queue)) {
        TRACER_ERROR("net init failed");
        exit(1);
    }
    

    server_config cfg;
    if (load_config("db_server", cfg)) {
        TRACER_ERROR("load db_server config failed");
        exit(1);
    }
    if (net.net_listen(cfg.ip.c_str(), cfg.port) < 0) {
        TRACER_ERROR("net listen failed addr: %s:%d\n", cfg.ip.c_str(), cfg.port);
        exit(1);
    }


    std::thread t1(&Net::product_msg, &net);

    std::thread t2(&db_server::run, &server);

    std::thread t3(&Net::consume_msg, &net);


    t1.join();
    t2.join();
    t3.join();

    return 0;
}