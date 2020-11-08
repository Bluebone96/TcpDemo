#include "MsgTransmission.h"

int32_t MsgTrans::Init(uint32_t tag, int32_t fd, uint32_t size) 
{
    TcpSocket::Init(fd);
    MsgRecord::Init(tag, size);
    return 0;
}

int32_t MsgTrans::sendmsg(Data& data)
{
    size_t sz = sizeof(Record);
    
    TRACER("sendmsg to fd %d\n", TcpSocket::GetSocketfd());

    if (!data.SerializePartialToArray(m_pRecord->m_data + sz, data.ByteSizeLong())) {
        TRACER("protobuf SerializePartialToArray failed.%s:%d", __FILE__, __LINE__);
    }
    // tag 值标记为 addr, len 为整个数据的长度
    if (Init(data.ByteSizeLong() + sz) < 0) {
        TRACER("MsgTrans::sendmsg MsgRecord::Init failed. %s:%d", __FILE__, __LINE__);
    } 
    
    TRACER("data size = %d\n", m_pRecord->m_len);

    MsgRecord::Encode();
    
    TRACER("data size = %d\n", m_pRecord->m_len);
    
    int32_t nleft = TcpSocket::SendData(m_pRecord->m_data, m_pRecord->m_len);
    if (nleft > 0) {
        TRACER("MsgTrans::sendmsg failed. %d bytes left.", nleft);
        return -1;
    }

    return 0;
}

int32_t MsgTrans::recvmsg(Data& data) 
{

    TRACER("recvmsg from fd %d", TcpSocket::GetSocketfd());

    TcpSocket::RecvData(m_pRecord->m_data, sizeof(Record));
    Decode(m_pRecord->m_data, sizeof(Record));
    TcpSocket::RecvData(m_pRecord->m_data + sizeof(Record), m_pRecord->m_len);
    data.ParseFromArray(m_pRecord->m_data + sizeof(Record), m_pRecord->m_len);
    return 0;
}