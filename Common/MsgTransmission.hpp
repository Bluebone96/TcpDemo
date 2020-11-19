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
    
    int32_t Init(int32_t fd, void * addr, int32_t len)
    {
        TcpSocketInit(fd, addr, len); // 考虑没必要，待修改

        // MsgRecordInit(0, 0);
        return 0;
    }

    int32_t RecvMsgHead(char* _usrbuf, int _len)
    {   
        if (_len < m_RecordSize) {
            TRACER("buflen is %d bytes, the message head is %d bytes", _len, m_RecordSize);
            return -1;
        }
        int errnocode = 0;
        if ( (errnocode = TcpSocket::RecvData(_usrbuf, m_RecordSize)) < 0) {
            TRACERERRNO("recvmsg error %s:%d\n", __POSITION__);
            return errnocode;
        }

        Decode(_usrbuf, m_RecordSize);

        return m_RecordSize;
    }

    int32_t RecvMsgHead() 
    {
        return RecvMsgHead(m_pRecord->m_data, m_RecordSize);
    }


    int32_t sendmsg(char* data, int32_t len)
    {
        int32_t nleft = TcpSocket::SendData(data, len);
        
        if (nleft > 0) {
            TRACER("MsgTrans::sendmsg failed. %d bytes left.", nleft);
            return -1;
        }

        return 0;
    }

    int32_t sendmsg() {
        return sendmsg(m_pRecord->m_data, m_pRecord->m_len + m_RecordSize);
    }

    template <typename T>
    int32_t sendmsg(T& data)
    {
        
        if (!data.SerializeToArray(m_pRecord->m_data + m_RecordSize, data.ByteSizeLong())) {
            TRACER("protobuf SerializePartialToArray failed.%s:%d", __FILE__, __LINE__);
        }

        
        MsgRecord::SetLen(data.ByteSizeLong());

        Encode();
        
        return sendmsg();
    }

    int32_t recvmsg(char* data, uint32_t len)
    {
        int32_t errnocode = 0;

        if ( (errnocode = TcpSocket::RecvData(data, m_RecordSize)) < 0) {
            TRACERERRNO("recvmsg error %s:%d\n", __POSITION__);
            return errnocode;
        }

        Decode(data, m_RecordSize);

        if (len < m_pRecord->m_len) {
            TRACERERRNO("Complete data is %d bytes, just recv %d bytes", m_pRecord->m_len, len);
            m_pRecord->m_len = len;
        }

        if ((errnocode = TcpSocket::RecvData(data + m_RecordSize, m_pRecord->m_len)) < 0) {
            TRACERERRNO("recvmsg error %s:%d\n", __POSITION__);
            return errnocode;
        }

        return m_pRecord->m_len;
    }

    int32_t recvmsg()
    {
        return recvmsg(m_pRecord->m_data, m_size);
    }

    template<typename T>
    int32_t recvmsg(T& data)
    {

        int32_t error;
        if ((error = recvmsg()) < 0) {
            return error;
        }
        
        data.ParseFromArray(m_pRecord->m_data + m_RecordSize, m_pRecord->m_len);

        return 0;
    }

    
   


    void  printbin(char* bin, int len)
    {
        printf("The data bin is: ");
        for (int i = 0; i < len; ++i) { 
            printf("%d ", (int)(bin[i])); 
        } 
        printf("\n");
    }
};


#endif
