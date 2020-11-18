#include <iostream>
#include <string>
#include "PlayerInfo.pb.h"

void printbin(char* bin, int len)
{
    std::cout << "The data bin is: ";
    for (int i = 0; i < len; ++i) { 
        std::cout << (int)(bin[i]) << ' '; 
    } 
    std::cout << std::endl; 

}

int main()
{
    Proto::Unity::PlayerInfo data;
    std::string msg = "hello";
    data.set_msg(msg);

    char buf[7] = {0};
    data.SerializeToArray(buf, 7);
    printbin(buf, 7);

    data.ParseFromArray(buf, 7);
    printbin(buf, 7);

}