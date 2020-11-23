#include <netinet/in.h>
#include "MsgRecord.h"
#include "log.h"


// src to dest
int32_t 
Coder::encode(void* dest, uint32_t destsz, void* src, uint32_t count) 
{
    uint32_t maxcoded = destsz > count ? destsz : count;

    uint32_t *pd = (uint32_t*)dest, *ps = (uint32_t*)src;
    for (; destsz >= 4 && count >= 4; ++pd, ++ps, destsz -= 4, count -= 4) {
        *pd = htonl(*ps);
    }

    uint16_t *d2 = (uint16_t*)pd, *s2 = (uint16_t*)ps;
    for (; destsz >= 2 && count >= 2; ++d2, ++s2, destsz -= 2, count -= 2) {
        *d2 = htons(*s2);
    }

    if (destsz && count) {
        *d2 = *s2;
    }

    return maxcoded;
}


int32_t
Coder::decode(void* dest, uint32_t destsz, void* src, uint32_t count) 
{
    uint32_t maxcoded = destsz > count ? destsz : count;

    uint32_t *pd = (uint32_t*)dest, *ps = (uint32_t*)src;
    for (; destsz >= 4 && count >= 4; ++pd, ++ps, destsz -= 4, count -= 4) {
        *pd = ntohl(*ps);
    }

    uint16_t *d2 = (uint16_t*)pd, *s2 = (uint16_t*)ps;
    for (; destsz >= 2 && count >= 2; ++d2, ++s2, destsz -= 2, count -= 2) {
        *d2 = ntohs(*s2);
    }

    if (destsz && count) {
        *d2 = *s2;
    }

    return maxcoded;
}

int32_t MsgRecord::MsgRecordInit(int32_t tag, int32_t len)
{
   // memset(m_pRecord, 0, m_size + m_RecordSize);
    m_pRecord->m_id = tag;
    m_pRecord->m_len = len;
    return 0;
}

int32_t MsgRecord::Encode(void *dest, uint32_t sz)
{
    m_pRecord->m_data[0] = m_pRecord->m_type;
    m_pRecord->m_data[1] = m_pRecord->m_id;
    m_pRecord->m_data[2] = m_pRecord->m_add1;
    m_pRecord->m_data[3] = m_pRecord->m_add2;
    *((uint32_t *)(m_pRecord->m_data + 4)) = htonl(m_pRecord->m_len);
    return m_RecordSize;
    // return Coder::encode(dest, sz, m_pRecord, m_RecordSize);
}

int32_t MsgRecord::Decode(void *src, uint32_t sz)
{
    m_pRecord->m_type = m_pRecord->m_data[0];
    m_pRecord->m_id = m_pRecord->m_data[1];
    m_pRecord->m_add1 = m_pRecord->m_data[2];
    m_pRecord->m_add2 = m_pRecord->m_data[3];

    m_pRecord->m_len = ntohl(*((uint32_t *)(m_pRecord->m_data + 4)));
    
    return m_RecordSize;
//    return Coder::decode(m_pRecord, m_RecordSize, src, sz);
}

int32_t MsgRecord::Encode()
{   
    m_pRecord->m_data[0] = m_pRecord->m_type;
    m_pRecord->m_data[1] = m_pRecord->m_id;
    m_pRecord->m_data[2] = m_pRecord->m_add1;
    m_pRecord->m_data[3] = m_pRecord->m_add2;
    *((uint32_t *)(m_pRecord->m_data + 4)) = htonl(m_pRecord->m_len);
    return m_RecordSize;
    //return Coder::encode(m_pRecord->m_data, m_RecordSize, m_pRecord, m_RecordSize);
}

int32_t MsgRecord::Decode()
{
    m_pRecord->m_type = m_pRecord->m_data[0];
    m_pRecord->m_id = m_pRecord->m_data[1];
    m_pRecord->m_add1 = m_pRecord->m_data[2];
    m_pRecord->m_add2 = m_pRecord->m_data[3];

    m_pRecord->m_len = ntohl(*((uint32_t *)(m_pRecord->m_data + 4)));
    
    return m_RecordSize;
    //return this->Decode(m_pRecord->m_data, m_RecordSize);
}


int32_t MsgRecord::Encode(const ::google::protobuf::Message& _protobuf)
{
    m_pRecord->m_len = _protobuf.ByteSizeLong();

    TRACER("Prorobuf Encode  m_pRecord len is %d\n", m_pRecord->m_len);

    if (!_protobuf.SerializeToArray(m_pRecord->m_data + m_RecordSize, m_pRecord->m_len)) {
        TRACER("protobuf SerializePartialToArray failed.%s:%d", __FILE__, __LINE__);
    }
    
    Encode();
    
    return m_pRecord->m_len + m_RecordSize;
}

int32_t MsgRecord::Decode(::google::protobuf::Message& _protobuf)
{
    _protobuf.ParseFromArray(m_pRecord->m_data + m_RecordSize, m_pRecord->m_len);
    return 0;
}