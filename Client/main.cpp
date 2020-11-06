#include <cstdlib>
#include "Client.h"

int main(int argc, char **argvs)
{
    if (argc != 3) {
        TRACER("use: client <ip> <port>");
    }
    if (CLIENT.Init(argvs[1], atoi(argvs[2])) == 0) {
        CLIENT.GOGOGO();
    }
}