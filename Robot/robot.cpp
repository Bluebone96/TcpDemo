#include "robot.h"
#include "../Common/log.h"
#include "../Common/basetype.h"

static unsigned int BKDRHash(const std::string& _str)
{
    unsigned int seed = 1313;
    unsigned int hash = 0;
    for (auto& c : _str) {
        hash = hash * seed + c;
    }
    return (hash & 0x7FFFFFFF);
}


inline const char* get_name(int x)
{
    static char name[20];
    snprintf(name, 30, "robot_%d", x);
    return name;
}


Robot::Robot(int i) : m_name(get_name(i)), m_pass(m_name), m_id(BKDRHash(m_name)) {}

Robot::~Robot() 
{
    exit();
    delete m_socket;
}


bool Robot::login(std::string& _ip, uint32_t _port)
{
    m_socket = new tcp_socket();
    m_socket->tcp_init(-1, 4096);

    if (m_socket->tcp_connect(_ip.c_str(), _port) < 0) {
        TRACER_ERROR("connect filed! name: %s\n", m_name.c_str());
    }
    Proto::Unity::Authentication au;
    au.set_name(m_name.c_str());
    au.set_password(m_name.c_str());

    m_msg.m_head.m_type = LOGIN_REQUEST;
    m_msg.m_head.m_usrID = m_id;
    m_msg.m_head.m_errID = 0;
    m_msg.encode_pb(au);
    m_socket->tcp_send(m_msg.m_data, m_msg.m_head.m_len + MSG_HEAD_SIZE);

    if (m_socket->tcp_recv(m_msg.m_data, MSG_HEAD_SIZE) > 0) {
        m_msg.decode();
        if (m_msg.m_head.m_type == GET_ALLINFO) {
            TRACER_DEBUG("login success! %s\n", m_name);
            close(m_socket->getfd());
    
            if ((m_socket->tcp_connect("192.168.80.3", 3333)) > 0) {
                TRACER_DEBUG("connect gate server success %s", m_name.c_str());

                m_msg.m_head.m_type = 0;
                m_msg.m_head.m_len = 0;
                m_msg.encode();
                m_socket->tcp_send(m_msg.m_data, MSG_HEAD_SIZE);
                return true;

            }
        }
    }

    TRACER_ERROR("login failed! %s\n", m_name.c_str());
    return false;
}



bool Robot::exit()
{
    m_msg.m_head.m_type = USEREXIT;
    m_msg.m_head.m_len = 0;
    m_msg.m_head.m_usrID = m_id;
    m_msg.encode();
    
    return m_socket->tcp_send(m_msg.m_data, MSG_HEAD_SIZE);
}