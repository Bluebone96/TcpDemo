#ifndef _TOMYSQL_H_
#define _TOMYSQL_H_
#define MYSQLPP_MYSQL_HEADERS_BURIED
#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>
#include <string>
#include <vector>


// #ifndef _DEMO_DB_TYPE_
// #define MYSQLPP_SSQLS_NO_STATICS
// #endif



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


sql_create_7 (ITEM, 2, 7,
    mysqlpp::sql_int, userid,
    mysqlpp::sql_int, itemid,
    mysqlpp::sql_char, name,
    mysqlpp::sql_int, hp,
    mysqlpp::sql_int, mp,
    mysqlpp::sql_int, atk,
    mysqlpp::sql_int, def
)


// #endif
// #include <mysql++/mysql++.h>
// #include <mysql++/ssqls.h>


// using namespace mysqlpp;


class ToMysql {
public:
    ToMysql(const std::string& db, const std::string& server, const std::string&, const std::string&);
    ~ToMysql();

    int Connect();
    int DisConnect();

    // TODO 自定义结构 提供模板方法
    int GetByString(const std::string& _table, mysqlpp::StoreQueryResult&);
    int GetByString(const std::string& _table, const std::string& _what, mysqlpp::StoreQueryResult&);
    int GetByString(const std::string& _table, const std::string& _what, const std::string& _where, mysqlpp::StoreQueryResult&);

    template<typename T>
    int SetBySQL(const T& t /*, int _argc, char **_argv */)
    {
        m_query.insert(t);
        m_query.execute();
    }

    // _argv 顺序 _tabel, _what, _where, 参考 GetByString
    template<typename T>
    int GetBySQL( T& t, int _argc, const char* const _argv[]) 
    {
        switch (_argc)
        {
        case 1:
            snprintf(m_cmd, 50, "SELECT * FROM %s", _argv[0]);
            break;
        case 2:
            snprintf(m_cmd, 50, "SELECT %s FROM %s ", _argv[1], _argv[0]);
            break;
        case 3:
            snprintf(m_cmd, 50, "SELECT %s FROM %s where %s", _argv[1], _argv[0], _argv[2]);
            break;
        default:
            // TODO tracer log
            break;
        }

        m_query << m_cmd;

        m_query.storein(t);

        return 0;
    }

    template<typename T>
    int ModBySQL(const T& t)
    {

    }

    void testgetsql(std::vector<PLAYER>& _player)
    {
        m_query << "SELECT * FROM PLAYER";
        m_query.storein(_player);
    }

    // mysqlpp::Query& operator<<(const std::string&);
    
    // mysqlpp::Query& operator>>(std::string&);
    // mysqlpp::Query& operator>>(std::vecotr<string>&);
private:
    int Query(const std::string&, mysqlpp::StoreQueryResult&);
    
    char m_cmd[50];
    std::string m_dbname;
    std::string m_server;
    std::string m_user;
    std::string m_pass;
    mysqlpp::Connection m_connect;
    mysqlpp::Query m_query;
    // mysqlpp::StoreQueryResult m_result;
    // // std::vector<mysqlpp::Row> m_row;
};

#endif

