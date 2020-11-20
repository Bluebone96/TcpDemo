#ifndef _HANDLE_HPP_
#define _HANDLE_HPP_

#include "../Player/Player.h"
#include "../Server/Server.h"


class Handle {
public:
    virtual int operator()() =0;
    virtual int operator()(void *) { return 0; };
    virtual int operator()(void *, int) { return 0;};
    
    Handle() = default;
    virtual ~Handle() {};
};



class HandleUserLogin : public Handle {
public:
    int operator()() override { return 0; };

    int operator()(void* _s) override 
    {

        TRACER("HandleUserLogin\n");
        // TODO
        //SERVER.
        Player* player = static_cast<Player*>(_s);

        (void) player->InitPlayer();
        

        // TODO 考虑重新设计，MsgTrans MsgRecord TcpSocket 这 3 个类的封装关系
        MsgRecord msg;

        TAG t;

        t.tag._type = static_cast<unsigned char>(EventType::USERLOGIN);

        t.tag._id = player->getId();

        msg.SetTag(t.val);

        int len = msg.Encode(player->GetPlayerInfo());

        SERVER.SendMsgToOne(player->getId(), msg.GetDataAddress(), len);

        t.tag._type = static_cast<unsigned char>(EventType::USERUP);

        msg.SetTag(t.val);
        msg.Encode();

        SERVER.SendMsgToAll(msg.GetDataAddress(), len);

        return 0;
    }
};


class HandleUserExit : public Handle {
public:
    int operator()() override { return 0; };

    // int operator()(void* _p, int) override
    // {
    //     Player* p = static_cast<Player*>(_p);
        
    //     // TODO
        
    //     return 0;
    // }
};


class HandleUpdateStatus : public Handle {
public:
    int operator()() override { return 0; };

    int operator()(void * _p, int) override
    {
        TRACER("HandleUpdateStatus\n")s;

        Player* player = static_cast<Player*>(_p);
        
        MsgRecord msg;

        TAG t;

        t.tag._type = static_cast<unsigned char>(EventType::USERUP);

        t.tag._id = player->getId();

        msg.SetTag(t.val);

        int len = msg.Encode(player->GetPlayerInfo());

        SERVER.SendMsgToAll(msg.GetDataAddress(), len);

        return 0;
    }
};


class HandleSyncClient : public Handle {    
public:
    int operator()() override { return 0; };

    int operator()(void * _s) override
    {

        TRACER("HandleSyncClient\n");

        auto playersMap = static_cast<Server::PLAYERMAP *>(_s);
        
        // char* allstatus = new char[4096]; // 保存所有玩家的状态, 较滞后，复杂度 n
        // char* p = allstatus;
        
        // int left = 4096;
        // int n = 0;
        MsgRecord msg;
        TAG t;
        t.tag._type = static_cast<unsigned char>(EventType::SYNCCLIENT);
        
        for (auto& iter : *playersMap) {

            t.tag._id = iter.second->getId();

            msg.SetTag(t.val);

            int len = msg.Encode(iter.second->GetPlayerInfo());

            SERVER.SendMsgToAll(msg.GetDataAddress(), len);
        }


        // SERVER.SendMsgToAll(allstatus, 4096 - left);
        
        // delete[] allstatus;

        return 0;

    }

    int operator()(void * _s, int _n) override
    {
        // ToDo 调用 每个player 的 sendPlayerStatus 复杂度 n * n
        return 0;
    }
};


#endif
