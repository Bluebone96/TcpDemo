#include "Player.h"
#include "../Common/MsgTransmission.hpp"

int Player::InitPlayer()
{
    m_Id = m_msgTrans->GetSocketfd(); 
    // 直接取 玩家发过来的 用户名 和 密码 进行初始化
    Proto::Unity::Authentication A;
    m_msgTrans->Decode(A);
    m_name = A.name();
    TRACER("player name: %s\n id: %d", A.name().c_str(), m_Id);

    m_protoInfo.set_id(m_Id);
    m_protoInfo.set_name(m_name);
    m_protoInfo.set_posx(random() % 10);
    m_protoInfo.set_posz(random() % 10);
    m_protoInfo.set_speed(10);
    m_protoInfo.set_state(0);
    return 0;
}


int Player::Update()
{
    TRACER("test Player UpDate\n");
    struct timeval  curTime;

    m_msgTrans->Decode(m_opNew);
    TRACER("new op h = %f, v = %f\n", m_opNew.h(), m_opNew.v());

    gettimeofday(&curTime, nullptr);
    
    if (curTime.tv_sec - m_offline.tv_sec > 5) {
        // TODO 离线
    }

    m_offline = curTime;


    float passtime = (curTime.tv_sec  - lastTimeUp.tv_sec) * 1000 + (curTime.tv_usec - lastTimeUp.tv_usec) / 1000;

    TRACER("passtime is %f\n", passtime);
    // PlayerStatus nextStatus = m_pStatus[(m_pos + 1) % MAXSTATUS];

    // nextStatus = m_pStatus[m_pos];

    m_protoInfo.set_posx(m_protoInfo.posx() + (passtime * m_protoInfo.speed() * m_protoInfo.op().h()) / 1000);
    m_protoInfo.set_posz(m_protoInfo.posz() + (passtime * m_protoInfo.speed() * m_protoInfo.op().v()) / 1000);
    m_protoInfo.mutable_op()->CopyFrom(m_opNew);

    TRACER("player posx = %f, posz = %f\n", m_protoInfo.posx(), m_protoInfo.posz());
    TRACER("test Player UpDate end\n");
    lastTimeUp = curTime;
    return 0;
}


// int Player::setPlayerInfo()
// {


//     m_protoInfo.set_id(currStatus.m_id);
//     m_protoInfo.set_hp(currStatus.m_hp);
//     m_protoInfo.set_speed(currStatus.m_speed);
//     m_protoInfo.set_state(currStatus.m_state);
    
//     return ERR;
// }


PROTOBUF& Player::GetPlayerInfo()
{

    TRACER("test PlayerGetPlayerInfo\n");
    Update();

//    setPlayerInfo();

    TRACER("test PlayerGetPlayerInfo end\n");
    return m_protoInfo;
}

