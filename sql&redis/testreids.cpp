#include <iostream>
#include "toredis.h"

int main()
{
    ToRedis redis;
    
    char arr[20] = "just test";
    redis.Connect();
    
    redis.SetByBit("test", arr, 20);
    char* result;
    int len;
    redis.GetByBit("test", (void**)&result, &len);

    std::cout << "reply: len is " << len << " data is " << result << std::endl;

}