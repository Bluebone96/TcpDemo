#ifndef _PLAYER_H_
#define _PLAYER_H_


#include <cstring>
#include <vector>
#include <unordered_map>
#include <string>
#include <sys/time.h>

#include "Bag.h"
#include "../Common/basetype.h"
#include "../Common/MsgTransmission.hpp"

class Player {
public:

    explicit Player(MsgTrans* msg = nullptr) : m_pos(0), m_msgTrans(msg) { if (msg) { m_Id = msg->GetSocketfd(); }}

    ~Player() { 
        TRACER("player dctor delete msgtrans\n");
        delete m_msgTrans;
        m_msgTrans = nullptr; 
    }

    int InitPlayer();
    
    int Update();

 //   int setPlayerInfo();

    int getPlayerStatus(char*, int);

    int getId() { return m_Id; }

    int Activity() { return gettimeofday(&m_offline, nullptr); }

    int isActivity() 
    {
        struct timeval cur;
        gettimeofday(&cur, nullptr);
        if (cur.tv_sec - m_offline.tv_sec < 5) {
            m_offline = cur;
            return 0;
        }
        return -1;
    }

    PROTOBUF& GetPlayerInfo();
    
    PROTOBUF& GetPlayerOp();

private:

    int32_t m_Id; // 暂时定为 acceptfd
    std::string m_name;

#define MAXSTATUS 5
    // 定义一个环形结构
    // PlayerStatus m_pStatus[MAXSTATUS];
    
    UINT32 m_pos;

    Proto::Unity::PlayerInfo m_protoInfo;

    Proto::Unity::Operation m_opNew;

    struct  timeval  m_offline; // 5秒 无响应下线

    struct  timeval  lastTimeUp;

    BaseBag m_baseBag;
    EquipBag m_equipBag;
    MoneyBag m_moneyBag;

public:
    MsgTrans* m_msgTrans;
};


#endif
