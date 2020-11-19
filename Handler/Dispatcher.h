#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

#include "EventHandler.h"


class Dispatcher {
public:
    Dispatcher();

    ~Dispatcher();

    int Process(Player*);

    int Notify(Player*);
    int Notify();
private:
    EventHandler m_eventhandler;
};

#endif
