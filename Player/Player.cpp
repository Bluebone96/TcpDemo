#include <cstdlib>
#include <vector>

#include "Player.h"
#include "../Proto/PlayerInfo.pb.h"
#include "../Net/message.h"



extern msg_queue g_send_queue;
extern std::map<uint32_t, uint32_t> g_connet_server;

unsigned int BKDRHash(const std::string& _str)
{
    unsigned int seed = 1313;
    unsigned int hash = 0;
    for (auto& c : _str) {
        hash = hash * seed + c;
    }
    return (hash & 0x7FFFFFFF);
}


int Player::InitPlayer(Proto::Unity::PlayerAllFuckInfo& _allinfo)
{
    m_fuckAllPb = std::move(_allinfo);

    m_name = m_fuckAllPb.baseinfo().name();
    m_id = m_fuckAllPb.baseinfo().id();

    m_protoInfo = m_fuckAllPb.mutable_baseinfo();

    TRACER_DEBUG("player name: %s, id: %d\n", m_name.c_str(), m_id);
    TRACER_DEBUG("player posx: %f, posz: %f, speed %d\n", m_protoInfo->posx(), m_protoInfo->posz(), m_protoInfo->speed());

    m_inventory.InitInventory(m_fuckAllPb.mutable_baginfo(), this);
    
    return 0;
}


int Player::update_status(Proto::Unity::Operation& op)
{
    struct timeval  curTime;
    m_opNew = op;
    TRACER_DEBUG("new op h = %f, v = %f\n", m_opNew.h(), m_opNew.v());

    gettimeofday(&curTime, nullptr);
    
    if (curTime.tv_sec - m_offline.tv_sec > 5) {
        // TODO 离线
    }

    m_offline = curTime;


    float passtime = (curTime.tv_sec  - m_lastTimeUp.tv_sec) * 1000 + (curTime.tv_usec - m_lastTimeUp.tv_usec) / 1000;

    TRACER_DEBUG("passtime is %f\n", passtime);
    // PlayerStatus nextStatus = m_pStatus[(m_pos + 1) % MAXSTATUS];

    // nextStatus = m_pStatus[m_pos];
    TRACER_DEBUG("speed is %d, old op h = %f, v = %f\n", m_protoInfo->speed(), m_protoInfo->op().h(), m_protoInfo->op().v());

    m_protoInfo->set_posx(m_protoInfo->posx() + (passtime * m_protoInfo->speed() * m_protoInfo->op().h()) / 1000);
    m_protoInfo->set_posz(m_protoInfo->posz() + (passtime * m_protoInfo->speed() * m_protoInfo->op().v()) / 1000);
    m_protoInfo->mutable_op()->operator=(m_opNew);

    TRACER_DEBUG("player posx = %f, posz = %f\n", m_protoInfo->posx(), m_protoInfo->posz());
    m_lastTimeUp = curTime;
    return 0;
}


// int Player::setPlayerInfo()
// {


//     m_protoInfo->set_id(currStatus.m_id);
//     m_protoInfo->set_hp(currStatus.m_hp);
//     m_protoInfo->set_speed(currStatus.m_speed);
//     m_protoInfo->set_state(currStatus.m_state);
    
//     return ERR;
// }


const Proto::Unity::PlayerInfo* Player::GetPlayerInfo()
{

    TRACER_DEBUG("test PlayerGetPlayerInfo\n");
    update_status(m_opNew);

    TRACER_DEBUG("PlayerGetPlayerInfo end\n");
    return m_protoInfo;
}



int Player::save_allinfo()
{

    return 0;
}





Proto::Unity::PlayerAllFuckInfo&  Player::getAllFuckInfo()
{
    // 先通知背包把所有物品信息保存到pb里
    m_inventory.saveAll();

    return m_fuckAllPb;
}


int Player::update_Inventory(Proto::Unity::ItemEvent& pb)
{
    return m_inventory.update_item(pb);
}
