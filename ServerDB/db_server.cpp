#include <vector>
#include <unistd.h>

#include "db_server.h"
#include "../Config/loadconfig.h"
#include "../Net/tcp_socket.h"
#include "../Proto/PlayerInfo.pb.h"
#include "../Net/byte_swap.hpp"
#include "../SQL/sql_pb_swap.hpp"
#include "../Common/basetype.h"
#include "../Common/log.h"

db_server::db_server() 
{

}

db_server::~db_server()
{

}

int8_t db_server::init()
{   
    TRACER("server init start connect sql\n");
    sql_config sql_cfg;
    if (load_config("sql", sql_cfg)) {
        TRACER_ERROR("load sql config failed");
        return -1;
    }
    if (m_sql.Init(sql_cfg.db, sql_cfg.ip, sql_cfg.usr, sql_cfg.pass) < 0) {
        return -1;
    }
    TRACER("server init start connect sql success\n");
    return 0;
}


int8_t db_server::run()
{
    message *msg = nullptr;
    for (;;) {
        if ((msg = g_recv_queue.dequeue()) == nullptr) {
            
            usleep(100 * 1000);;
            continue;
        }
        TRACER_DEBUG("msg type is %d\n", msg->m_head.m_type);

        switch (msg->m_head.m_type) {
            case GETPASS:
                get_pass(msg);
                break;
            case SETPASS:
                set_pass(msg);
                break;
            case GETITEM:
                get_item(msg);
                break;
            case SETITEM:
                set_item(msg);
                break;
            case GETPLAYER:
                get_player(msg);
                break;
            case SETPLAYER:
                set_player(msg);
                break;
            case GET_ALLINFO:
                TRACER_DEBUG("just for Debug !! %s:%d\n", __POSITION__);
                get_all(msg);
                break;
            case SET_ALLINFO:
                set_all(msg);
                break;
            default:
                break;
        }

        msg->m_flag = msg_flags::INACTIVE;
    }
}


void db_server::db_reply(message* _msg, int _type)
{
    _msg->m_head.m_type = _type;
    _msg->m_head.m_len = 0;
    _msg->encode();
    tcp_socket::tcp_send(_msg->m_from, _msg->m_data, MSG_HEAD_SIZE);
}

int8_t db_server::set_pass(message* _msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;
    m_passdb[usrid]->pass = ntoh_32(*(int32_t*)(_msg->m_pdata));

    db_reply(_msg, DB_SUCCESS);
    return 0;
}


int8_t db_server::get_player_from_sql(uint32_t _usrid)
{
    std::vector<PLAYER> player;
    char cmd[100];
    snprintf(cmd, 100, "SELECT * FROM PLAYER where id = %d", _usrid);


    m_sql.GetBySQL(player, cmd);
    if (player.size() == 0) {
        TRACER_ERROR("can't find player from sql, usrid: %d\n", _usrid);
        return -1;
    }
    
    TRACER_DEBUG("player name: %s, id: %d\nposx: %f, posz: %f, speed: %d\n", player[0].name.c_str(), player[0].id, player[0].posx, player[0].posz, player[0].speed);
    m_playerdb.insert(std::make_pair(_usrid, std::make_shared<PLAYER>(player[0])));

    return 0;
}

int8_t db_server::get_player_allitems_from_sql(uint32_t _usrid)
{
    std::vector<ITEM> vitems;
    char cmd[100];
    snprintf(cmd, 100, "SELECT * FROM ITEM where usrid = %d", _usrid);
    

    m_sql.GetBySQL(vitems, cmd);
    if (vitems.size() == 0) {
        TRACER_ERROR("no items info for player id =  %d", _usrid);
        // return -1; 不返回错误， insert 一个空的map
    }

    auto mitems = std::make_shared<std::unordered_map<uint32_t, ITEM>>();
    for (auto&x : vitems) {
        // If I moved the ith element out, then ith slot goes into an undefined but valid state, 未定义但有效
        mitems->insert(std::make_pair(x.itemid, std::move(x)));
    }

    m_itemdb.insert(std::make_pair(_usrid, mitems)); // 没物品时是空的

    return 0;
}


int8_t db_server::get_pass(message *_msg)
{
    TRACER_DEBUG("try get pass start \n");
    uint32_t usrid = _msg->m_head.m_usrID;
    
    auto iter = m_passdb.find(usrid);
    if (iter == m_passdb.end()) {
        TRACER_DEBUG("db server not find , try to sql find usrid is %d\n", usrid);

        char cmd[100] = {0};
        snprintf(cmd, 100, "SELECT * FROM PASS where id = %d", usrid);
        std::vector<PASS> pass;
        m_sql.GetBySQL(pass, cmd);

        if (pass.size() == 0) {
            TRACER_ERROR("no pass for usrid %d\n", usrid);
            db_reply(_msg, DB_FAILED);
        }
        TRACER_DEBUG("usrid is %d, pass is %d\n", pass[0].id, pass[0].pass);

        auto ret = m_passdb.insert(std::make_pair(usrid, std::make_shared<PASS>(pass[0])));
        if (!ret.second) {
            TRACER_ERROR("------BUG BUG BUG-----\n%s:%d\n", __POSITION__);
        }
    } 

    *(int32_t*)(_msg->m_pdata) = hton_32(m_passdb[usrid]->pass);

    _msg->m_head.m_type = GETPASS;

    _msg->m_head.m_len = 4;

    _msg->encode();

    tcp_socket::tcp_send(_msg->m_from, _msg->m_data, 4 + MSG_HEAD_SIZE);
    
    TRACER_DEBUG("try get pass end\n");
    
    return 0;
}


int8_t db_server::get_item(message *_msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;

    uint32_t itemid = ntoh_32(*(uint32_t*)(_msg->m_pdata));

    auto iter_usr = m_itemdb.find(usrid);

    if (iter_usr == m_itemdb.end()) {
        if (get_player_allitems_from_sql(usrid) < 0) {
            TRACER_ERROR("no item for player id = %d\n", usrid);
            db_reply(_msg, DB_FAILED);
            return -1;
        }
    }


    auto item = m_itemdb[usrid]->find(itemid);

    if (item == m_itemdb[usrid]->end()) {
        TRACER_ERROR("no item for player id = %d\n", usrid);
        db_reply(_msg, DB_FAILED);
        return -1;
    }

    message *msg = nullptr;

    while ((msg = g_send_queue.enqueue()) == nullptr)
    {
        // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
        TRACER_ERROR("sleep 50ms, g_recv_queue is full, usrid is %d\n", usrid);
        g_send_queue.debug_info();
        usleep(50 * 1000);
    }
    
    
    msg->m_head.m_type = GETITEM;  // 如果用 db_success 请求方需保存上一次的请求类型，暂时用 errid==0 表示成功
    msg->m_head.m_errID = 0;
    msg->m_head.m_usrID = usrid;
    msg->m_head.m_len = m_itempb.ByteSizeLong();
    msg->m_to = _msg->m_from;
    
    msg->encode_pb(m_itempb);
    msg->m_flag = msg_flags::ACTIVE;

    return 0;
}


int8_t db_server::set_item(uint32_t _usrid, const ITEM& _itemsql)
{
    auto iter_usr = m_itemdb.find(_usrid);
    if (iter_usr != m_itemdb.end()) {
        auto iter_item = iter_usr->second->find(_itemsql.itemid);
        if (iter_item != iter_usr->second->end()) {
            if (!m_sql.ModBySQL(iter_item->second, m_itemsql)) {
                TRACER_ERROR("modbysql failed, try again. %s:%d\n", __POSITION__);
                m_sql.ModBySQL(iter_item->second, m_itemsql);
            }
            
            iter_item->second = m_itemsql;
            return 0;
        }
    }

    return -1;
}


int8_t db_server::set_item(message *_msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;

    _msg->decode_pb(m_itempb);
    
    pb2sql(&m_itempb, &m_itemsql);

    if (set_item(usrid, m_itemsql) < 0) {
        db_reply(_msg, DB_FAILED); // 失败才返回消息
        return -1;
    }

    return 0;
}

int8_t db_server::add_item(message *_msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;

    _msg->decode_pb(m_itempb);

    pb2sql(&m_itempb, &m_itemsql);

    auto items = m_itemdb.find(usrid);
    if (items != m_itemdb.end()) {
        m_itemdb[usrid]->insert(std::make_pair(m_itemsql.itemid, m_itemsql));
        if (!m_sql.SetBySQL(m_itemsql)) {
            TRACER_ERROR("sql.setbtsql failed! try again. %s:%d\n", __POSITION__);
            m_sql.SetBySQL(m_itemsql);
        }
    }

    db_reply(_msg, DB_SUCCESS);

    return 0;
}



int8_t db_server::get_player(message *_msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;

    auto iter = m_playerdb.find(usrid);
    if (iter == m_playerdb.end()) {
        if (get_player_from_sql(usrid) < 0) {
            TRACER_ERROR("can't find player from db & sql, player id = %d\n", usrid);
            return -1;
        }
    }

    sql2pb(iter->second, &m_playerpb);

    message *msg = nullptr;

    while ((msg = g_send_queue.enqueue()) == nullptr)
    {
        // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
        TRACER_ERROR("sleep 50ms, g_recv_queue is full, usrid is %d\n", usrid);
        g_send_queue.debug_info();
        usleep(50 * 1000);
    }

    msg->m_head.m_usrID = usrid;
    msg->m_head.m_type = GETPLAYER;
    msg->m_head.m_errID = 0;
    msg->m_to = _msg->m_from;
    msg->encode_pb(m_playerpb);
    msg->m_flag = msg_flags::ACTIVE;

    return 0;
}

int8_t db_server::set_player(uint32_t _usrid, const PLAYER& _playerslq)
{
    auto player = m_playerdb.find(_usrid);
    if (player != m_playerdb.end()) {
        if (!m_sql.ModBySQL(*(player->second), _playerslq)) {
            TRACER_ERROR("db_server::set_player, modbysql failed, try again. %s:%d\n", __POSITION__);
            m_sql.ModBySQL(*(player->second), _playerslq);
        };

        *(player->second) = m_playersql;
        return 0;
    }
    return -1;
}



int8_t db_server::set_player(message *_msg)
{
    uint32_t usrid  = _msg->m_head.m_usrID;
    
    _msg->decode_pb(m_playerpb);
    pb2sql(&m_playerpb, &m_playersql);

    if (set_player(usrid, m_playersql) < 0) {
        db_reply(_msg, DB_FAILED);
        return 0;
    }

    return -1;
}





int8_t db_server::get_all(message *_msg)
{

    Proto::Unity::PlayerAllFuckInfo player_all_info;
    
    uint32_t usrid = _msg->m_head.m_usrID;

    auto player_iter = m_playerdb.find(usrid);

    if (player_iter == m_playerdb.end()) {
        if (get_player_from_sql(usrid) < 0) {
            TRACER_ERROR("can't find player id: %d\n", usrid);
            return -1;
        } else {
            player_iter = m_playerdb.find(usrid);
        }
    }

    if (player_iter != m_playerdb.end()) {
        sql2pb(player_iter->second, player_all_info.mutable_baseinfo());
    } else {
        TRACER_ERROR("no player info. player id is %d", usrid);
        return -1;
    }

    TRACER_DEBUG("just for Debug !! %s:%d\n", __POSITION__);

    auto items_iter = m_itemdb.find(usrid);
    if (items_iter == m_itemdb.end()) {
        if (get_player_allitems_from_sql(usrid) < 0) {
            TRACER_ERROR("can't find player items from db & sql, inventory is empty! player id = %d\n", usrid);
            // return -1;
        } else {
            items_iter = m_itemdb.find(usrid);
        }
    } 
    TRACER_DEBUG("just for Debug !! %s:%d\n", __POSITION__);

    if (items_iter != m_itemdb.end()) {
        for (auto item = items_iter->second->cbegin(), end = items_iter->second->cend(); item != end; ++item) {
            auto *pb = player_all_info.mutable_baginfo()->add_items();
            sql2pb(&item->second, pb);
        }
    } else {
        TRACER_DEBUG("no items for player id: %d\n", usrid);
    }
    TRACER_DEBUG("just for Debug !! %s:%d\n", __POSITION__);

    message *msg = nullptr;

    while ((msg = g_send_queue.enqueue()) == nullptr)
    {
        // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
        TRACER_ERROR("sleep 50ms, g_recv_queue is full, usrid is %d\n", usrid);
        g_send_queue.debug_info();
        usleep(50 * 1000);
    }
    TRACER_DEBUG("just for Debug !! %s:%d\n", __POSITION__);

    msg->m_head.m_type = GET_ALLINFO;
    msg->m_head.m_usrID = usrid;
    msg->m_head.m_errID = 0;
    msg->m_to = _msg->m_from;
    msg->encode_pb(player_all_info);
    msg->m_flag = msg_flags::ACTIVE;

    TRACER_DEBUG("just for Debug !! %s:%d\n", __POSITION__);

    return 0;
}

int8_t db_server::set_all(message* _msg)
{   
    Proto::Unity::PlayerAllFuckInfo player_all_info;

    uint32_t usrid = _msg->m_head.m_usrID;
    
    TRACER_DEBUG("usrid %d save allinfo\n", usrid);

    _msg->decode_pb(player_all_info);


    pb2sql(&player_all_info.baseinfo(), &m_playersql);

    set_player(usrid, m_playersql);

    // todo 待优化
    for (const Proto::Unity::ItemInfo& x : player_all_info.baginfo().items()) {
        pb2sql(&x, &m_itemsql);
        set_item(usrid, m_itemsql);
    }

    // db_reply(_msg, DB_SUCCESS);

    return 0;
}


int8_t db_server::saveall()
{
    //todo 把所有信息写回数据库
    return 0;
}