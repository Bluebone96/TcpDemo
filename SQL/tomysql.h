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


sql_create_9 (ITEM, 2, 9,
    mysqlpp::sql_int, userid,
    mysqlpp::sql_int, itemid,
    mysqlpp::sql_int, type,
    mysqlpp::sql_int, count,
    mysqlpp::sql_char, name,
    mysqlpp::sql_int, hp,
    mysqlpp::sql_int, mp,
    mysqlpp::sql_int, atk,
    mysqlpp::sql_int, def
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
    int SetBySQL(const T& t)
    {
        m_query.insert(t);
        m_query.execute();
        return 0;
    }

    // template<typename T>
    // int SetBySQLList(const T& t)
    // {
    //     try {
    //         m_query.insert(t.begin(), t.end()).execute();
    //     } catch (...) {
    //         std::cout << "sql insert error" << std::endl;
    //         return -1;
    //     }
    //     std::cout << "set_by_sql_list\n";
    //     return 0;
    // }

    int SetByString(const char* cmd)
    {
        m_query << cmd;
        m_query.execute();
        return 0;
    }



    // select * from _where = what 
    template<typename T>
    int GetBySQL(T& t, const char* _table, const char* _where, const char* _what)
    {
        // m_query << "select * from %0:table where %1:wheref = %2q:what";

        // m_query.parse();

        // t = m_query.store(_table, _where, _what);
        snprintf(m_cmd, 50, "SELECT * FROM %s where %s = %s", _table, _where, _what);
        m_query << m_cmd;
        m_query.storein(t);
        return 0;
    }


    
    template<typename T>
    int GetBySQL(T& t, const char* _cmd)
    {   
        //std::cout << "try get by sql cmd is " << _cmd << std::endl;
        
        m_query << _cmd;
        m_query.storein(t);
        return 0;
    }

    template<typename T>
    int ModBySQL(const T& _old, const T& _new)
    {
        std::cout << "update sql\n";
        m_query.update(_old, _new);
        m_query.execute();
        return 0;
    }
    
    int ModByStr(const std::string& _str)
    {

        std::cout << "try modbystr\n";
        m_query << _str.c_str();
        m_query.execute();
        return 0;
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

