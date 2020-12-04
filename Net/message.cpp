#include "message.h"
#include <exception>
#include <iostream>

msg_head::msg_head() : m_len(0), m_type(0), m_from(0), m_to(0) 
{

}

msg_head::~msg_head()
{

}

uint8_t msg_head::decode(uint8_t *_data, uint32_t _len) 
{
    if (_len < MSG_HEAD_SIZE) { return -1; }

    m_len = ntoh_32(*(uint32_t*)_data);
    m_type = ntoh_32(*(uint32_t*)(_data + 4));
    m_from = ntoh_32(*(uint32_t*)(_data + 8));
    m_to = ntoh_32(*(uint32_t*)(_data + 12));
    m_usrID = ntoh_32(*(uint32_t*)(_data + 16));
    m_errID = ntoh_32(*(uint32_t*)(_data + 20));
    
    return 0;
}

uint8_t msg_head::encode(uint8_t *_data, uint32_t _len)
{
    if (_len < MSG_HEAD_SIZE) { return -1; }
    
    *(uint32_t*)_data = hton_32(m_len);
    *(uint32_t*)(_data + 4) = hton_32(m_type);
    *(uint32_t*)(_data + 8) = hton_32(m_from);
    *(uint32_t*)(_data + 12) = hton_32(m_to);
    *(uint32_t*)(_data + 16) = hton_32(m_usrID);
    *(uint32_t*)(_data + 20) = hton_32(m_errID);

    return 0;
}

message::message() : m_head(), m_data{0}, m_pdata(m_data + MSG_HEAD_SIZE), m_fd(-1), m_flag(0)
{

}

message::~message()
{

}

uint8_t message::decode()
{
    return m_head.decode(m_data, 1024);
}

uint8_t message::encode()
{
    return m_head.encode(m_data, 1024);
}



static inline bool is_pow_of_two (uint32_t _n)
{
    return (_n != 0 && ((_n & (_n - 1)) == 0));
}


static inline uint32_t roundup_pow_of_two(uint32_t _s)
{
    if (!is_pow_of_two(_s)) {
        int p = 0;
        for (int i = _s; i != 0; i >>= 1) {
            ++p;
        }
        return (1u << p);
    }
    return _s;
}





msg_queue::msg_queue() : m_in(0), m_out(0), m_size(0), m_pmsg(0)
{

}

msg_queue::~msg_queue()
{
    delete[] m_pmsg;
}

int8_t msg_queue::init_queue(uint32_t _s)
{
    m_size = roundup_pow_of_two(_s);
    try
    {
        m_pmsg = new message[m_size];
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}


message* msg_queue::enqueue()
{
    uint32_t pos = (m_in & (m_size - 1));
    
    if ((m_pmsg + pos)->m_flag) { // 无效返回
        return nullptr;
    }
    ++m_in;
    return (m_pmsg + pos);
}

message* msg_queue::dequeue()
{
    uint32_t pos = (m_out & (m_size - 1));

    if ((m_pmsg + pos)->m_flag) { // 有效返回
        ++m_out;
        return m_pmsg + pos;
    }

    return nullptr;
}
