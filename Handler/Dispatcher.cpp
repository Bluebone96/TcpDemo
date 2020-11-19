#include "Dispatcher.h"


int Dispatcher::Process(Player *_p)
{
    MsgTrans* msg = _p->m_msgTrans;
    Handle* hand;

    if (msg->RecvMsgHead() < 0) {
        TRACER("recv error maybe client offline");
        // TODO 可能析构2次
        return -1;
    }
    TAG  T;
    T.val = msg->GetTag();
    
    switch (T.tag._type)
    {
    case 0:
        if ((hand = m_eventhandler.Notify(EventType::USERLOGIN)) != nullptr) {
            if (hand->operator()(_p) < 0) {
                TRACER("handle UsrLogin failed %s:%d", __POSITION__);
                return -1;
            }
        }
        break;
    case 1:
        if ((hand = m_eventhandler.Notify(EventType::USEREXIT)) != nullptr) {
            if (hand->operator()(_p) < 0) {
                TRACER("handle USEREXIT failed %s:%d", __POSITION__);
                return -1;
            }
        }
        break;
    case 2:
        if ((hand = m_eventhandler.Notify(EventType::USERALIVE)) != nullptr) {
            if ((*hand)(_p) < 0) {
                TRACER("handle USERALIVE failed %s:%d", __POSITION__);
                return -1;
            }
        }
        break;
    case 3:
        if ((hand = m_eventhandler.Notify(EventType::USERUP)) != nullptr) {
            if ((*hand)(_p) < 0) {
                TRACER("handle USERUPDATE failed %s:%d", __POSITION__);
                return -1;
            }
        }
        break;
    case 4:
        if ((hand = m_eventhandler.Notify(EventType::SYNCCLIENT)) != nullptr) {
            if ((*hand)(_p) < 0) {
                TRACER("handle USERSYCN failed %s:%d", __POSITION__);
                return -1;
            }
        }
        break;
    case 5:
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

    return hand ?  0 : -1;

}