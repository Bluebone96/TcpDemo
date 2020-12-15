#include <unistd.h>
#include <vector>


#include "login_server.h"
#include "../Proto/PlayerInfo.pb.h"
#include "../Common/basetype.h"
#include "../SQL/tomysql.h"
#include "../Config/loadconfig.h"

login_server::login_server()
{

}

login_server::~login_server()
{

}



int8_t login_server::run()
{
    message *msg = nullptr;
    for (;;) {
        while ((msg = g_recv_queue.dequeue()) == nullptr) {
            usleep(100 * 1000);
        }

        switch (msg->m_head.m_type)
        {   
            case SERVER_INFO:
                {
                    Proto::Unity::ServerInfo server;
                    msg->decode_pb(server);
                    g_connet_server[server.m_type()] = msg->m_from;
                }
                break;
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

        msg->m_flag = msg_flags::INACTIVE;
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
    
    TRACER_DEBUG("usrname is %s, pass is %s, fd is %d\n", m_au.name().c_str(), m_au.password().c_str(), _msg->m_from);
    unsigned int id = BKDRHash(m_au.name());
    unsigned int pass = BKDRHash(m_au.password());

    // 记录
    TRACER_DEBUG("usrid:%d fd is : %d\n", _msg->m_head.m_usrID, _msg->m_from);
    m_usrfd[id] = _msg->m_from;
    m_waitverify.insert(std::make_pair(id, pass));


    message *msg = nullptr;
    while ((msg = g_send_queue.enqueue()) == nullptr)
    {
        // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
        TRACER_ERROR("sleep 50ms, g_recv_queue is full, usrid is %d\n", id);
        g_send_queue.debug_info();
        usleep(50 * 1000);
    }


    msg->m_head.m_type = GETPASS;
    // msg->m_from = LOGIN_SERVER;
    msg->m_to = g_connet_server[DB_SERVER];
    msg->m_head.m_usrID = id;
    msg->m_head.m_errID = 0;
    msg->m_head.m_len = 0;
    msg->encode();

    msg->m_flag = msg_flags::ACTIVE;

    return 0;
}


int8_t login_server::login_verify(message* _msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;
    if (_msg->m_head.m_errID) {
        // login_failed(_msg); 客户端还没加这个登录失败消息
        m_waitverify.erase(usrid);
        return -1;
    } 
    uint32_t key = ntoh_32(*(uint32_t*)(_msg->m_pdata));

    if (m_waitverify[usrid] != key) {
        // login_failed(_msg);
        m_waitverify.erase(usrid);
        return -1;
    }
    
    message *msg = nullptr;
    while ((msg = g_send_queue.enqueue()) == nullptr)
    {
        // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
        TRACER_ERROR("sleep 50ms, g_recv_queue is full, usrid is %d\n", usrid);
        g_send_queue.debug_info();
        usleep(50 * 1000);
    }

    msg->m_head.m_type = GET_ALLINFO;
    // msg->m_from = LOGIN_SERVER;
    msg->m_to = g_connet_server[DB_SERVER];
    msg->m_head.m_usrID = usrid;
    msg->m_head.m_errID = 0;
    msg->m_head.m_len = 0;
    msg->encode();
    msg->m_flag = msg_flags::ACTIVE;

    return 0;

}

int8_t login_server::login_failed(message* _msg)
{
    uint32_t usrid = _msg->m_head.m_usrID;

    message *msg = nullptr;
    while ((msg = g_send_queue.enqueue()) == nullptr)
    {
        // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
        TRACER_ERROR("sleep 50ms, g_recv_queue is full, usrid is %d\n", usrid);
        g_send_queue.debug_info();
        usleep(50 * 1000);
    }
    
    TRACER_ERROR("usrid %d login failed fd is %d\n", usrid, m_usrfd[usrid]);
    msg->m_head.m_type = LOGIN_FAILED;
    // msg->m_from = LOGIN_SERVER;
    msg->m_head.m_usrID = usrid;
    msg->m_to = m_usrfd[usrid];
    msg->m_head.m_errID = _msg->m_head.m_errID;
    msg->m_head.m_len = 0;
    msg->encode();
    msg->m_flag = msg_flags::ACTIVE;

    return 0;
}



int8_t login_server::login_success(message* _msg)
{

    uint32_t usrid = _msg->m_head.m_usrID;

    TRACER_DEBUG("usrid %d login success fd is %d\n", usrid, m_usrfd[usrid]);

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
    
    Proto::Unity::ServerInfo serverpb;

    server_config cfg;
    if (load_config("gate_server_for_client", cfg) < 0) {
        TRACER_ERROR("load gate_server_for_client config failed");
        return -1;
    }

    serverpb.set_m_type(GATE_SERVER);
    serverpb.set_m_ip(cfg.ip);
    serverpb.set_m_port(cfg.port);
    msg.encode_pb(serverpb);

    return tcp_socket::tcp_send(m_usrfd[usrid], msg.m_data, msg.m_head.m_len + MSG_HEAD_SIZE);
    
}