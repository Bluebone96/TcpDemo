#include <iostream>
#include <string>
#include "Message.pb.h"

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
    Proto::Unity::Data data;
    std::string msg = "hello";
    data.set_data(msg);

    char buf[7] = {0};
    data.SerializeToArray(buf, 7);
    printbin(buf, 7);

    data.ParseFromArray(buf, 7);
    printbin(buf, 7);

}