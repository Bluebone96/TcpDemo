#include "../Common/base.h"
#include "../Common/Epoll.h"
#include "../Common/log.h"
#include "../Common/TcpSocket.h"
#include "../Common/MsgRecord.h"
#include "../Common/MsgTransmission.h"


class Client {
public:
    static Client& GetInstance()
    {
        static Client Instance;
        return Instance;
    }

    ~Client() {};
    int32_t Init(const char* hostname, int16_t port);
    int32_t GOGOGO();
    
    void Test();

    int32_t SendMsg();
    int32_t RcvMsg();


private:
    
    Client(){};
    MsgTrans m_MsgTrans;
    Data m_data;
    Epoll m_epoll;
    friend std::ostream& operator<<(std::ostream& os, Client& ct);
    friend std::istream& operator>>(std::istream& is, Client& ct);
};

#define CLIENT Client::GetInstance()