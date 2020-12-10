#define MYSQLPP_SSQLS_NO_STATICS
#include "../SQL/tomysql.h"

#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <unistd.h>

// #define ROBOT_NAME(x) ("robot_"#x)

static unsigned int BKDRHash(const std::string& _str)
{
    unsigned int seed = 1313;
    unsigned int hash = 0;
    for (auto& c : _str) {
        hash = hash * seed + c;
    }
    return (hash & 0x7FFFFFFF);
}


inline const char* get_name(int x)
{
    static char name[20];
    snprintf(name, 30, "robot_%d", x);
    return name;
}





// static void init_robot(PLAYER);



int main()
{


    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<int32_t> rand_att(50, 100);
    std::uniform_int_distribution<int32_t> rand_speed(5, 20);
    std::uniform_int_distribution<int32_t> rand_pos(-20, 20);
    
    uint32_t hkey;
    std::vector<PLAYER> robots(500);
    std::vector<PASS> pass(500);

    ToMysql tosql;
    tosql.Init("ProjectA", "127.0.0.1:3306", "blue", "1024");
    std::cout << "init start" << std::endl;
    for (int i = 0; i < 500; ++i) {
        robots[i].hp = rand_att(eng);
        robots[i].mp = rand_att(eng);
        robots[i].atk = rand_att(eng);
        robots[i].def = rand_att(eng);
        robots[i].speed = rand_speed(eng);
        
        // std::cout << "test rand_speed" << std::endl;
        
        robots[i].posx = rand_pos(eng);
        robots[i].posz = rand_pos(eng);
        // std::cout << "test rand_pos" << std::endl;
        
        robots[i].name = get_name(i);
        hkey = BKDRHash(robots[i].name);
        // std::cout << "test hash" << std::endl;

        robots[i].id = hkey;
        pass[i].id = hkey;
        pass[i].pass = hkey;
       
        if (!(i & 0xf)) {
            std::cout << robots[i].name << '\t';
        }
    }

    // if (tosql.SetBySQLList(robots) < 0) {
    //     return 1;
    // }
    for (int i = 0; i < 500; ++i) {
        tosql.SetBySQL(robots[i]);
        // usleep(10 * 1000);
        // tosql.SetBySQL(pass[i]);
    }

}