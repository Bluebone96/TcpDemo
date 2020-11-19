#include "Player.h"
#include "../Common/MsgTransmission.hpp"

int Player::InitPlayer()
{
    TAG T;
    m_Id = m_msgTrans->GetSocketfd(); 
    // 直接取 玩家发过来的 用户名 和 密码 进行初始化
    Proto::Unity::Authentication A;
    m_msgTrans->RecvProtobuf(A);
    m_name = A.name();
    
    PlayerStatus& currStatus = m_pStatus[m_pos];
    m_protoInfo.set_name(currStatus.m_name);
    m_protoInfo.set_id(currStatus.m_id);
    
    return 0;
}


int Player::Update()
{
    struct timeval  curTime;

    gettimeofday(&curTime, nullptr);
    
    if (curTime.tv_sec - m_offline.tv_sec > 5) {
        // TODO 离线
    }
    m_offline = curTime;

    long passtime = (curTime.tv_sec  - lastTimeUp.tv_sec) * 1000 + (curTime.tv_usec - lastTimeUp.tv_usec) / 1000;

 
    PlayerStatus nextStatus = m_pStatus[(m_pos + 1) % MAXSTATUS];
    nextStatus = m_pStatus[m_pos];

    nextStatus.m_position[0] += (passtime * nextStatus.m_speed * ( m_operation.op._w - m_operation.op._s) / 1000);

    nextStatus.m_position[2] += (passtime * nextStatus.m_speed * (m_operation.op._a - m_operation.op._d) / 1000);
    
    setPlayerOp();

    m_operation.val = 0;
}


int Player::setPlayerOp()
{
    m_msgTrans->RecvProtobuf(m_protoOp);
    m_operation.val = m_protoOp.op();
    if (m_operation.val == 0) {
        gettimeofday(&m_offline, nullptr);  // 无操作可表示心跳包
    } else {
        Update();
    }
    return EOK;
}

int Player::setPlayerStatus()
{

    PlayerStatus& currStatus = m_pStatus[m_pos];
    for (int i = 0; i < 3; ++i) {
        m_protoInfo.add_position(currStatus.m_position[i]);
        m_protoInfo.add_rotation(currStatus.m_rotation[i]);
    }

    m_protoInfo.set_hp(currStatus.m_hp);
    m_protoInfo.set_mp(currStatus.m_mp);
    m_protoInfo.set_speed(currStatus.m_speed);
    m_protoInfo.set_state(currStatus.m_state);
    
    return ERR;
}


PROTOBUF& Player::GetPlayerInfo()
{

    Update();

    setPlayerStatus();

    return m_protoInfo;
}
