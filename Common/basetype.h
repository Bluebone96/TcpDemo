#ifndef _BASETYPE_H_
#define _BASETYPE_H_

enum _OK_ERR_ {
    ERR = -1,
    EOK = 0
};

typedef unsigned char BYTE;
typedef signed char SIG_BYTE;

typedef unsigned int UINT32;

typedef int SINT32;


// // TODO 大小端转换有bug，等待重构 
// typedef union _OPERATION_ {
//     struct {
//         unsigned char _w : 1;
//         unsigned char _s : 1;
//         unsigned char _a : 1;
//         unsigned char _d : 1;

//         unsigned char _jp : 1;  // 跳 不实现
//         unsigned char _m1 : 1;
//         unsigned char _m2 : 1;
//         unsigned char _s1 : 1;
        
//         unsigned char _s2 : 1;
//         unsigned char _s3 : 1;
//         unsigned char _s4 : 1;
//         unsigned char _s5 : 1;
        
//         unsigned char _s6 : 1;
//         unsigned char _s7 : 1;
//         unsigned char _s8 : 1;
//         unsigned char _s9 : 1;
        
//         unsigned char _s2_add : 8;
//         unsigned char _s3_add : 8;
//     } op;
//     UINT32 val;
// } OPERATION;


// typedef union _TAG_ {
//     struct {
//         unsigned char _type : 8;
//         unsigned char _id : 8;
//         unsigned char _add1 : 8;
//         unsigned char _add2 : 8;
//     } tag;

//     UINT32 val;
// } TAG;


// enum class EventType {
//     USERLOGIN = 0,
//     USEREXIT = 1,
//     USERALIVE = 2,
//     USERUP = 3,
//     USERSYNC = 4,
//     USERCHAT = 5,
// };


enum MsgType {
    USERLOGIN = 0,
    USEREXIT = 1,
    USERALIVE = 2,
    USERUP = 3,
    USERSYNC = 4,
    USERCHAT = 5,

    ITEMEVENT = 6,

    LOGIN_REQUEST = 7,
    LOGIN_SUCCESS = 8,
    LOGIN_FAILED = 9,

    GETPASS = 10,
    SETPASS = 11,

    GETITEM = 12,
    SETITEM = 13,

    GETPLAYER = 14,
    SETPLAYER = 15,

    GET_ALLINFO = 16,
    SET_ALLINFO = 17,
    
    DB_SUCCESS = 18,  // 不需要返回内容
    DB_FAILED = 19,  // db 中失败

    SERVER_INFO = 20 // 服务器信息
};


// struct Auth {
//     uint32_t m_id;
//     uint32_t m_key;
//     Auth(uint32_t _i = 0, uint32_t _k = 0) : m_id(_i), m_key(_k) {}
//     void decode(uint8_t *_buf)
//     {
//         m_id = ntoh_32(*(uint32_t*)_buf);
//         m_key = ntoh_32(*(uint32_t*)(_buf + 4));
//     }

//     void encode(uint8_t *_buf)
//     {
//         *(uint32_t*)_buf = hton_32(m_id);
//         *(uint32_t*)(_buf + 4) = hton_32(m_key);
//     }
// };



enum ServerID {
    LOGIN_SERVER = 100,
    GATE_SERVER = 102,
    DB_SERVER = 103,
    GAME_SERVER = 104
};




#endif

