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



int8_t login_server::run()
{
    message* msg = nullptr;
    for (;;) {
        if ((msg = g_recv_queue.dequeue()) == nullptr) {
            usleep(100 * 1000);;
            continue;
        }

        switch (msg->m_head.m_type)
        {
            case LOGIN_REQUEST:
                TRACER_DEBUG("login server msg type is LOGIN_REQUEST\n");
                login_request(msg);
                break;
            case GETPASS:
                TRACER_DEBUG("login server msg type is GETPASS\n");
                login_verify(msg);
                break;
            case GET_ALLINFO:
                TRACER_DEBUG("login server msg type is ALLINFO\n");
                login_success(msg);
                break;
            default:
                TRACER_DEBUG("login server msg undefine type\n");
                break;
        }

        msg->setinvalid();
    }
}



static unsigned int BKDRHash(const std::string& _str)
{
    unsigned int seed = 1313;
    unsigned int hash = 0;
    for (auto& c : _str) {
        hash = hash * seed + c;
    }
    return (hash & 0x7FFFFFFF);
}



int8_t login_server::login_request(message* _msg)
{
    m_au.ParseFromArray(_msg->m_pdata, _msg->m_head.m_len);
    
    TRACER("usrname is %s, pass is %s, fd is %d\n", m_au.name().c_str(), m_au.password().c_str(), _msg->m_from);
    unsigned int id = BKDRHash(m_au.name());
    unsigned int pass = BKDRHash(m_au.password());

    // 记录
    TRACER_DEBUG("usrid:%d fd is : %d\n", _msg->m_head.m_usrID, _msg->m_from);
    m_usrfd[id] = _msg->m_from;
    m_waitverify.insert(std::make_pair(id, pass));


    message *msg = g_send_queue.enqueue();
    msg->m_head.m_type = GETPASS;
    // msg->m_from = LOGIN_SERVER;
    msg->m_to = g_connet_server[DB_SERVER];
    msg->m_head.m_usrID = id;
    msg->m_head.m_errID = 0;
    msg->m_head.m_len = 0;
    msg->encode();

    msg->setvalid();

    return 0;
}


int8_t login_server::login_verify(message* _msg)
{
    if (_msg->m_head.m_errID) {
        // login_failed(_msg); 客户端还没加这个登录失败消息
        m_waitverify.erase(_msg->m_head.m_usrID);
        return -1;
    } 
    uint32_t key = ntoh_32(*(uint32_t*)(_msg->m_pdata));

    if (m_waitverify[_msg->m_head.m_usrID] != key) {
        // login_failed(_msg);
        m_waitverify.erase(_msg->m_head.m_usrID);
        return -1;
    }
    
    message *msg = g_send_queue.enqueue();
    msg->m_head.m_type = GET_ALLINFO;
    // msg->m_from = LOGIN_SERVER;
    msg->m_to = g_connet_server[DB_SERVER];
    msg->m_head.m_usrID = _msg->m_head.m_usrID;
    msg->m_head.m_errID = 0;
    msg->m_head.m_len = 0;
    msg->encode();
    msg->setvalid();

    return 0;

}

int8_t login_server::login_failed(message* _msg)
{
    message* msg = g_send_queue.enqueue();
    
    TRACER_ERROR("usrid %d login failed fd is %d\n", _msg->m_head.m_usrID, m_usrfd[_msg->m_head.m_usrID]);
    msg->m_head.m_type = LOGIN_FAILED;
    // msg->m_from = LOGIN_SERVER;
    msg->m_head.m_usrID = _msg->m_head.m_usrID;
    msg->m_to = m_usrfd[_msg->m_head.m_usrID];
    msg->m_head.m_errID = _msg->m_head.m_errID;
    msg->m_head.m_len = 0;
    msg->encode();
    msg->setvalid();

    return 0;
}


int8_t login_server::login_success(message* _msg)
{

    uint32_t usrid = _msg->m_head.m_usrID;

    TRACER("usrid %d login success fd is %d\n", usrid, m_usrfd[usrid]);

    if (_msg->m_head.m_errID) {
        TRACER_ERROR("usrid %d login failed fd is %d\n", usrid, m_usrfd[usrid]);
        return -1;
    }
    
    // 先转发给gate server, 然后 gateserver 转发给 gameserver
    int32_t ret =  tcp_socket::tcp_send(g_connet_server[GATE_SERVER], _msg->m_data, _msg->m_head.m_len + MSG_HEAD_SIZE);
    if (ret < 0) {
        // 
    }


    TRACER_DEBUG("send allinfo to usrid %d, fd is %d\n", usrid, m_usrfd[usrid]);
    ret = tcp_socket::tcp_send(m_usrfd[usrid], _msg->m_data, _msg->m_head.m_len + MSG_HEAD_SIZE);

    // 发送gate server 端口给用户

    message msg;
    msg.m_head.m_type = SERVER_INFO;
    msg.m_head.m_usrID = usrid;
    msg.m_head.m_errID = 0;
    
    msg.encode_pb(g_serverinfo[GATE_SERVER]);

    return tcp_socket::tcp_send(m_usrfd[usrid], msg.m_data, msg.m_head.m_len + MSG_HEAD_SIZE);
    
}