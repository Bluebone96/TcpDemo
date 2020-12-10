#include <unistd.h>

#include "game_server.h"
#include "../Net/tcp_socket.h"

game_server::game_server()
{

}

game_server::~game_server()
{

}


void game_server::run()
{
    message *msg = nullptr;

    for(;;) {
        msg = g_recv_queue.dequeue();
        if (msg == nullptr) {
            usleep(50 * 1000);;
            continue;
        }
        
        switch (msg->m_head.m_type)
        {
            case GET_ALLINFO:
                usr_login(msg);
                break;
            case USERUP:
                usr_update_status(msg);
                break;
            case USEREXIT:
                usr_update_item(msg);
                break;
            case USERCHAT:
                usr_chat(msg);
                break;
            case ITEMEVENT:
                usr_update_item(msg);
                break;
            case USERALIVE:
                usr_alive(msg);
                break;
            default:
                // todo
                break;
        }

        msg->m_flag = msg_flags::INACTIVE;
    }
}



int8_t game_server::usr_login(message *_msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;
    
    Player *p = new Player;

    Proto::Unity::PlayerAllFuckInfo allinfo;

    _msg->decode_pb(allinfo);
    // allinfo.ParseFromArray(_msg->m_pdata, _msg->m_head.m_len);

    p->InitPlayer(allinfo);

    m_players[usrid] = p;
    
    return 0;
}


int8_t game_server::usr_exit(message *_msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;

    auto iter = m_players.find(usrid);
    if (iter != m_players.end()) {
        message *msg = nullptr;

        while ((msg = g_send_queue.enqueue()) == nullptr)
        {
            // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
            TRACER_ERROR("sleep 50ms, g_recv_queue is full, usrid is %d\n", usrid);
            g_send_queue.debug_info();
            usleep(50 * 1000);
        }

        msg->m_head.m_type = SET_ALLINFO;
        msg->m_head.m_usrID = usrid;
        msg->m_head.m_errID = 0;
        msg->m_to = g_connet_server[DB_SERVER];
        msg->encode_pb(iter->second->getAllFuckInfo());
        msg->m_flag = msg_flags::ACTIVE;
    
        // todo 是否等待数据库返回确认消息后再释放内存
        m_players.erase(iter);


        // 通知客户端，有玩家下线
        while ((msg = g_send_queue.enqueue()) == nullptr)
        {
            // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
            TRACER_ERROR("sleep 50ms, g_recv_queue is full, usrid is %d\n", usrid);
            g_send_queue.debug_info();
            usleep(50 * 1000);
        }

        msg->m_head.m_type = USEREXIT;
        msg->m_head.m_usrID = usrid;
        msg->m_head.m_len = 0;
        msg->m_head.m_errID = 0;
        msg->m_to = _msg->m_from;
        msg->encode();
        msg->m_flag = msg_flags::ACTIVE;
        
        return 0;

    }
    return -1;
}



int8_t game_server::usr_alive(message *_msg)
{
    
    uint32_t usrid = _msg->m_head.m_usrID;
    auto iter = m_players.find(usrid);
    if (iter != m_players.end()) {
        iter->second->Activity();
    }

    return 0;
}
int8_t game_server::usr_update_status(message *_msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;

    Proto::Unity::Operation op;
    _msg->decode_pb(op);

    m_players[usrid]->update_status(op);

    usr_sync(usrid, _msg->m_from);
    return 0;
}

int8_t game_server::usr_sync(uint32_t _usrid, uint32_t _fd)
{
    
    message *msg = nullptr;

    while ((msg = g_send_queue.enqueue()) == nullptr)
    {
        // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
        TRACER_ERROR("sleep 50ms, g_recv_queue is full, usrid is %d\n", _usrid);
        g_send_queue.debug_info();
        usleep(50 * 1000);
    }

    const Proto::Unity::PlayerInfo* info = m_players[_usrid]->GetPlayerInfo();
    msg->m_head.m_type = USERSYNC;
    msg->m_head.m_usrID = _usrid;
    msg->m_head.m_errID = 0;
    msg->m_to = _fd;

    msg->encode_pb(*info);

    msg->m_flag = msg_flags::ACTIVE;
    TRACER_DEBUG("USR_SYNC END, usrid is %d\n", _usrid);
    return 0;
}

int8_t game_server::usr_update_item(message *_msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;

    Proto::Unity::ItemUpdate pb;
    _msg->decode_pb(pb);
    if ((m_players[usrid]->update_Inventory(pb)) < 0) {
        return -1;
    }
    
    tcp_socket::tcp_send(_msg->m_from, _msg->m_data, _msg->m_head.m_len + MSG_HEAD_SIZE);

    return -1;
}


int8_t game_server::usr_chat(message *_msg)
{
    // uint32_t usrid = _msg->m_head.m_usrID;
    // todo

    return 0;
}