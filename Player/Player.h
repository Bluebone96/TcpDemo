#ifndef _PLAYER_H_
#define _PLAYER_H_


#include <cstring>
#include <vector>
#include <unordered_map>
#include <string>
#include <sys/time.h>

#include "../Common/basetype.h"
#include "../Common/MsgTransmission.hpp"
#include "../SQL/toredis.h"


#define MYSQLPP_SSQLS_NO_STATICS

#include "../SQL/tomysql.h"


#include "Inventory.h"


class Player {
public:
    explicit Player(MsgTrans* msg = nullptr) :  m_msgTrans(msg) { }

    ~Player() { 
        TRACER("player dctor delete msgtrans\n");
        saveAll();
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


    int updateInventroy( /* Message* */);

    int savePlayer();

    int saveAll();

    int getPlayer();

    Proto::Unity::PlayerAllFuckInfo&  getAllFuckInfo();

private:

    int32_t m_Id; 

    char m_idstr[20];
    std::string m_name;

#define MAXSTATUS 5
    // 定义一个环形结构
    // PlayerStatus m_pStatus[MAXSTATUS];
    

    Proto::Unity::PlayerInfo m_protoInfo;

    Proto::Unity::Operation m_opNew;

    Proto::Unity::PlayerAllFuckInfo m_fuckAllPb;

    struct  timeval  m_offline; // 5秒 无响应下线

    struct  timeval  lastTimeUp;

    Inventory m_inventory;

public:
    MsgTrans* m_msgTrans;
};


#endif
