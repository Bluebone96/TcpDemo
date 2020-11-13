#ifndef _MSGRECORD_H_
#define _MSGRECORD_H_

#include <stdint.h>
#include <cstdlib>
#include <cstring>

#define  MAXDATALEN 1024


struct Record {
    int32_t m_tag;
    uint32_t m_len; // m_data的大小
    char m_data[0];
};

class MsgRecord {
public:
    
    explicit MsgRecord(uint32_t sz = MAXDATALEN) 
        : m_RecordSize(sizeof(Record))
        {
            sz = sz < MAXDATALEN ? sz : MAXDATALEN;
            m_size = sz;
            m_pRecord = (Record*)malloc(sz + m_RecordSize);
            bzero(m_pRecord, sz + m_RecordSize);
        }

    ~MsgRecord() { free(m_pRecord); }

    // 初始化 数据的tag, 和 数据的长度
    int32_t MsgRecordInit(int32_t tag, int32_t len);

    void SetLen(int32_t len) { m_pRecord->m_len = len; };

    uint32_t GetDataLen() { return m_pRecord->m_len; }
    uint32_t GetTag() { return m_pRecord->m_tag; }
    int32_t Encode(void* src, uint32_t sz);
    int32_t Decode(void* src, uint32_t sz);
    
    int32_t Decode();
    int32_t Encode();

    char*     GetDataAddress() { return m_pRecord->m_data; }
    Record*   GetRecord() { return m_pRecord; }
protected:
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
