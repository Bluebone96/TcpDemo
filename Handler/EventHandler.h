#ifndef _EVENTHANDLER_H_
#define _EVENTHANDLER_H_

#include <unordered_map>

#include "Handle.hpp"
#include "../Player/Player.h"
#include "../Server/Server.h"

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
