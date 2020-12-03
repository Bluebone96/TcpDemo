#ifndef _KBUFFER_H_
#define _KBUFFER_H_
// 参考 kfifo.h 环形缓冲区

#include <stdint.h>
#include <cstring>
#include <exception>
#include <iostream>

#define MIN(A, B) (((A) < (B)) ? (A) : (B))

static inline bool is_pow_of_two (uint32_t _n)
{
    return (_n != 0 && ((_n & (_n - 1)) == 0));
}


static inline uint32_t roundup_pow_of_two(uint32_t _s)
{
    if (!is_pow_of_two(_s)) {
        int p = 0;
        for (int i = _s; i != 0; i >>= 1) {
            ++p;
        }
        return (1u << p);
    }
    return _s;
}


struct kbuffer {
    volatile uint32_t m_in; // todo 不确定是否有效，或采用原子操作
    volatile uint32_t m_out;
    uint32_t m_size;
    uint8_t *m_buffer;

    kbuffer()
    {
    
    }

    ~kbuffer()
    {
        delete[] m_buffer;
    }

    
    int8_t initbuff(uint32_t _s)
    {
        m_in = 0;
        m_out = 0;
        m_size = roundup_pow_of_two(_s);
        try
        {
            m_buffer = new uint8_t[m_size];
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return -1;
        }

        return 0;
    }

    uint32_t put(const uint8_t *_data, uint32_t _length)
    {
        _length = MIN(_length, m_size - m_in + m_out);

        uint32_t len = MIN(_length, m_size - (m_in & (m_size - 1)));
        memcpy(m_buffer + (m_in & (m_size - 1)), _data, len);

        memcpy(m_buffer, _data + len, _length - len);

        m_in += _length;

        return _length; 
    }

    uint32_t get(uint8_t *_data, uint32_t _length)
    {
        _length = MIN(_length, m_in - m_out);

        uint32_t len = MIN(_length, m_size - (m_out & (m_size - 1)));
        memcpy(_data, m_buffer + (m_out & (m_size - 1)), len);

        memcpy(_data + len, m_buffer, _length - len);

        m_out += _length;

        return _length;
    }

};



#endif

