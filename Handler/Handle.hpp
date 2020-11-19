#ifndef _HANDLE_HPP_
#define _HANDLE_HPP_

#include "../Player/Player.h"
#include "../Server/Server.h"


class Handle {
public:
    virtual int operator()() =0;
    virtual int operator()(void *);
    virtual int operator()(void *, int);
    
    Handle() = default;
    virtual ~Handle() {};
};



class HandleUserLogin : public Handle {
public:
    int operator()() override {};

    int operator()(void* _s, int n) override 
    {
        // TODO
        //SERVER.
        Player* player = static_cast<Player*>(_s);

        player->InitPlayer();
        

        // TODO 考虑重新设计，MsgTrans MsgRecord TcpSocket 这 3 个类的封装关系
        MsgRecord msg;

        TAG t;

        t.tag._type = static_cast<unsigned char>(EventType::USERLOGIN);

        t.tag._id = player->getId();

        msg.SetTag(t.val);

        int len = msg.Encode(player->GetPlayerInfo());

        SERVER.SendMsgToAll(msg.GetDataAddress(), len);

        return 0;
    }
};


class HandleUserExit : public Handle {
public:
    int operator()() override {};

    int operator()(void* _p, int) override
    {
        Player* p = static_cast<Player*>(_p);


    }
};


class HandleUpdateStatus : public Handle {
public:
    int operator()() override {};

    int operator()(void * _p, int) override
    {
        Player* player = static_cast<Player*>(_p);
        
        PROTOBUF& proto = player->GetPlayerInfo();

        MsgRecord msg;

        TAG t;

        t.tag._type = static_cast<unsigned char>(EventType::USERLOGIN);

        t.tag._id = player->getId();

        msg.SetTag(t.val);

        int len = msg.Encode(player->GetPlayerInfo());

        SERVER.SendMsgToAll(msg.GetDataAddress(), len);

        return 0;
    }
};


class HandleSyncClient : public Handle {    
public:
    int operator()() override {};

    int operator()(void * _s) override
    {
        auto playersMap = static_cast<Server::PLAYERMAP *>(_s);
        
        char* allstatus = new char[4096]; // 保存所有玩家的状态, 较滞后，复杂度 n
        char* p = allstatus;
        
        int left = 4096;
        int n = 0;
        for (auto& iter : *playersMap) {
            n = iter.second->getPlayerStatus(p, left);
            p += n;
            left -= n;
        }


        Server::GetInstance().SendMsgToAll(allstatus, 4096 - left);
        
        delete[] allstatus;
    }

    int operator()(void * _s, int _n) override
    {
        // ToDo 调用 每个player 的 sendPlayerStatus 复杂度 n * n
    }
};


#endif
