#ifndef _GAME_SERVER_H_
#define _GAME_SERVER_H_

#include <map>

#include "../Player/Player.h"
#include "../Net/message.h"

extern msg_queue g_recv_queue;
extern msg_queue g_send_queue;
extern std::map<uint32_t, uint32_t> g_connet_server;

class game_server {
public:
    game_server();
    ~game_server();
    void run();

private:
    int8_t usr_login(message *msg);

    int8_t usr_exit(message *msg);

    int8_t usr_update_status(message *msg);

    int8_t usr_alive(message *msg);

    int8_t usr_sync(uint32_t _usrid);

    int8_t usr_update_item(message *msg);

    int8_t usr_chat(message *msg);

    std::map<uint32_t, Player*> m_players;
};

#endif