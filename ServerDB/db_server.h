#ifndef _DB_SERVER_H_
#define _DB_SERVER_H_

#include "../Net/message.h"
#include "../Proto/PlayerInfo.pb.h"

#define MYSQLPP_SSQLS_NO_STATICS
#include "../SQL/tomysql.h"

extern msg_queue g_recv_queue;
extern msg_queue g_send_queue;


class db_server {
public:
    db_server();
    ~db_server();

    int8_t init();
    int8_t run();

    int8_t set_pass(message* _msg);
    int8_t get_pass(message* _msg);

    int8_t set_player(message* _msg);
    int8_t get_player(message* _msg);
    int8_t add_player(message* _msg);

    int8_t set_item(message* _msg);
    int8_t get_item(message* _msg);
    int8_t add_item(message* _msg);

    int8_t get_all(message* _msg);
    int8_t set_all(message* _msg);


    void db_reply(message* _msg, int _type);

    template<typename T>
    int8_t save2sql(T& _old, T _new)
    {
        return m_sql.ModBySQL(_old, _new);
    }

    int8_t saveall();

private:
    ToMysql m_sql;

    Proto::Unity::Authentication m_authpb;
    Proto::Unity::ItemInfo m_itempb;
    Proto::Unity::PlayerInfo m_playerpb;
    
    PASS m_passslq;
    ITEM m_itemslq;
    PLAYER m_playersql;

    std::map<uint32_t, PASS> m_passdb;
    std::map<uint32_t, std::map<uint32_t, ITEM>> m_itemdb;
    std::map<uint32_t, PLAYER> m_playerdb;

    std::map<uint32_t, PASS> m_passdb_bak;
    std::map<uint32_t, std::map<uint32_t, ITEM>> m_itemdb_bak;
    std::map<uint32_t, PLAYER> m_playerdb_bak;

    std::map<uint32_t, Proto::Unity::PlayerAllFuckInfo> m_allplayer_info; // 是否改为指针 map value 是栈 堆？
};

#endif
