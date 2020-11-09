#include "MsgTransmission.h"

int32_t MsgTrans::Init(uint32_t tag, int32_t fd, uint32_t size) 
{
    TcpSocket::Init(fd); // 考虑没必要，待修改
    MsgRecord::Init(tag, size);
    return 0;
}

int32_t MsgTrans::sendmsg(Data& data)
{

    
    TRACER("sendmsg to fd %d\n", TcpSocket::GetSocketfd());

    if (!data.SerializePartialToArray(m_pRecord->m_data + m_RecordSize, data.ByteSizeLong())) {
        TRACER("protobuf SerializePartialToArray failed.%s:%d", __FILE__, __LINE__);
    }
    // tag 值标记为 addr, len 为整个数据的长度
    if (MsgRecord::Init(m_sockaddr.sin_addr.s_addr, data.ByteSizeLong() + m_RecordSize) < 0) {
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

    TRACER("recvmsg from fd %d\n", TcpSocket::GetSocketfd());

    TcpSocket::RecvData(m_pRecord->m_data, m_RecordSize);
    Decode();
    TcpSocket::RecvData(m_pRecord->m_data + m_RecordSize, m_pRecord->m_len - m_RecordSize);
    
    TRACER("parse data to protobuf\n");

    data.ParseFromArray(m_pRecord->m_data + m_RecordSize, m_pRecord->m_len - m_RecordSize);
    return 0;
}