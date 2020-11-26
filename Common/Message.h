#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <queue>


struct Message
{
    int m_type;
    void* m_owner;
    int m_len;
    char m_data[0];
};


static std::queue<Message*> g_messageQue;




#endif
