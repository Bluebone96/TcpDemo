#include <cstdlib>
#include "Client.h"

int main(int argc, char **argvs)
{
    if (argc != 3) {
        TRACER("failed .use: client <ip> <port>\n");
        return 0;
    }
    if (CLIENT.Init(argvs[1], atoi(argvs[2])) == 0) {
        CLIENT.GOGOGO();
    }
}