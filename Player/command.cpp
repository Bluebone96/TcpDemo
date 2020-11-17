
#include "../Common/basetype.h"


class Command {

private:
    OPERATION OP;

public:
    void SetCommand(UINT32 op)
    {
        OP.val = op;
    }

};

