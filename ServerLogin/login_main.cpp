#include "login_server.h"
#include "../Net/net.h"



msg_queue g_recv_queue;
msg_queue g_send_queue;

std::map<uint32_t, std::shared_ptr<tcp_socket>> g_connections;


int main()
{
    
}