#include "Player.h"


void Player::Update()
{
    PlayerStatus nextStatus = m_pStatus[(m_pos + 1) % MAXSTATUS];
    nextStatus = m_pStatus[m_pos];

     if (m_operation.op._w ^ m_operation.op._s) {
        nextStatus.m_position[0] += nextStatus.m_speed * (2 * m_operation.op._w - 1);
    }

     if (m_operation.op._a ^ m_operation.op._d) {
        nextStatus.m_position[2] += nextStatus.m_speed * (2 * m_operation.op._a - 1);
    }

     nextStatus.m_position[1] += nextStatus.m_jump * m_operation.op._jp;

     //

     m_pos = (m_pos + 1) % MAXSTATUS;
    m_operation.val = 0;
}


int Player::getPlayerOperation()
{
    m_msgTrans->recvmsg(m_protoOp);
    m_operation.val = m_protoOp.op();
    if (m_operation.val == 0) {
        m_offline = 0;  // 无操作可表示心跳包
    } else {
        Update();
    }
    return EOK;
}

int Player::sendPlayerStatus() 
{
    if (m_offline == 5) {
        // todo , 超5秒下线
    }
    PlayerStatus& currStatus = m_pStatus[m_pos];
    for (int i = 0; i < 3; ++i) {
        m_protoInfo.add_position(currStatus.m_position[i]);
        m_protoInfo.add_rotation(currStatus.m_rotation[i]);
    }
    
    // todo
    if (m_msgTrans->sendmsg(m_protoInfo) == 0) {
        return EOK;
    }
    return ERR;
}