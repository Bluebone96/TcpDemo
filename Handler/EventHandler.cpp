#include <utility>
#include "EventHandler.h"
#include "../Common/log.h"

EventHandler::EventHandler()
{

}

EventHandler::~EventHandler()
{
    for(auto& iter : m_handleEvents) {
        delete iter.second;
    }
}

int EventHandler::InitHandler()
{
    int flag = 0;
    flag |= AddHandle(MsgType::USERLOGIN, new HandleUserLogin);

    flag |= AddHandle(MsgType::USEREXIT, new HandleUserExit);

    flag |= AddHandle(MsgType::USERALIVE, new HandleUserAlive);
    
    flag |= AddHandle(MsgType::USERUP, new HandleUpdateStatus);

    flag |= AddHandle(MsgType::USERSYNC, new HandleSyncClient);

    flag |= AddHandle(MsgType::ITEMEVENT, new HandleEventItem);

    return 0;
}

int EventHandler::AddHandle(MsgType _type, Handle* _handle)
{
    auto res = m_handleEvents.insert(std::move(std::make_pair(_type, _handle)));
    if (!res.second) {
        TRACER("EventHandler insert failed type %d, try replace the old _hanle function\n", static_cast<int>(_type));
        m_handleEvents[_type] = _handle;
        return EOK;
    }
    return EOK;
}

int EventHandler::DelHandle(MsgType _type)
{
    auto iter = m_handleEvents.find(_type);
    if (iter != m_handleEvents.end()) {
        delete iter->second;
        m_handleEvents.erase(iter);
        return EOK;
    }
    return ERR;
}

Handle* EventHandler::Notify(MsgType _type)
{
    Handle* handle = nullptr;
    try {
        handle = m_handleEvents.at(_type);
    } catch (...) {
        TRACER("maybe out_of_range exception, %s:%d", __POSITION__);
        handle = nullptr;
    }
    return handle;
}