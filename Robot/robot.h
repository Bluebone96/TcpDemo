#ifndef _ROBOT_H_

#include <unistd.h>

#include "../Net/tcp_socket.h"
#include "../Net/message.h"
#include "../Proto/PlayerInfo.pb.h"



class Robot {
public:
    Robot(int);
    ~Robot();

    bool init_net();

    bool login(std::string& _ip, uint32_t _port);
    bool exit();

    void send(char* _buf, uint32_t _len);
    void send(const google::protobuf::Message& rhs);

    void recv(char* _buf, uint32_t _len);
private:
    std::string m_name;
    std::string m_pass;
    uint32_t m_id;

    tcp_socket* m_socket;

    message m_msg;
    Proto::Unity::Operation m_op;
};


#endif
