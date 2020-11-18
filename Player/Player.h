#ifndef _PLAYER_H_
#define _PLAYER_H_


#include <cstring>
#include <vector>
#include <unordered_map>
#include <string>


#include "command.cpp"
#include "PlayerStatus.h"
#include "Inventory.hpp"
#include "../Common/basetype.h"

class Player {
public:

    Player() : m_pos(0), m_msgTrans(nullptr) {}

    ~Player() { delete m_msgTrans; }

    void InitPlay(MsgTrans* _msgT) { m_msgTrans = _msgT; }
    
    void Update();

    int getPlayerOperation();

    int sendPlayerStatus();

private:

#define MAXSTATUS 5
    // 定义一个环形结构
    PlayerStatus m_pStatus[MAXSTATUS];
    UINT32 m_pos;
    
    MsgTrans* m_msgTrans;

    Proto::Unity::PlayerInfo m_protoInfo;
    Proto::Unity::Operation m_protoOp;

    OPERATION m_operation; 

    float m_offline;

    float m_lastUptime;

    Inventory m_inventory;

};


#endif
