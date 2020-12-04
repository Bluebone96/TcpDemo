#include "Handle.h"
#include "Dispatcher.h"
#include "../Player/Player.h"
#include "../Server/Server.h"


int HandleUserLogin::operator()(void* _s)
{

    TRACER("HandleUserLogin\n");
    // TODO
    // SERVER.
    Player* player = static_cast<Player*>(_s);

    if (player->InitPlayer() < 0) {
        return -1;
    }
    
    player->m_msgTrans->SetType(static_cast<unsigned char>(MsgType::USERLOGIN));

    player->m_msgTrans->SetId(player->getId());

    player->m_msgTrans->sendmsg(player->getAllFuckInfo());

    // player->m_msgTrans->SetType(static_cast<unsigned char>(MsgType::USERSYNC));

    // player->m_msgTrans->Encode(player->GetPlayerInfo());

    // SERVER.SendMsgToAll(player->m_msgTrans->GetDataAddress(), player->m_msgTrans->GetLen() + player->m_msgTrans->m_RecordSize);

    TRACER("HandleUserLogin end\n");

    return 0;
}


int HandleUserExit::operator()(void* _p)
{
    // Player* p = static_cast<Player*>(_p);
    
    // // TODO
    
    return 0;
}




int HandleUserAlive::operator()(void* _p)
{
    TRACER("HandleUserkeepAlive\n");

    Player *p = static_cast<Player*>(_p);
    return p->Activity();
}


int HandleUpdateStatus::operator()(void * _p)
{
    TRACER("HandleUserUpdate\n");

    Player* player = static_cast<Player*>(_p);

    player->m_msgTrans->SetType(static_cast<unsigned char>(MsgType::USERSYNC));

    TRACER("Test HandleUPdate\n");
    player->m_msgTrans->Encode(player->GetPlayerInfo());
    TRACER("Test HandleUPdate end\n");

    SERVER.SendMsgToAll(player->m_msgTrans->GetDataAddress(), player->m_msgTrans->GetLen() + player->m_msgTrans->m_RecordSize);

    return 0;
}





int HandleSyncClient::operator()(void * _s)
{

    TRACER("HandleSyncClient\n");

    auto playersMap = static_cast<Server::PLAYERMAP *>(_s);
    
    // char* allstatus = new char[4096]; // 保存所有玩家的状态, 较滞后，复杂度 n
    // char* p = allstatus;
    
    // int left = 4096;
    // int n = 0;
    Player* p = nullptr;
    for (auto& iter : *playersMap) {
        p = iter.second;
        if (p->isActivity() == 0) {
            DISPATCHER.Process(MsgType::USERUP, p);
        } else {
            DISPATCHER.Process(MsgType::USEREXIT, p);
        }
    }


    // SERVER.SendMsgToAll(allstatus, 4096 - left);
    
    // delete[] allstatus;

    return 0;

}


int HandleEventItem::operator()(void *_s)
{
    Player* player = static_cast<Player*>(_s);

    player->updateInventroy();
    
    return 0;
}