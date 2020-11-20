#include "Player.h"
#include "../Common/MsgTransmission.hpp"

int Player::InitPlayer()
{
    m_Id = m_msgTrans->GetSocketfd(); 
    // 直接取 玩家发过来的 用户名 和 密码 进行初始化
    Proto::Unity::Authentication A;
    m_msgTrans->Decode(A);
    m_name = A.name();
    
    PlayerStatus& currStatus = m_pStatus[m_pos];
    m_protoInfo.set_name(currStatus.m_name);
    m_protoInfo.set_id(currStatus.m_id);
    
    return 0;
}


int Player::Update()
{
    m_msgTrans->Decode(m_opNew);

    struct timeval  curTime;

    gettimeofday(&curTime, nullptr);
    
    if (curTime.tv_sec - m_offline.tv_sec > 5) {
        // TODO 离线
    }
    m_offline = curTime;

    long passtime = (curTime.tv_sec  - lastTimeUp.tv_sec) * 1000 + (curTime.tv_usec - lastTimeUp.tv_usec) / 1000;

 
    PlayerStatus nextStatus = m_pStatus[(m_pos + 1) % MAXSTATUS];

    nextStatus = m_pStatus[m_pos];

    nextStatus.m_position[0] += (passtime * nextStatus.m_speed * ( m_opOld.w() - m_opOld.s()) / 10);

    nextStatus.m_position[2] += (passtime * nextStatus.m_speed * (m_opOld.a() - m_opOld.d()) / 10);
    

    m_opOld = m_opNew;

    return 0;
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

