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
    load_config("sql", sql_cfg);
    if (m_sql.Init(sql_cfg.db, sql_cfg.ip, sql_cfg.usr, sql_cfg.pass) < 0) {
        return -1;
    }
    TRACER("server init start connect sql success\n");
    return 0;
}


int8_t db_server::run()
{
    message* msg;
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
    m_passdb[usrid].pass = ntoh_32(*(int32_t*)(_msg->m_pdata));

    db_reply(_msg, DB_SUCCESS);
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
        snprintf(cmd, 100, "select * from PASS where id = %d", usrid);
        std::vector<PASS> pass;
        if (m_sql.GetBySQL(pass, cmd) < 0) {
            TRACER_ERROR("no pass for usrid %d\n", usrid);
            db_reply(_msg, DB_FAILED);
        }
        TRACER_DEBUG("usrid is %d, pass is %d\n", pass[0].id, pass[0].pass);
        auto ret = m_passdb_bak.insert(std::make_pair(usrid, pass[0]));
        if (!ret.second) {
            TRACER_ERROR("------BUG BUG BUG-----\n%s:%d\n", __POSITION__);
        }

        ret = m_passdb.insert(std::make_pair(usrid, pass[0]));
        if (!ret.second) {
            TRACER_ERROR("------BUG BUG BUG-----\n%s:%d\n", __POSITION__);
        }
    } 

    *(int32_t*)(_msg->m_pdata) = hton_32(m_passdb[usrid].pass);

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
    if (iter_usr != m_itemdb.end()) {
        auto iter_item = iter_usr->second.find(itemid);
        if (iter_item != iter_usr->second.end()) {
            sql2pb(iter_item->second, &m_itempb);
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
    } 

    std::vector<ITEM> items;
    char cmd[100] = {0};
    snprintf(cmd, 100, "SELECT * FROM ITEM where userid = %d, itemid = %d", usrid, itemid);
    if (m_sql.GetBySQL(items, cmd) < 0) {
        db_reply(_msg, DB_FAILED);
        return -1;
    }

    m_itemdb[usrid][itemid] = items[0];
    m_itemdb_bak[usrid][itemid] = items[0];

    sql2pb(items[0], &m_itempb);
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


int8_t db_server::set_item(message *_msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;

    _msg->decode_pb(m_itempb);
    
    pb2sql(&m_itempb, m_itemslq);

    auto iter_usr = m_itemdb.find(usrid);
    if (iter_usr != m_itemdb.end()) {
        auto iter_item = iter_usr->second.find(m_itemslq.itemid);
        if (iter_item != iter_usr->second.end()) {
            iter_item->second = m_itemslq;
            db_reply(_msg, DB_SUCCESS);
            return 0;
        }
    }
    db_reply(_msg, DB_FAILED);
    return -1;
}

int8_t db_server::add_item(message *_msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;

    _msg->decode_pb(m_itempb);

    pb2sql(&m_itempb, m_itemslq);
    m_itemslq.userid = usrid;

    m_itemdb[usrid].insert(std::make_pair(m_itemslq.itemid, m_itemslq));
    m_itemdb_bak[usrid].insert(std::make_pair(m_itemslq.itemid, m_itemslq));

    m_sql.SetBySQL(m_itemslq);

    db_reply(_msg, DB_SUCCESS);

    return 0;
}


int8_t db_server::get_player(message *_msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;

    auto iter = m_playerdb.find(usrid);
    if (iter == m_playerdb.end()) {
        std::vector<PLAYER> player;
        char cmd[100];
        snprintf(cmd, 100, "select * PLAYER where id = %d", usrid);

        if (m_sql.GetBySQL(player, cmd) < 0) {
            db_reply(_msg, DB_FAILED);
            return -1;
        }

        m_playerdb[usrid] = player[0];
        m_playerdb_bak[usrid] = player[0];
    }

    sql2pb(m_playerdb[usrid], m_playerpb);

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

int8_t db_server::set_player(message *_msg)
{
    uint32_t usrid  = _msg->m_head.m_usrID;

    Proto::Unity::PlayerInfo *info = m_allplayer_info[usrid].mutable_baseinfo();

    _msg->decode_pb(*info);

    db_reply(_msg, DB_SUCCESS);

    return 0;
}


int8_t db_server::get_all(message *_msg)
{

    uint32_t usrid = _msg->m_head.m_usrID;

    auto iter = m_allplayer_info.find(usrid);
    
    if (iter == m_allplayer_info.end()) {
        std::vector<PLAYER> player;
        char cmd[100] = {0};
        snprintf(cmd, 100, "select * from PLAYER where id = %d", usrid);
        m_sql.GetBySQL(player, cmd);
        Proto::Unity::PlayerAllFuckInfo player_all_info;
        sql2pb(player[0], *player_all_info.mutable_baseinfo());

        std::vector<ITEM> items;

        snprintf(cmd, 100, "select * from ITEM where userid = %d", usrid);

        m_sql.GetBySQL(items, cmd);

        for (auto& x : items) {
            Proto::Unity::ItemInfo *item = player_all_info.mutable_baginfo()->add_items();

            sql2pb(x, item);
        }

        m_allplayer_info.insert(std::make_pair(usrid, std::move(player_all_info)));
    }

    message *msg = nullptr;

    while ((msg = g_send_queue.enqueue()) == nullptr)
    {
        // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
        TRACER_ERROR("sleep 50ms, g_recv_queue is full, usrid is %d\n", usrid);
        g_send_queue.debug_info();
        usleep(50 * 1000);
    }

    msg->m_head.m_type = GET_ALLINFO;
    msg->m_head.m_usrID = usrid;
    msg->m_head.m_errID = 0;
    msg->m_to = _msg->m_from;
    msg->encode_pb(m_allplayer_info[usrid]);
    msg->m_flag = msg_flags::ACTIVE;

    return 0;
}

int8_t db_server::set_all(message* _msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;
    
    _msg->decode_pb(m_allplayer_info[usrid]);

    db_reply(_msg, DB_SUCCESS);

    return 0;
}


int8_t db_server::saveall()
{
    //todo
    return 0;
}