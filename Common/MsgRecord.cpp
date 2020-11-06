#include <netinet/in.h>
#include "MsgRecord.h"
#include "log.h"

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

int32_t MsgRecord::Init(int32_t tag, uint32_t len)
{
    m_size = len;
    m_pRecord->m_tag = tag;
    m_pRecord->m_len = len;
    return 0;
}

int32_t MsgRecord::Encode(void *src, uint32_t sz)
{
    return Coder::encode(m_pRecord, sizeof(Record), src, sz);
}

int32_t MsgRecord::Decode(void *src, uint32_t sz)
{
    return Coder::decode(src, sz, m_pRecord, sizeof(Record));
}