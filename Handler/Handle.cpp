#include "Handle.h"
#include "Dispatcher.h"
#include "../Player/Player.h"
#include "../Server/Server.h"


int HandleUserLogin::operator()(void* _s)
{

    TRACER("HandleUserLogin\n");
    // TODO
    //SERVER.
    Player* player = static_cast<Player*>(_s);

    (void) player->InitPlayer();
    
    player->m_msgTrans->SetEventType(static_cast<unsigned char>(EventType::USERLOGIN));

    player->m_msgTrans->SetID(player->getId());

    player->m_msgTrans->sendmsg(player->GetPlayerInfo());

    player->m_msgTrans->SetEventType(static_cast<unsigned char>(EventType::USERUP));

    player->m_msgTrans->Encode();

    SERVER.SendMsgToAll(player->m_msgTrans->GetDataAddress(), player->m_msgTrans->GetLen());

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
    Player *p = static_cast<Player*>(_p);
    return p->Activity();
}


int HandleUpdateStatus::operator()(void * _p)
{

    Player* player = static_cast<Player*>(_p);

    player->m_msgTrans->SetEventType(static_cast<unsigned char>(EventType::USERUP));

    player->m_msgTrans->Encode(player->GetPlayerInfo());

    SERVER.SendMsgToAll(player->m_msgTrans->GetDataAddress(), player->m_msgTrans->GetLen());

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
            DISPATCHER.Process(EventType::USERUP, p);
        } else {
            DISPATCHER.Process(EventType::USEREXIT, p);
        }
    }


    // SERVER.SendMsgToAll(allstatus, 4096 - left);
    
    // delete[] allstatus;

    return 0;

}

