#ifndef _SERVER_H_
#define _SERVER_H_

#include <memory>
#include <unordered_map>

#include "../Common/Singleton.h"
#include "../Common/base.h"
#include "../Common/Epoll.h"
#include "../Common/log.h"
#include "../Common/TcpSocket.h"
#include "../Common/MsgRecord.h"
#include "../Common/MsgTransmission.hpp"
#include "../Player/Player.h"
//#include "../Handler/EventHandler.h"

//#include "../Status/"

class Server {
public:

    // static Server& GetInstance ()
    // {
    //     static Server Instance;
    //     return Instance;
    // }

    Server() {  };
    
    ~Server() {};

    int32_t GOGOGO();

    int32_t Init(int32_t port, const char* hostname = nullptr);

    int32_t RecvMsg();

    int32_t SendMsg();

    int32_t SendMsgToAll(char*, int);

    int32_t SendMsgToOne(int, char*, int);

    int32_t AcceptNewClient();

    typedef std::unordered_map<int32_t, Player*> PLAYERMAP;

//     Server(const Server&);
//     Server& operator=(const Server);

private:

    PLAYERMAP m_players;

    int32_t m_listenfd;

    Epoll m_epoll;

    TcpSocket m_socket;
        
    friend class Dispatcher; // 
};

// #define SERVER Server::GetInstance()

#define SERVER Singleton<Server>::GetInstance()

#endif
