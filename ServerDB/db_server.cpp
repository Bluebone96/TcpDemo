#include <unistd.h>

#include "db_server.h"
#include "../Config/loadconfig.hpp"
#include "../Proto/PlayerInfo.pb.h"
#include "../Common/basetype.h"


db_server::db_server() 
{

}

db_server::~db_server()
{

}

int8_t db_server::init()
{
    sql_config sql_cfg;
    if (m_sql.Init(sql_cfg.db, sql_cfg.ip, sql_cfg.usr, sql_cfg.pass) < 0) {
        return -1;
    }
}


int8_t db_server::run()
{
    message* msg;
    for (;;) {
        if ((msg = g_recv_queue.dequeue()) == nullptr) {
            sleep(2);
        }
        switch (msg->m_head.m_type) {
            case GETPASS:
                {
                    auto iter = passdb.find(msg->m_head.m_usrID);
                    if (iter != passdb.end()) {
                        
                    }
                }
            case SETPASS:
            case GETITEM:
            case SETITEM:
            case GETPLAYER:
            case SETPLAYER:
            default:

        }
    }
}

