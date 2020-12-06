#include <unistd.h>
#include <vector>


#include "login_server.h"
#include "../Proto/PlayerInfo.pb.h"
#include "../Common/basetype.h"
#include "../SQL/tomysql.h"


login_server::login_server()
{

}

login_server::~login_server()
{

}

// int8_t login_server::init()
// {

// }


int8_t login_server::run()
{
    message* msg = nullptr;
    for (;;) {
        if ((msg = g_recv_queue.dequeue()) == nullptr) {
            sleep(2);
            continue;
        }

        switch (msg->m_head.m_type)
        {
        case LOGIN_REQUEST:
            login_request(msg);
            break;
        case GETPASS:
            login_verify(msg);
        case GET_ALLINFO:
            login_success(msg);
        default:
            break;
        }        

        msg->setinvalid();
    }
}



// int8_t login_server::run()
// {
//         for (;;) {
//         int32_t fn = m_epoll.Wait();
//         for (int32_t i = 0; i < fn; ++i) {
//             struct epoll_event* pEvent = m_epoll.GetEvent(i);

//             if (pEvent->data.fd == m_listenfd.getfd()) {
                
//                 uint32_t newclient = m_listenfd.tcp_accept(nullptr, nullptr);
//                 if (newclient > 0) {
//                     auto re = m_connections.insert(std::make_pair(newclient, std::make_shared<tcp_socket>(new tcp_socket())));
//                     if (re.second == false) {
//                         TRACER("insert failed\n");
//                     }
//                     m_connections[newclient]->tcp_init(newclient);
//                 }
//                 TRACER("new client\n");
//             } else {
//                 uint32_t event = pEvent->events;
//                 int32_t fd = pEvent->data.fd;
//                 switch (event)
//                 {
//                 case EPOLLIN:
//                     TRACER("Epoll event EPOLLIN fd = %d\n", fd);
//                     break;
//                 case EPOLLHUP:
//                     TRACER("Epoll event epollhup fd = %d\n", fd);
//                     break;
//                 case EPOLLERR:
//                     TRACER("Epoll event epollerr fd= %d, delete ptr\n", fd);
//                     m_connections.erase(fd);
//                     continue;
//                 default:
//                     TRACER("Epoll unknowen, fd = %d, delete ptr\n", fd);
//                     m_connections.erase(fd);
//                     continue;
//                 }
//                 auto socket = m_connections[fd];

//                 if (Net::recvmsg(socket, m_msg) < 0) {
//                     m_connections.erase(fd);
//                     continue;
//                 }
                
//                 if (login_request(&m_msg) < 0) {
//                     m_connections.erase(fd);
//                     continue;
//                 }

//                 if (Net::sendmsg(socket, m_msg) < 0) {
//                     m_connections.erase(fd);
//                     continue;
//                 }
//             }
//         }
//     }
//}

static unsigned int BKDRHash(const std::string& _str)
{
    unsigned int seed = 1313;
    unsigned int hash = 0;
    for (auto& c : _str) {
        hash = hash * seed + c;
    }
    return (hash & 0x7FFFFFFF);
}


// int8_t login_server::login_request(message* _msg)
// {
//     m_au.ParseFromArray(_msg->m_data + MSG_HEAD_SIZE, _msg->m_head.m_len);
    
//     unsigned int id = BKDRHash(m_au.name());
//     unsigned int pass = BKDRHash(m_au.password());

//     std::vector<PASS> passSql;


//     char cmd[50];

//     snprintf(cmd, 50, "select * from pass where id = %d", id);

//     if (m_sql.GetBySQL(passSql, cmd) < 0) {
//         return -1;
//     }

//     if ((unsigned int)(passSql[0].pass) != pass) {
//         return -1;
//     }

//     std::vector<PLAYER> playerSql;

//     snprintf(cmd, 50, "SELECT * FROM PLAYER where id = %d", id);
//     if (m_sql.GetBySQL(playerSql, cmd) < 0) {
//         return -1;
//     }

//     m_player.set_id(id);
//     m_player.set_name(m_au.name());
//     m_player.set_hp(playerSql[0].hp);
//     m_player.set_posx(playerSql[0].posx);
//     m_player.set_posz(playerSql[0].posz);
//     m_player.set_speed(playerSql[0].speed);
//     m_player.set_state(0);
    
//     m_playerinfo.mutable_baseinfo()->operator=(m_player);

//     std::vector<ITEM> itemsql;

//     snprintf(cmd, 60, "SELECT * FROM ITEM where userid = %d", id);

//     for (auto&x : itemsql) {
//         Proto::Unity::ItemInfo *pitem = m_bag.add_items();
//         sql2pb(x, pitem);
//     }

//     m_playerinfo.mutable_baginfo()->operator=(m_bag);

//     _msg->m_head.m_type = LOGIN_SUCCESS;


//     _msg->m_head.m_len = (m_playerinfo.ByteSizeLong() + MSG_HEAD_SIZE);

//     m_playerinfo.SerializeToArray(_msg->m_data + MSG_HEAD_SIZE, 1024);

//     _msg->encode();

//     // message* msg = g_send_queue.dequeue();
//     // if (msg == nullptr) {
//     //     return -1;
//     // }

    

//     // msg->m_head.m_type = GETPASS;

//     // msg->m_head.m_len = MSG_HEAD_SIZE;
// }

// static int sql2pb(ITEM& _itemsql, Proto::Unity::ItemInfo* _itempb)
// {
//     _itempb->set_m_uid(_itemsql.itemid);
//     _itempb->set_m_count(_itemsql.count);
//     _itempb->set_m_type(_itemsql.type);
//     _itempb->set_m_hp(_itemsql.hp);
//     _itempb->set_m_atk(_itemsql.atk);

//     return 0;
// }

int8_t login_server::login_request(message* _msg)
{
    m_au.ParseFromArray(_msg->m_data + MSG_HEAD_SIZE, _msg->m_head.m_len);
    
    unsigned int id = BKDRHash(m_au.name());
    unsigned int pass = BKDRHash(m_au.password());

    m_waitverify.insert(std::make_pair(id, pass));

    message *msg = g_send_queue.enqueue();
    msg->m_head.m_type = GETPASS;
    msg->m_from = LOGIN_SERVER;
    msg->m_to = DB_SERVER;
    msg->m_head.m_usrID = id;
    msg->m_head.m_errID = 0;
    msg->m_head.m_len = MSG_HEAD_SIZE;
    msg->decode();
    msg->setvalid();


    return 0;
}

int8_t login_server::login_verify(message* _msg)
{
    if (_msg->m_head.m_errID) {
        login_failed(_msg);
        m_waitverify.erase(_msg->m_head.m_usrID);
        return -1;
    } 
    uint32_t key = ntoh_32(*(uint32_t*)(_msg->m_pdata));

    if (m_waitverify[_msg->m_head.m_usrID] != key) {
        login_failed(_msg);
        m_waitverify.erase(_msg->m_head.m_usrID);
        return -1;
    }

    m_usrfd[_msg->m_head.m_usrID] = _msg->m_from;
    
    message *msg = g_send_queue.enqueue();
    msg->m_head.m_type = GET_ALLINFO;
    msg->m_from = LOGIN_SERVER;
    msg->m_to = DB_SERVER;
    msg->m_head.m_usrID = _msg->m_head.m_usrID;
    msg->m_head.m_errID = 0;
    msg->m_head.m_len = 0;
    msg->decode();
    msg->setvalid();

    return 0;

}

int8_t login_server::login_failed(message* _msg)
{
    message* msg = g_send_queue.enqueue();
    
    msg->m_head.m_type = LOGIN_FAILED;
    msg->m_from = LOGIN_SERVER;
    msg->m_head.m_usrID = _msg->m_head.m_usrID;
    msg->m_to = m_usrfd[_msg->m_head.m_usrID];
    msg->m_head.m_errID = _msg->m_head.m_errID;
    msg->m_head.m_len = 0;
    msg->decode();
    msg->setvalid();

    return 0;
}


int8_t login_server::login_success(message* _msg)
{
    if (_msg->m_head.m_errID) {
        return -1;
    }

    return tcp_socket::tcp_send(m_usrfd[_msg->m_head.m_usrID], _msg->m_data, _msg->m_head.m_len + MSG_HEAD_SIZE);
}