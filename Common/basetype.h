#ifndef _BASETYPE_H_
#define _BASETYPE_H_

// #define  EOK 0
// #define  ERR -1
#include "../Proto/PlayerInfo.pb.h"

typedef  ::google::protobuf::Message PROTOBUF;

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


enum class EventType {
    USERLOGIN = 0,
    USEREXIT = 1,
    USERALIVE = 2,
    USERUP = 3,
    USERSYNC = 4,
    USERCHAT = 5,
};

#endif

