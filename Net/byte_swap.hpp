#ifndef _BYTE_SWAP_HPP_
#define _BYTE_SWAP_HPP_
// 参考 byteswap.h

#include <stdint.h>


inline bool is_little()
{
    static const union {
        int big;
        char little;
    } endian {1};
    return endian.little;
}

inline uint16_t hton_16 (uint16_t _x)
{
    if (is_little()) {
        return ((_x >> 8) & 0xff) | ((_x & 0xff) << 8);
    }
    return _x;
}

inline uint16_t ntoh_16(uint16_t _x)
{
    return hton_16(_x);
}



inline uint32_t hton_32 (uint32_t _x)
{
    if (is_little()) {
        return ((_x & 0xff000000u) >> 24) | ((_x & 0x00ff0000u) >> 8) 
              | ((_x & 0x0000ff00u) << 8) | ((_x & 0x000000ffu) << 24);
    }
    return _x;
}

inline uint32_t ntoh_32(uint32_t _x)
{
    return hton_32(_x);
}


inline uint64_t hton_64 (uint64_t _x)
{
    if (is_little()) {
        return ((_x & 0xff00000000000000ull) >> 56)
               | ((_x & 0x00ff000000000000ull) >> 40)
               | ((_x & 0x0000ff0000000000ull) >> 24)
               | ((_x & 0x000000ff00000000ull) >> 8)
               | ((_x & 0x00000000ff000000ull) << 8)
               | ((_x & 0x0000000000ff0000ull) << 24)
               | ((_x & 0x000000000000ff00ull) << 40)
               | ((_x & 0x00000000000000ffull) << 56);
    }
    return _x;
}

inline uint64_t ntoh_64(uint64_t _x)
{
    return hton_64(_x);
}

#endif
