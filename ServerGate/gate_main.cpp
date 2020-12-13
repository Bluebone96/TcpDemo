#include <thread>
#include <map>


#include "gate_server.h"
#include "../Net/Net.h"
#include "../Config/loadconfig.h"
#include "../Common/basetype.h"
#include "../Common/log.h"

msg_queue g_server_queue;
msg_queue g_client_queue;

std::map<uint32_t, uint32_t> g_connet_server;


int main ()
{
    g_server_queue.init_queue(1024 * 16);
    g_client_queue.init_queue(1024 * 16);

    gate_server server;
    if (server.init_gate() < 0) {
        TRACER_ERROR("gate_server init failed\n");
        exit(1);
    }

    Net net_client;
    Net net_server;

    if (net_client.net_init(&g_client_queue, nullptr) < 0) {
        TRACER_ERROR("net_client init failed\n");
        exit(1);
    }

    server_config cfg;

    if (load_config("gate_server_for_client", cfg) < 0) {
        TRACER_ERROR("load gate_server_for_client config failed");
        exit(1);
    }
    if (net_client.net_listen(cfg.ip.c_str(), cfg.port)) {
        TRACER_ERROR("net_client listen failed addr: %s:%d\n", cfg.ip.c_str(), cfg.port);
        exit(1);
    }


    if (net_server.net_init(&g_server_queue, nullptr) < 0) {
        TRACER_ERROR("net_server init failed\n");
        exit(1);
    }


    if (load_config("gate_server_for_server", cfg)) {
        TRACER_ERROR("load gate_server_for_server config failed");
        exit(1);
    }
    if (net_server.net_listen(cfg.ip.c_str(), cfg.port)) {
        TRACER_ERROR("net_server listen failed addr: %s:%d\n", cfg.ip.c_str(), cfg.port);
        exit(1);
    }
    
    load_config("game_server", cfg);
    int32_t fd = -1;
    if ((fd = net_server.net_connect(cfg.ip.c_str(), cfg.port)) < 0) {
        TRACER_ERROR("net_server connect game_server failed addr: %s:%d\n", cfg.ip.c_str(), cfg.port);
        exit(1);
    }

    g_connet_server[GAME_SERVER] = fd;

    std::thread t1(&Net::product_msg, &net_server);
    std::thread t2(&gate_server::run_server, &server);

    std::thread t3(&Net::product_msg, &net_client);
    std::thread t4(&gate_server::run_client, &server);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

}