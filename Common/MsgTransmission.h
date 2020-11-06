#ifndef __MSGTRANS_H_
#define __MSGTRANS_H_

#include "TcpSocket.h"
#include "MsgRecord.h"
#include "Epoll.h"
#include "base.h"

class MsgTrans : public TcpSocket, public MsgRecord {
public:
    int32_t Init(uint32_t tag, int32_t fd, uint32_t size = SOCKETBUFLEN);
    int32_t sendmsg(Data& data);
    int32_t recvmsg(Data& data);
    int32_t OpenAsClient(const char* hostname, int16_t port);
    int32_t OpenAsServer(const int16_t port, const char* hostname= nullptr);

    int32_t Accept(int32_t fd, sockaddr_in* ps, socklen_t *len) { return TcpSocket::Accept(fd, ps, len); }

};



#endif
