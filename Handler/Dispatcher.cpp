#include "Dispatcher.h"


int Dispatcher::Process(EventType _type, void* _p)
{
    Handle* hand = nullptr;

    switch (_type)
    {
    case EventType::USERLOGIN:
        if ((hand = m_eventhandler.Notify(EventType::USERLOGIN)) != nullptr) {
            if (hand->operator()(_p) < 0) {
                TRACER("handle UsrLogin failed %s:%d", __POSITION__);
                return -1;
            }
        }
        break;
    case EventType::USEREXIT:
        if ((hand = m_eventhandler.Notify(EventType::USEREXIT)) != nullptr) {
            if (hand->operator()(_p) < 0) {
                TRACER("handle USEREXIT failed %s:%d", __POSITION__);
                return -1;
            }
        }
        break;
    case EventType::USERALIVE:
        if ((hand = m_eventhandler.Notify(EventType::USERALIVE)) != nullptr) {
            if ((*hand)(_p) < 0) {
                TRACER("handle USERALIVE failed %s:%d", __POSITION__);
                return -1;
            }
        }
        break;
    case EventType::USERUP:
        if ((hand = m_eventhandler.Notify(EventType::USERUP)) != nullptr) {
            if ((*hand)(_p) < 0) {
                TRACER("handle USERUPDATE failed %s:%d", __POSITION__);
                return -1;
            }
        }
        break;
    case EventType::SYNCCLIENT:
        if ((hand = m_eventhandler.Notify(EventType::SYNCCLIENT)) != nullptr) {
            if ((*hand)(&SERVER.m_players) < 0) {
                TRACER("handle USERSYCN failed %s:%d", __POSITION__);
                return -1;
            }
        }
        break;
    case EventType::USERCHAT:
        if ((hand = m_eventhandler.Notify(EventType::USERCHAT)) != nullptr) {
            if ((*hand)(_p) < 0) {
                TRACER("handle USERCHAT failed %s:%d", __POSITION__);
                return -1;
            }
        }
        break;

    default:
        // throw; TODO
        break;
    }

    return hand  ?  0 : -1;
}

int Dispatcher::Process(Player *_p)
{
    MsgTrans* msg = _p->m_msgTrans;
    if (msg->RecvMsgHead() < 0) {
        TRACER("recv error maybe client offline");
        // TODO 可能析构2次
        return -1;
    }
    TAG  T;
    T.val = msg->GetTag();
    
    return Process(static_cast<EventType>(T.tag._type), (void*)_p);

}