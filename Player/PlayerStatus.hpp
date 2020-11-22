#include <cstring>
#include <vector>
#include <unordered_map>
#include <string>

//#include "command.cpp"
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
    float m_hp;

    float m_posX;
    float m_posZ;
    float m_angle;

    float m_speed;

    STATE m_state;


public:

    PlayerStatus() : m_id(0), m_name(0), m_hp(100), m_posX(0), m_posZ(0), 
                     m_angle(0), m_speed(10), m_state(IDEL) {}
    
    PlayerStatus(const PlayerStatus& _status)
    {
        m_id = _status.m_id;
        m_name = _status.m_name;
        m_hp = _status.m_hp;
        m_posX = _status.m_posX;
        m_posZ = _status.m_posZ;
        m_speed = _status.m_speed;
        m_state = _status.m_state;
    }


    PlayerStatus& operator=(const PlayerStatus& _status)
    {
        m_id = _status.m_id;
        m_name = _status.m_name;
        m_hp = _status.m_hp;
        m_posX = _status.m_posX;
        m_posZ = _status.m_posZ;
        m_speed = _status.m_speed;
        m_state = _status.m_state;
        return *this;
    }

    

};