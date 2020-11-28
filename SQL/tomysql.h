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


// #endif
// #include <mysql++/mysql++.h>
// #include <mysql++/ssqls.h>




// #define FORMATSTRING(_argc, format, args...) 
// do { 
//     if (_argc)

// }








// TODO 错误处理，出错会抛出异常，未处理
class ToMysql {
public:
    ToMysql();
    ~ToMysql();

    int Init(const std::string& _db, const std::string& _server, const std::string& _usr, const std::string& _pass);
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
        return 0;
    }

    // _argv 顺序 _tabel, _what, _where, 参考 GetByString
    template<typename T>
    int GetBySQL( T& t, int _argc, const char* const _argv[]) 
    {
        FormatCmd(_argc, _argv);

        m_query << m_cmd;

        m_query.storein(t);

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
        m_query << _cmd;
        m_query.storein(t);
        return 0;
    }

    template<typename T>
    int ModBySQL(const T& _old, const T& _new)
    {
        m_query.update(_old, _new);
        m_query.execute();
        return 0;
    }

    // void testgetsql(std::vector<PLAYER>& _player)
    // {
    //     m_query << "SELECT * FROM PLAYER";
    //     m_query.storein(_player);
    // }

    // mysqlpp::Query& operator<<(const std::string&);
    
    // mysqlpp::Query& operator>>(std::string&);
    // mysqlpp::Query& operator>>(std::vecotr<string>&);
private:
    int Query(const std::string&, mysqlpp::StoreQueryResult&);
    int FormatCmd(int _argc, const char * const _argv[]);
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

