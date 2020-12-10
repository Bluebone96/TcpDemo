#ifndef _LOGIN_SERVER_H_
#define _LOGIN_SERVER_H_

#include <map>
#include <memory>

#include "../Net/Net.h"
#include "../Proto/PlayerInfo.pb.h"
// #include "../SQL/tomysql.h"

extern msg_queue g_recv_queue;
extern msg_queue g_send_queue;
extern std::map<uint32_t, uint32_t> g_connet_server;

extern std::map<int32_t, Proto::Unity::ServerInfo> g_serverinfo;


class login_server {
public:
    login_server();
    ~login_server();

    int8_t run();

    int8_t login_request(message* _msg);

    int8_t login_verify(message* _msg);

    int8_t login_success(message* _msg);

    int8_t login_failed(message* _msg);

    
private:

    message m_msg;

    Proto::Unity::Authentication m_au;

    Proto::Unity::PlayerInfo m_player;
    Proto::Unity::PlayerBag m_bag;

    Proto::Unity::PlayerAllFuckInfo m_playerinfo;



    // 异步保留信息
    std::map<uint32_t, uint32_t> m_usrfd; 
    
    std::map<uint32_t, uint32_t> m_waitverify;
};

#endif

