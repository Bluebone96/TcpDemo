#ifndef _EVENTHANDLER_H_
#define _EVENTHANDLER_H_

#include <unordered_map>

#include "Handle.h"
#include "../Common/basetype.h"


// enum MsgType {
//     USERLOGIN = 0,
//     USEREXIT = 1,
//     USERALIVE = 2,
//     USERUP = 3,
//     USERSYNC = 4,
//     USERCHAT = 5,

//     ITEMEVENT = 6    // 先用一个顶着
// };

class EventHandler {
public:
    EventHandler();
    
    ~EventHandler();

    int InitHandler();

    int AddHandle(MsgType, Handle*);

    int DelHandle(MsgType);
    
    Handle* Notify(MsgType);

protected:
    std::unordered_map<MsgType, Handle*> m_handleEvents;

};

#endif
