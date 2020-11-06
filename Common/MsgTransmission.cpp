#include "MsgTransmission.h"

int32_t MsgTrans::Init(uint32_t tag, int32_t fd, uint32_t size) 
{
    TcpSocket::Init(fd, size);
    MsgRecord::Init(tag);
    return 0;
}

int32_t MsgTrans::sendmsg(Data& data)
{
    size_t sz = sizeof(Record);
    std::string tmp;
    
    TRACER("sendmsg to fd %d\n", TcpSocket::GetFd());

    if (!data.SerializePartialToArray(m_pRecord->m_data + sz, data.ByteSizeLong())) {
        TRACER("protobuf SerializePartialToArray failed.%s:%d", __FILE__, __LINE__);
    }
    
    if (MsgRecord::Init(data.ByteSizeLong() + sz) < 0) {
        TRACER("MsgTrans::sendmsg MsgRecord::Init failed. %s:%d", __FILE__, __LINE__);
    } 
    
    MsgRecord::Encode();
    
    int32_t nleft = TcpSocket::SendData(m_pRecord->m_data, m_pRecord->m_len);
    if (nleft > 0) {
        TRACER("MsgTrans::sendmsg failed. %d bytes left.", nleft);
        return -1;
    }

    return 0;
}

int32_t MsgTrans::recvmsg(Data& data) 
{

    TRACER("recvmsg from fd %d", TcpSocket::GetFd());

    TcpSocket::RecvData(m_pRecord->m_data, sizeof(Record));
    Decode(m_pRecord->m_data, sizeof(Record));
    TcpSocket::RecvData(m_pRecord->m_data + sizeof(Record), m_pRecord->m_len);
    data.ParseFromArray(m_pRecord->m_data + sizeof(Record), m_pRecord->m_len);
    return 0;
}