#ifndef _DB_SERVER_H_
#define _DB_SERVER_H_

#include "../Net/message.h"
#include "../SQL/tomysql.h"
#include "../Proto/PlayerInfo.pb.h"


extern msg_queue g_recv_queue;
extern msg_queue g_send_queue;

static std::map<uint32_t, PASS> passdb;
static std::map<uint32_t, std::map<uint32_t, ITEM>> itemdb;
static std::map<uint32_t, PLAYER> playerdb;

class db_server {
public:
    db_server();
    ~db_server();

    int8_t init();
    int8_t run();

    template<typename T1>
    int8_t set(T1 _map, message* _msg)
    {
        T1
    }

    template<typename T1, typename T2>
    int8_t get(T1, T2&);
private:

    Proto::Unity::ItemInfo m_itempb;
    Proto::Unity::PlayerInfo m_playerpb;
    Proto::Unity::Authentication m_authpb;

    ToMysql m_sql;
};

#endif
