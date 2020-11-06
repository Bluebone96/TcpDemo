#ifndef _MSGRECORD_H_
#define _MSGRECORD_H_

#include <stdint.h>
#include <cstdlib>
#include <cstring>

#define  MAXDATALEN 1024


struct Record {
    int32_t m_tag;
    uint32_t m_len;
    char m_data[0];
};

class MsgRecord {
public:
    
    explicit MsgRecord(int32_t sz = 0, Record* mp = nullptr) 
        : m_size(sz), m_pRecord(mp)
        {
            if (nullptr == m_pRecord) {
                m_pRecord = (Record*)malloc(MAXDATALEN);
                bzero(m_pRecord, MAXDATALEN);
            }
        }

    ~MsgRecord() { free(m_pRecord); }

    int32_t Init(int32_t tag, uint32_t len = MAXDATALEN);

    uint32_t GetSize() { return m_pRecord->m_len; }
    uint32_t GetTag() { return m_pRecord->m_tag; }
    int32_t Encode(void* src, uint32_t sz);
    int32_t Decode(void* src, uint32_t sz);

    char*   GetDateAddress() { return m_pRecord->m_data; }

protected:
    uint32_t m_size;
    Record* m_pRecord;
};

class Coder {
public:
    static int32_t encode(void* dest, uint32_t destsz, void* src, uint32_t count);
    static int32_t decode(void* dest, uint32_t destsz, void* src, uint32_t count);
};

#endif
