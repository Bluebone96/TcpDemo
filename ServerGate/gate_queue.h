#include <unistd.h>
#include <stdint.h>

#include "../Proto/PlayerInfo.pb.h"

// #include "../Net/message.h"


// class gate_queue : public msg_queue {
// public:
//     gate_queue();
//     ~gate_queue();

// };
#define MSG_HEAD_SIZE 24

struct message {
// ===================== message head ====================
    uint32_t m_len; // 仅仅表示消息体长度，消息头长度固定的
    uint32_t m_type;
    uint32_t m_errID;
    uint32_t m_usrID;
    uint64_t m_tick;

// ===================== message body ====================
    uint32_t m_from;
    uint32_t m_to;
    uint8_t* m_data;

// ===================== func ============================
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
    virtual ~msg_queue();

typedef bool (*Foo)(message*, message*);

    int32_t init_queue(uint32_t, Foo = _func);
private:
    static inline bool _func(message*, message*);

    uint32_t m_in;
    uint32_t m_out;
    uint32_t m_size;

    message *m_pmsg;

private:
// 消息中数据都存放在这里
    uint8_t* m_pin;
    uint8_t* m_pout;
    uint8_t* m_big_memory;
};


