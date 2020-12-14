#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdint.h>
#include "byte_swap.hpp"
#include "../Proto/PlayerInfo.pb.h"


struct msg_head {
#define MSG_HEAD_SIZE 24
    uint32_t m_len; // 仅仅表示消息体长度，消息头长度固定的
    uint32_t m_type;
    uint32_t m_errID;
    uint32_t m_usrID;
    uint64_t m_tick;

    msg_head();
    ~msg_head();

    uint8_t decode(uint8_t *_data, uint32_t _len);

    uint8_t encode(uint8_t *_data, uint32_t _len);

};

enum class msg_flags {
    INVALID = -1, // 消息队列中当入队和出队失败时设置， 写入时直接覆盖，读取时跳过
    INACTIVE = 0,
    ACTIVE = 1
};


struct message {
// #define MSG_ACTIVE  1
// #define MSG_INACTIVE  0
#define MSG_BUF_SIZE 256    // 现在一条消息很小
    msg_flags m_flag;  // 是否过期 0 过期， 1 有效
    uint32_t m_from;
    uint32_t m_to;
    msg_head m_head;
    uint8_t m_data[MSG_BUF_SIZE];
    uint8_t *m_pdata;

    message();
    ~message();

    uint8_t decode();

    uint8_t encode();

    uint8_t decode_pb(google::protobuf::Message& _pb);

    uint8_t encode_pb(const google::protobuf::Message& _pb);

};


class msg_queue {
public:
    msg_queue();
    ~msg_queue();

    int8_t init_queue(uint32_t _s);

    message* enqueue();
    message* dequeue();
    
    // for debug
    void debug_info();
    
private:
    // volatile uint32_t m_in;
    // volatile uint32_t m_out;
    uint32_t m_in;
    uint32_t m_out;
    uint32_t m_size;
    message *m_pmsg;
};



#endif