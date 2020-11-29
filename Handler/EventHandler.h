#ifndef _EVENTHANDLER_H_
#define _EVENTHANDLER_H_

#include <unordered_map>

#include "Handle.h"
#include "../Common/basetype.h"


enum EventType {
    USERLOGIN = 0,
    USEREXIT = 1,
    USERALIVE = 2,
    USERUP = 3,
    USERSYNC = 4,
    USERCHAT = 5,
};

class EventHandler {
public:
    EventHandler();
    
    ~EventHandler();

    int InitHandler();

    int AddHandle(EventType, Handle*);

    int DelHandle(EventType);
    
    Handle* Notify(EventType);

protected:
    std::unordered_map<EventType, Handle*> m_handleEvents;

};

#endif
