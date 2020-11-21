#include "Dispatcher.h"
#include "../Player/Player.h"


int Dispatcher::Process(EventType _type, void* _p)
{
    if (_p) {
        Handle* hand = nullptr;

        if ((hand = Notify(_type)) != nullptr) {
            if (hand->operator()(_p) < 0) {
                TRACER("handle UsrLogin failed %s:%d", __POSITION__);
                return -1;
            }
        }
        return hand  ?  0 : -1;
    }
    return 1;
}

int Dispatcher::Process(Player *_p)
{
    if (_p) {
        MsgTrans* msg = _p->m_msgTrans;
        if (msg->recvmsg() < 0) {
            TRACER("recv error maybe client offline!!!\n");
            // TODO 可能析构2次
            return -2;
        }
        uint32_t tag = msg->GetEventType();
        return Process(static_cast<EventType>(tag), _p);
    }

    return -1;
}