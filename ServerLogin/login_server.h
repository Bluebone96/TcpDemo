#ifndef _LOGIN_SERVER_H_
#define _LOGIN_SERVER_H_

#include <map>
#include <memory>

#include "../Net/net.h"
#include "../Proto/PlayerInfo.pb.h"
#include "../SQL/tomysql.h"

extern msg_queue g_recv_queue;
extern msg_queue g_send_queue;
extern std::map<uint32_t, std::shared_ptr<tcp_socket>> g_connections;


class login_server {
public:
    login_server();
    ~login_server();

    int8_t init();

    int8_t run();

    int8_t login_request(message* _msg);

private:

    ToMysql m_sql;

    message m_msg;


    Proto::Unity::Authentication m_au;
    Proto::Unity::PlayerInfo m_player;
    Proto::Unity::PlayerBag m_bag;

    Proto::Unity::PlayerAllFuckInfo m_playerinfo;



    Epoll m_epoll;
    tcp_socket m_listenfd;
    tcp_socket m_connectfd;
    std::map<uint32_t, std::shared_ptr<tcp_socket>> m_connections;
};

#endif

