#ifndef _TOMYSQL_H_
#define _TOMYSQL_H_
#define MYSQLPP_MYSQL_HEADERS_BURIED
#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>
#include <string>
#include <vector>


// #ifndef _DEMO_DB_TYPE_
// #endif

sql_create_2 (PASS, 1, 2,
    mysqlpp::sql_int, id,
    mysqlpp::sql_int, pass
)

sql_create_9 (PLAYER, 2, 9,
    mysqlpp::sql_int, id,
    mysqlpp::sql_char, name,
    mysqlpp::sql_int, hp,
    mysqlpp::sql_int, mp,
    mysqlpp::sql_int, atk,
    mysqlpp::sql_int, def,
    mysqlpp::sql_int, speed,
    mysqlpp::sql_float8, posx,
    mysqlpp::sql_float8, posz
)


sql_create_10 (ITEM, 2, 10,
    mysqlpp::sql_int, usrid,
    mysqlpp::sql_int, itemid,
    mysqlpp::sql_int, type,
    mysqlpp::sql_int, count,
    mysqlpp::sql_char, name,
    mysqlpp::sql_int, hp,
    mysqlpp::sql_int, mp,
    mysqlpp::sql_int, atk,
    mysqlpp::sql_int, def,
    mysqlpp::sql_int, price
)





// TODO 错误处理，出错会抛出异常，未处理
class ToMysql {
public:
    ToMysql();
    ~ToMysql();

    int Init(const std::string& _db, const std::string& _server, const std::string& _usr, const std::string& _pass);
    int Connect();
    int DisConnect();


    template<typename T>
    bool SetBySQL(const T& t)
    {
        m_query.insert(t);
        return m_query.exec();
    }

    template<typename T>
    bool SetBySQLList(const T& t)
    {
        m_query.insert(t.begin(), t.end());
        return m_query.exec();
    }

    int ExecByString(const char* cmd)
    {
        return m_query.exec(cmd);
    }

    
    template<typename T>
    void GetBySQL(T& t, const char* _cmd)
    {   
        // std::cout << "try get by sql cmd is " << _cmd << std::endl;
        
        m_query << _cmd;
        m_query.storein(t);
    }

    template<typename T>
    bool ModBySQL(const T& _old, const T& _new)
    {
        std::cout << "update sql\n";
        m_query.update(_old, _new);
        
        return m_query.exec();
    }
    


private:

    char m_cmd[50];
    std::string m_dbname;
    std::string m_server;
    std::string m_user;
    std::string m_pass;
    mysqlpp::Connection m_connect;
    mysqlpp::Query m_query;
};

#endif

