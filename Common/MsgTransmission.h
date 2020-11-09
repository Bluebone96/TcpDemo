#ifndef __MSGTRANS_H_
#define __MSGTRANS_H_

#include "TcpSocket.h"
#include "MsgRecord.h"
#include "Epoll.h"
#include "base.h"

class MsgTrans : public TcpSocket, public MsgRecord {
public:
    MsgTrans(int32_t fd = -1, uint32_t sockbufsize = SOCKETBUFLEN, uint32_t recordbufsize = MAXDATALEN)
         : TcpSocket(fd), MsgRecord(){}
    ~MsgTrans() {}
    int32_t Init(uint32_t tag, int32_t fd, uint32_t size = 0);
    
    // int32_t Init()

    int32_t sendmsg(Data& data);
    int32_t recvmsg(Data& data);
    void    printbin(char*, int len);
};



#endif
