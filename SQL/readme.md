
// 编译 tomysql 需指定 mysqlclient.so 地址
g++ -std=c++11 -o testmysql testmysql.cpp tomysql.cpp -lmysqlpp -L/usr/lib64/mysql -lmysqlclient

// 编译 toredis
g++ -std=c++11 -o testmysql testmysql.cpp tomysql.cpp -lmysqlpp

