#ifndef _PLAYER_H_
#define _PLAYER_H_


#include <cstring>
#include <vector>
#include <unordered_map>
#include <string>
#include <sys/time.h>

#include "../Common/basetype.h"
#include "../Common/log.h"
#include "../Net/message.h"

#define MYSQLPP_SSQLS_NO_STATICS

#include "../SQL/tomysql.h"

#include "Inventory.h"


class Player {
public:
    Player() : m_id(0), m_name(), m_fuckAllPb(), m_protoInfo(nullptr), m_opNew(), m_inventory()
    { 
        gettimeofday(&m_lastTimeUp, nullptr); 
        gettimeofday(&m_offline, nullptr); 

    }

    ~Player() 
    { 
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
    

    // int update_status(Proto::Unity::Operation& op);

    int update_status(message* msg);

    int update_Inventory(Proto::Unity::ItemEvent& pb);


    int save_allinfo();


    Proto::Unity::PlayerAllFuckInfo&  getAllFuckInfo();


private:

    uint32_t m_id; 

    std::string m_name;

#define MAXSTATUS 5
    // 定义一个环形结构
    // PlayerStatus m_pStatus[MAXSTATUS];
    
    Proto::Unity::PlayerAllFuckInfo m_fuckAllPb;

    Proto::Unity::PlayerInfo* m_protoInfo; // 指向 m_fuckallpb 的 baseinfo

    Proto::Unity::Operation m_opNew;


    struct  timeval  m_offline; // 5秒 无响应下线

    struct  timeval  m_lastTimeUp;

    uint64_t m_lastTick;

    Inventory m_inventory;
};


#endif
