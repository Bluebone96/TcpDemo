#ifndef _EVENTHANDLER_H_
#define _EVENTHANDLER_H_

#include <unordered_map>
#include "../Player/Player.h"
#include "../Server/Server.h"


class Handle {
public:
    virtual int operator()() =0;
    virtual int operator()(void *);
    virtual int operator()(void *, int);
};



class HandleUserLogin : public Handle {
public:
    int operator()() override {};

    int operator()(void* _s, int) override 
    {
        // TODO
        //SERVER.
    }
};

class HandleUserExit : public Handle {
public:
    int operator()() override {};

    int operator()(void*, int) override
    {
        // TODO
    }
};

class HandleUpdateStatus : public Handle {
public:
    int operator()() override {};

    int operator()(void * _p, int) override
    {
        Player* p = static_cast<Player*>(_p);

        p->getPlayerOperation();

        p->Update();

        p->sendPlayerStatus();

        return 0;
    }
};


class HandleSyncClient : public Handle{    
public:
    int operator()() override {};

    int operator()(void * _s) override
    {
        Server* server = static_cast<Server*>(_s);
        
        auto playersMap = server->GetAllPlayers();
        
        char* allstatus = new char[4096]; // 保存所有玩家的状态, 较滞后，复杂度 n
        char* p = allstatus;
        
        int left = 4096;
        int n = 0;
        for (auto& iter : *playersMap) {
            n = iter.second->getPlayerStatus(p, left);
            p += n;
            left -= n;
        }

        server->SendMsgToAll(allstatus, 4096 - left);
        
        delete[] allstatus;
    }

    int operator()(void * _s, int _n) override
    {
        // ToDo 调用 每个player 的 sendPlayerStatus 复杂度 n * n
    }
};


class EventHandler {
public:
    EventHandler();
    
    ~EventHandler();

    void InitHandler();

    int AddHandle(EventType, Handle*);

    int DelHandle(EventType);
    
    Handle* Notify(EventType);

protected:
    std::unordered_map<EventType, Handle*> m_handleEvents;

};

#endif
