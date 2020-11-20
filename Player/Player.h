#ifndef _PLAYER_H_
#define _PLAYER_H_


#include <cstring>
#include <vector>
#include <unordered_map>
#include <string>
#include <sys/time.h>

#include "PlayerStatus.hpp"
#include "Inventory.hpp"
#include "../Common/basetype.h"
#include "../Common/MsgTransmission.hpp"

class Player {
public:

    explicit Player(MsgTrans* msg = nullptr) : m_pos(0), m_msgTrans(msg) { if (msg) { m_Id = msg->GetSocketfd(); }}

    ~Player() { delete m_msgTrans; }

    int InitPlayer();
    
    int Update();

    int setPlayerOp();

    int setPlayerStatus();

    int getPlayerStatus(char*, int);

    int getId() { return m_Id; }

    PROTOBUF& GetPlayerInfo();
    PROTOBUF& GetPlayerOp();

private:

    int32_t m_Id; // 暂时定为 acceptfd
    std::string m_name;

#define MAXSTATUS 5
    // 定义一个环形结构
    PlayerStatus m_pStatus[MAXSTATUS];
    
    UINT32 m_pos;
    

    Proto::Unity::PlayerInfo m_protoInfo;
    Proto::Unity::Operation m_protoOp;

    OPERATION m_operation; 

    struct  timeval  m_offline; // 5秒 无响应下线

    struct  timeval  lastTimeUp;

    Inventory m_inventory;

public:
    MsgTrans* m_msgTrans;
};


#endif
