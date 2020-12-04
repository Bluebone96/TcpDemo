#include "db_server.h"
#include "../Config/loadconfig.hpp"
#include "../Net/Net.h"
#include "../Net/message.h"

#include <map>



msg_queue g_recv_queue;
msg_queue g_send_queue;



int main()
{
    db_server db;
    Net _net;


    
}