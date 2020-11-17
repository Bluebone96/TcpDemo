#ifndef _BASETYPE_H_
#define _BASETYPE_H_

// #define  EOK 0
// #define  ERR -1

enum _OK_ERR_ {
    ERR = -1,
    EOK = 0
};

typedef unsigned char BYTE;
typedef signed char SIG_BYTE;

typedef unsigned int UINT32;

typedef int SINT32;

typedef union _OPERATION {
    struct {
        unsigned char _w : 1;
        unsigned char _s : 1;
        unsigned char _a : 1;
        unsigned char _d : 1;

        unsigned char _jp : 1;
        unsigned char _m1 : 1;
        unsigned char _m2 : 1;
        unsigned char _s1 : 1;
        
        unsigned char _s2 : 1;
        unsigned char _s3 : 1;
        unsigned char _s4 : 1;
        unsigned char _s5 : 1;
        
        unsigned char _s6 : 1;
        unsigned char _s7 : 1;
        unsigned char _s8 : 1;
        unsigned char _s9 : 1;
        
        unsigned char _s2_add : 8;
        unsigned char _s3_add : 8;
    } op;
    UINT32 val;
} OPERATION;

#endif

