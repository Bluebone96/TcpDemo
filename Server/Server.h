#include <memory>
#include <unordered_map>
#include "../Common/base.h"

#include "../Common/Epoll.h"
#include "../Common/log.h"
#include "../Common/TcpSocket.h"
#include "../Common/MsgRecord.h"
#include "../Common/MsgTransmission.h"

class Server {
public:
    int32_t Init();
    int32_t RecvMsg();
    int32_t SendMsg();
    int32_t ProcessData(); 
    int32_t GOGOGO();
    
    ~Server();

    static Server& GetInstance ()
    {
        static Server Instance;
        return Instance;
    }
    
private:
    Server();
    Server(const Server&);
    Server& operator=(const Server);

private:
    int32_t listenfd;
    Epoll m_epoll;
    MsgTrans m_MsgTrans;
    Data m_data;
    std::unordered_map<uint32_t, std::shared_ptr<TcpSocket>> m_clients;
};

#define SERVER Server::GetInstance()
