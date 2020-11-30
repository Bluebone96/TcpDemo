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
    flag |= AddHandle(EventType::USERLOGIN, new HandleUserLogin);

    flag |= AddHandle(EventType::USEREXIT, new HandleUserExit);

    flag |= AddHandle(EventType::USERALIVE, new HandleUserAlive);
    
    flag |= AddHandle(EventType::USERUP, new HandleUpdateStatus);

    flag |= AddHandle(EventType::USERSYNC, new HandleSyncClient);

    flag |= AddHandle(EventType::ITEMEVENT, new HandleEventItem);

    return 0;
}

int EventHandler::AddHandle(EventType _type, Handle* _handle)
{
    auto res = m_handleEvents.insert(std::move(std::make_pair(_type, _handle)));
    if (!res.second) {
        TRACER("EventHandler insert failed type %d, try replace the old _hanle function\n", static_cast<int>(_type));
        m_handleEvents[_type] = _handle;
        return EOK;
    }
    return EOK;
}

int EventHandler::DelHandle(EventType _type)
{
    auto iter = m_handleEvents.find(_type);
    if (iter != m_handleEvents.end()) {
        delete iter->second;
        m_handleEvents.erase(iter);
        return EOK;
    }
    return ERR;
}

Handle* EventHandler::Notify(EventType _type)
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