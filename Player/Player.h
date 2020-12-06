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
    explicit Player() { }

    ~Player() { 
        TRACER("player dctor delete msgtrans\n");
        // saveAll();交给 gameserver
    }


    int InitPlayer(Proto::Unity::PlayerAllFuckInfo&);
    
    int getId() { return m_id; }

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

    const Proto::Unity::PlayerInfo* GetPlayerInfo();
    
    PROTOBUF& GetPlayerOp();

    int update_status(Proto::Unity::Operation& op);

    int update_Inventory(Proto::Unity::ItemUpdate& pb);


    int save_allinfo();


    Proto::Unity::PlayerAllFuckInfo&  getAllFuckInfo();


private:

    int32_t m_id; 

    std::string m_name;

#define MAXSTATUS 5
    // 定义一个环形结构
    // PlayerStatus m_pStatus[MAXSTATUS];
    
    Proto::Unity::PlayerAllFuckInfo m_fuckAllPb;

    Proto::Unity::PlayerInfo* m_protoInfo;

    Proto::Unity::Operation m_opNew;


    struct  timeval  m_offline; // 5秒 无响应下线

    struct  timeval  lastTimeUp;

    Inventory m_inventory;

};


#endif
