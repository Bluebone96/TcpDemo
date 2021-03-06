#ifndef _DB_SERVER_H_
#define _DB_SERVER_H_

#include <memory>
#include <unordered_map>

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

    int8_t get_player_from_sql(uint32_t);
    int8_t get_player_allitems_from_sql(uint32_t);


    int8_t set_pass(message* _msg);
    int8_t get_pass(message* _msg);

    int8_t set_player(uint32_t, const PLAYER&);
    int8_t set_player(message* _msg);

    int8_t get_player(message* _msg);
    
    int8_t add_player(message* _msg);

    int8_t set_item(uint32_t, const ITEM&);
    int8_t set_item(message* _msg);
    int8_t get_item(message* _msg);
    
    int8_t add_item(message* _msg);

    int8_t get_all(message* _msg);
    int8_t set_all(message* _msg);

    void db_reply(message* _msg, int _type);

    int8_t saveall();
private:
    ToMysql m_sql;

    Proto::Unity::Authentication m_authpb;
    Proto::Unity::ItemInfo m_itempb;
    Proto::Unity::PlayerInfo m_playerpb;
    Proto::Unity::PlayerAllFuckInfo m_allinfopb;
    
    PASS m_passsql;
    ITEM m_itemsql;
    PLAYER m_playersql;

    std::unordered_map<uint32_t, std::shared_ptr<PASS>> m_passdb;
    std::unordered_map<uint32_t, std::shared_ptr<std::unordered_map<uint32_t, ITEM>>> m_itemdb;
    std::unordered_map<uint32_t, std::shared_ptr<PLAYER>> m_playerdb;

};

#endif
