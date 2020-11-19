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
#include "../Common/MsgTransmission.hpp"

class Player {
public:

    explicit Player(MsgTrans* msg = nullptr) : m_pos(0), m_msgTrans(msg) {}

    ~Player() { delete m_msgTrans; }

    int InitPlayer();
    
    int Update();

    int getPlayerOperation();

    int sendPlayerStatus();

    int getPlayerStatus(char*, int);



private:

    int32_t m_Id; // 暂时定为 acceptfd
    std::string m_name;

#define MAXSTATUS 5
    // 定义一个环形结构
    PlayerStatus* m_mainStatus;     
    PlayerStatus m_pStatus[MAXSTATUS];
    UINT32 m_pos;
    

    Proto::Unity::PlayerInfo m_protoInfo;
    Proto::Unity::Operation m_protoOp;

    OPERATION m_operation; 

    float m_offline;

    float m_lastUptime;

    Inventory m_inventory;

    Proto::Unity::PlayerInfo& GetPlayerInfo();
    Proto::Unity::Operation& GetPlayerOp();


public:
    MsgTrans* m_msgTrans;
};


#endif
