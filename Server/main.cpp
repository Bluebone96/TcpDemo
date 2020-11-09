#include "Server.h"

int main(int argc, char** argv)
{
    int port = 0;
    if (argc == 2) {
        port = atoi(argv[1]);
    } else {
        port = 8888;
    }
    if (SERVER.Init(port) == 0) {
        SERVER.GOGOGO();
    }
}