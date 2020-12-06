#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdint.h>
#include "byte_swap.hpp"
#include "../Proto/PlayerInfo.pb.h"


struct msg_head {
#define MSG_HEAD_SIZE 16
    uint32_t m_len; // 仅仅表示消息体长度，消息头长度固定的
    uint32_t m_type;
    uint32_t m_usrID;
    uint32_t m_errID;

    msg_head();
    ~msg_head();

    uint8_t decode(uint8_t *_data, uint32_t _len);

    uint8_t encode(uint8_t *_data, uint32_t _len);

};

struct message {
    uint8_t m_flag;  // 是否过期 0 过期， 1 有效
    uint32_t m_from;
    uint32_t m_to;
    msg_head m_head;
    uint8_t m_data[1024];
    uint8_t *m_pdata;

    message();
    ~message();

    uint8_t decode();

    uint8_t encode();

    uint8_t decode_pb(google::protobuf::Message& _pb);

    uint8_t encode_pb(const google::protobuf::Message& _pb);

    void setvalid() { m_flag = 1; }
    void setinvalid() { m_flag = 0; }
};


class msg_queue {
public:
    msg_queue();
    ~msg_queue();

    int8_t init_queue(uint32_t _s);

    message* enqueue();
    message* dequeue();

private:
    volatile uint32_t m_in;
    volatile uint32_t m_out;
    uint32_t m_size;
    message *m_pmsg;
};



#endif