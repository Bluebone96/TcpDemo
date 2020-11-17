//#include "../Common/base.h"
#include <cstring>
#include <vector>
#include <unordered_map>
#include "command.cpp"
#include "../Common/basetype.h"
#include "../Common/MsgTransmission.hpp"

enum STATE {
    IDEL,
    WALK
};

// class Item {
//     UINT32 itemID;
//     UINT32 count;
    
//     void InitItem(UINT32 id, UINT32 n = 1) { itemID = id; count = 1; }
//     Item& operator+(UINT32 n) { count += n;  return *this; }
//     Item& operator-(UINT32 n) { count -= n;  return *this; }
// };

class Inventory {
private:
//    std::vector<Item> m_itemList;
    std::unordered_map<UINT32, UINT32> m_itemList;

public:
    Inventory() {}
    Inventory(const Inventory& _in) { m_itemList = _in.m_itemList; }

    Inventory& operator=(const Inventory& _in)
    {
        m_itemList = _in.m_itemList;
        return *this;
    }

    Inventory(Inventory&& _in) : m_itemList(std::move(_in.m_itemList)) {}

    Inventory& operator=(Inventory&& _in) 
    { 
        m_itemList = std::move(_in.m_itemList);
        return *this;
    }

    int AddItem(UINT32 _id, UINT32 _n)
    {
        auto iter = m_itemList.find(_id);
        if (iter != m_itemList.end()) {
            iter->second += _n;
        } else
        {
            m_itemList[_id] = _n;
        }
        return EOK;
    }


    int DelItem(UINT32 _id, UINT32 _n)
    {
        auto iter = m_itemList.find(_id);
        if (iter != m_itemList.end()) {
            if (iter->second < _n) {
                return ERR;
            } else if (iter->second == _n) {
                m_itemList.erase(iter);
            } else {
                iter->second -= _n;
            }
        }
        return ERR;
    }


};


struct PlayerStatus {
    float m_position[3];
    float m_rotation[3];

    float m_hp;
    float m_mp;

    float m_speed;
    float m_jump;

    STATE m_state;

    Inventory m_inventory;

    PlayerStatus() : m_position({0}), m_rotation({0}), m_hp(0), m_mp(0), m_state(IDEL) {}
    PlayerStatus(const PlayerStatus& _status)
    {
        memcpy(&m_position, &_status.m_position, 3 * sizeof(float));
        memcpy(&m_rotation, &_status.m_rotation, 3 * sizeof(float));
        m_hp = _status.m_hp;
        m_mp = _status.m_mp;
        m_state = _status.m_state;
        m_inventory = _status.m_inventory;
    }

        
    PlayerStatus(PlayerStatus&& _status)
        : m_hp(_status.m_hp), m_mp(_status.m_mp), m_state(_status.m_state), m_inventory(std::move(_status.m_inventory))
    {
        memcpy(&m_position, &_status.m_position, 3 * sizeof(float));
        memcpy(&m_rotation, &_status.m_rotation, 3 * sizeof(float));
    }

    PlayerStatus& operator=(const PlayerStatus& _status)
    {
        memcpy(&m_position, &_status.m_position, 3 * sizeof(float));
        memcpy(&m_rotation, &_status.m_rotation, 3 * sizeof(float));
        m_hp = _status.m_hp;
        m_mp = _status.m_mp;
        m_state = _status.m_state;
        m_inventory = _status.m_inventory;
        return *this;
    }

    PlayerStatus& operator=(PlayerStatus&& _status)
    {
        memcpy(&m_position, &_status.m_position, 3 * sizeof(float));
        memcpy(&m_rotation, &_status.m_rotation, 3 * sizeof(float));
        m_hp = _status.m_hp;
        m_mp = _status.m_mp;
        m_state = _status.m_state;
        m_inventory = std::move(_status.m_inventory);

        return *this;
    }

};


class Player {
private:
//    std::vector<PlayerStatus> m_status;
#define MAXSTATUS 5
    // 定义一个环形结构
    PlayerStatus m_pStatus[MAXSTATUS];
    UINT32 m_pos;
    
    MsgTrans* m_msgTrans;

    Proto::Unity::PlayerInfo m_protobuf;
public:
    
    Player() : m_pos(0), m_msgTrans(nullptr) {}

    ~Player() { delete m_msgTrans; }

    void InitPlay(MsgTrans* _msgT) { m_msgTrans = _msgT; }
    
    void Update(OPERATION& _OP)
    {
        PlayerStatus nextStatus = m_pStatus[(m_pos + 1) % MAXSTATUS];
        nextStatus = m_pStatus[m_pos];

        if (_OP.op._w ^ _OP.op._s) {
            nextStatus.m_position[0] += nextStatus.m_speed * (2 * _OP.op._w - 1);
        }

        if (_OP.op._a ^ _OP.op._d) {
            nextStatus.m_position[2] += nextStatus.m_speed * (2 * _OP.op._a - 1);
        }

        nextStatus.m_position[1] += nextStatus.m_jump * _OP.op._jp;

        //

        m_pos = (m_pos + 1) % MAXSTATUS;
    }


    int getPlayerOperation()
    {
        
    }

    int sendPlayerStatus() 
    {
        PlayerStatus& currStatus = m_pStatus[m_pos];

        for (int i = 0; i < 3; ++i) {
            m_protobuf.add_position(currStatus.m_position[i]);
            m_protobuf.add_rotation(currStatus.m_rotation[i]);
        }
        
        // todo

        if (m_msgTrans->sendmsg(m_protobuf) == 0) {
            return EOK;
        }

        return ERR;
    }

};



