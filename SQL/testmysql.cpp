#include <iostream>
#define MYSQLPP_SSQLS_NO_STATICS

// #define _DEMO_DB_TYPE_
#include "tomysql.h"
// #undef  _DEMO_DB_TYPE_

#include <vector>

int main()
{
    mysqlpp::StoreQueryResult result;
    ToMysql sql;

    sql.Init("ProjectA", "127.0.0.1:3306", "Blue", "1024");
    
    sql.Connect();

    PLAYER dog(33, "dog", 100, 50, 99, 0, 0, 0, 0);

    // sql.SetBySQL(dog);

    sql.GetByString("PLAYER", result);

    std::vector<PLAYER> players;

    // sql.testgetsql(players);
    // char* cmd[] = {"PLAYER", "*", "id = 33" };

    // sql.GetBySQL(players, 3, cmd);
    // sql.GetByString("role", "*", "id = 33", result);

    for (auto& r : result) {
        std::cout << r["id"] << " " << r["name"] << " " << std::endl;
    }

    for (auto& p : players) {
        std::cout << p.id << " " 
                  << p.name << " "
                  << p.hp << " "
                  << p.mp << " "
                  << p.atk << " "
                  << p.def << " "
                  << p.speed << " "
                  << p.posx << " "
                  << p.posz << " "
                  << std::endl;
    }

}