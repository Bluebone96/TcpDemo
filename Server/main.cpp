#include "Server.h"

int main(int argc, char** argv)
{
    //int port = 0;
    int flag = -1;
    switch (argc)
    {
    case 1:
        flag = SERVER.Init(8888);
        break;
    case 2:
        flag = SERVER.Init(atoi(argv[1]));
    case 3:
        flag = SERVER.Init(atoi(argv[1]), argv[2]);
        break;
    }
    if (flag == 0) {
        SERVER.GOGOGO();
    }
}