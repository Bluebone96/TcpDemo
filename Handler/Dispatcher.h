#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

#include "../Common/Singleton.h"
#include "EventHandler.h"
#include "../Player/Player.h"
class Dispatcher : public EventHandler {
public:

    Dispatcher() { };
    ~Dispatcher() { };

    int Init() { return InitHandler();}

    int Process(Player*);

    int Process();

    int Process(MsgType, void* = nullptr);

};

#define DISPATCHER Singleton<Dispatcher>::GetInstance()

#endif
