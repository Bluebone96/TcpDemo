#include "message.h"
#include "../Common/log.h"

#include <exception>
#include <iostream>

msg_head::msg_head() : m_len(0), m_type(0), m_errID(0), m_usrID(0)
{

}

msg_head::~msg_head()
{

}

uint8_t msg_head::decode(uint8_t *_data, uint32_t _len) 
{
    if (_len < MSG_HEAD_SIZE) { return -1; }

    m_type = ntoh_32(*(uint32_t*)_data);
    m_len = ntoh_32(*(uint32_t*)(_data + 4));
    m_usrID = ntoh_32(*(uint32_t*)(_data + 8));
    m_errID = ntoh_32(*(uint32_t*)(_data + 12));
    
    return 0;
}

uint8_t msg_head::encode(uint8_t *_data, uint32_t _len)
{
    if (_len < MSG_HEAD_SIZE) { return -1; }
    
    *(uint32_t*)_data = hton_32(m_type);
    *(uint32_t*)(_data + 4) = hton_32(m_len);
    *(uint32_t*)(_data + 8) = hton_32(m_usrID);
    *(uint32_t*)(_data + 12) = hton_32(m_errID);

    return 0;
}

message::message() : m_flag(msg_flags::INACTIVE), m_from(0), m_to(0), m_head(), m_data{0}, m_pdata(m_data + MSG_HEAD_SIZE)
{

}

message::~message()
{

}

uint8_t message::decode()
{   
    TRACER_DEBUG("msg decode head\n");
    m_head.decode(m_data, 1024);
    TRACER_DEBUG("msg decode head end\n");
    return 0;
}

uint8_t message::encode()
{
    TRACER_DEBUG("msg encode head\n");
    return m_head.encode(m_data, 1024);
}

uint8_t message::decode_pb(google::protobuf::Message& _pb)
{
    m_head.decode(m_data, 1024);

    TRACER_DEBUG("msg decode pb\n");
    _pb.ParseFromArray(m_pdata, m_head.m_len);
    
    return 0;
}

uint8_t message::encode_pb(const google::protobuf::Message& _pb) {
    TRACER_DEBUG("msg encode head\n");
    m_head.m_len = _pb.ByteSizeLong();
    _pb.SerializeToArray(m_pdata, 1024);
    m_head.encode(m_data, 1024);
    return 0;
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





msg_queue::msg_queue() : m_in(0), m_out(0), m_size(0), m_pmsg(nullptr)
{
    TRACER_DEBUG("msg_queue ctor\n");
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
    if ((m_pmsg + pos)->m_flag != msg_flags::ACTIVE) { // 无效返回
        TRACER_DEBUG("message enqueue, pos = %d, flag is %d\n", pos, static_cast<int>((m_pmsg + pos)->m_flag));
        ++m_in;
        return (m_pmsg + pos);
    }
    return nullptr;
}

message* msg_queue::dequeue()
{
    uint32_t pos = (m_out & (m_size - 1));
    while ((m_pmsg + pos)->m_flag == msg_flags::INVALID)
    {
        ++m_out;
        ++pos;
    }
    
    if ((m_pmsg + pos)->m_flag == msg_flags::ACTIVE) { // 有效返回
        TRACER_DEBUG("message dequeue, pos = %d, flag is %d\n", pos,  static_cast<int>((m_pmsg + pos)->m_flag));
        ++m_out;
        return m_pmsg + pos;
    }

    return nullptr;
}
