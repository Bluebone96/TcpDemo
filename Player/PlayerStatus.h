#include <cstring>
#include <vector>
#include <unordered_map>
#include <string>

#include "command.cpp"
#include "../Common/base.h"
#include "../Common/basetype.h"
#include "../Common/MsgTransmission.hpp"




class PlayerStatus {

public:
    enum STATE {
        IDEL,
        WALK
    };
    
    UINT32 m_id;
    std::string m_name;

    float m_position[3] = {0};
    float m_rotation[3] = {0};

    float m_hp;
    float m_mp;

    float m_speed;
    float m_jump;

    STATE m_state;


public:

    PlayerStatus() : m_hp(0), m_mp(0), m_state(IDEL) {}
    
    PlayerStatus(const PlayerStatus& _status)
    {
        memcpy(&m_position, &_status.m_position, 3 * sizeof(float));
        memcpy(&m_rotation, &_status.m_rotation, 3 * sizeof(float));
        m_hp = _status.m_hp;
        m_mp = _status.m_mp;
        m_state = _status.m_state;
    }


    PlayerStatus& operator=(const PlayerStatus& _status)
    {
        memcpy(&m_position, &_status.m_position, 3 * sizeof(float));
        memcpy(&m_rotation, &_status.m_rotation, 3 * sizeof(float));
        m_hp = _status.m_hp;
        m_mp = _status.m_mp;
        m_state = _status.m_state;
        return *this;
    }


};