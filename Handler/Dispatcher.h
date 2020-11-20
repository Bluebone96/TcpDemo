#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

#include "EventHandler.h"
#include "../Common/Singleton.h"

class Dispatcher {
public:

    Dispatcher() { };
    ~Dispatcher() { };

    int Init() { return m_eventhandler.InitHandler();}

    int Process(Player*);

    int Process();

    int Process(EventType, void* = nullptr);
private:

    EventHandler m_eventhandler;
};

#define DISPATCHER Singleton<Dispatcher>::GetInstance()

#endif
