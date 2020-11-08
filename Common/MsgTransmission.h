#ifndef __MSGTRANS_H_
#define __MSGTRANS_H_

#include "TcpSocket.h"
#include "MsgRecord.h"
#include "Epoll.h"
#include "base.h"

class MsgTrans : public TcpSocket, public MsgRecord {
public:
    int32_t Init(uint32_t tag, int32_t fd, uint32_t size = 0);
    // 提供一个只初始化数据大小的 Init;

    int32_t Init(uint32_t size = 0)
    {
        int32_t tmp = m_pRecord->m_tag;
        return MsgRecord::Init(tmp, size);
    }
    int32_t sendmsg(Data& data);
    int32_t recvmsg(Data& data);
};



#endif
