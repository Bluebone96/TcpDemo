#include "db_server.h"
#include "../Config/loadconfig.h"
#include "../Net/Net.h"
#include "../Net/message.h"

#include <thread>



msg_queue g_recv_queue;
msg_queue g_send_queue;



int main()
{
    db_server server;
    Net net;

    server.init();

    server_config cfg;
    load_config("db_server", cfg);
    net.init(cfg.ip.c_str(), cfg.port);


    std::thread t1(&Net::product_msg, &net);

    std::thread t2(&db_server::run, &server);

    std::thread t3(&Net::consume_msg, &net);


    t1.join();
    t2.join();
    t3.join();

    return 0;
}