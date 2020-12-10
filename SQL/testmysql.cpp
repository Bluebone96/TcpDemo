// #define MYSQLPP_SSQLS_NO_STATICS
// #include "../SQL/tomysql.h"

// #include <vector>
// #include <iostream>

// int main()
// {
//     std::cout << "test\n"; 
//     ToMysql sql;
//     sql.Init("ProjectA", "127.0.0.1:3306", "blue", "1024");

//     // std::vector<ITEM> item_sql;

//     // sql.GetBySQL(item_sql, "SELECT * FROM ITEM where userid = 172542746");
//     // std::cout << "itemsql.size = " << item_sql.size() << std::endl;
//     // for (auto& x : item_sql) {
//     //     std::cout << "itemid: " << x.itemid << std::endl
//     //               << "count " << x.count << std::endl;
//     // }

//     // PLAYER player {170800408, "cat", 100, 100, 100, 50, 10, 10, 20};

//     // PLAYER cat;
//     // sql.SetBySQL(player);
//     char cmd[100];
//     snprintf(cmd, 100, "update PLAYER SET posx = 5 where id = 170800408");
//     sql.ModByStr(cmd);

//     std::cout << "test end\n";

// }

// g++ -std=c++11 -o test testmysql.cpp tomysql.cpp -Wl,-R/usr/local/lib -lmysqlpp -lmysqlclient