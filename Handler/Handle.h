#ifndef _HANDLE_H_
#define _HANDLE_H_

class Handle {
public:
    virtual int operator()(void *)  = 0;
    virtual int operator()(void *, int) { return 0;};
    
    Handle() = default;
    virtual ~Handle() {};
};



class HandleUserLogin : public Handle {
public:
    int operator()(void* _s) override;
};


class HandleUserExit : public Handle {
public:

    int operator()(void* _p) override;
};


class HandleUserAlive : public Handle {
public:
    int operator()(void* _p) override;
};

class HandleUpdateStatus : public Handle {
public:

    int operator()(void * _p) override;
};


class HandleSyncClient : public Handle {    
public:

    int operator()(void * _s) override;
};


class HandleEventItem : public Handle {
public:
    int operator()(void *_s) override;
};



#endif
