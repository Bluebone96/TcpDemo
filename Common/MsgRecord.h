#ifndef _MSGRECORD_H_
#define _MSGRECORD_H_

#include <stdint.h>
#include <cstdlib>
#include <cstring>

#include "../build/PlayerInfo.pb.h"

#define  MAXDATALEN 4096


struct Record {
    uint32_t m_tag;   // 第一个字节为 EventType m_tag & 0xFF; 第二个字节为 id m_tag & 0xFF00
    uint32_t m_len; // 仅仅为 protobuf 的大小
    char m_data[0];
};

class MsgRecord {
public:
    
    explicit MsgRecord(uint32_t sz = MAXDATALEN) 
        : m_RecordSize(sizeof(Record))
        {
            sz = sz < 0 ? 0 : sz;
            sz = sz < MAXDATALEN ? sz : MAXDATALEN;
            
            m_size = sz;
            m_pRecord = (Record*)malloc(sz + m_RecordSize);
            bzero(m_pRecord, sz + m_RecordSize);
        }

    virtual ~MsgRecord() { free(m_pRecord); }

    // 初始化 数据的tag, 和 数据的长度
    int32_t MsgRecordInit(int32_t tag, int32_t len);

    void SetLen(int32_t len) { m_pRecord->m_len = len; };

    uint32_t GetLen() { return m_pRecord->m_len; }

    uint32_t GetTag() { return m_pRecord->m_tag; }
    void  SetTag(int32_t _tag) { m_pRecord->m_tag = _tag; }

    uint32_t GetEventType() { return m_pRecord->m_tag & 0xFF; }
    uint32_t GetID() { return m_pRecord->m_tag & 0xFF00; }

    void SetEventType(uint32_t _t) { m_pRecord->m_tag |= _t; }
    void SetID(uint32_t _t) { m_pRecord->m_tag |= (_t << 8); }

    int32_t Encode(void* src, uint32_t sz);
    int32_t Decode(void* src, uint32_t sz);
    
    int32_t Decode();
    int32_t Encode();

    int32_t Encode(const ::google::protobuf::Message& _protobuf);

    int32_t Decode(::google::protobuf::Message& _protobuf);


    char*     GetDataAddress() { return m_pRecord->m_data; }
    Record*   GetRecord() { return m_pRecord; }

public:
    uint32_t m_size;    // 申请的buff大小，在构造时记录
    uint32_t m_RecordSize;
    Record* m_pRecord;
};

class Coder {
public:
    static int32_t encode(void* dest, uint32_t destsz, void* src, uint32_t count);
    static int32_t decode(void* dest, uint32_t destsz, void* src, uint32_t count);
};






















































#endif


