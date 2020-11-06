#include "Server.h"

int main()
{
    if (SERVER.Init() == 0) {
        SERVER.GOGOGO();
    }
}