#define MYSQLPP_MYSQL_HEADERS_BURIED
#include <mysql++/mysql++.h>
#include <string>

class ToMysql {
public:
    ToMysql();
    ~ToMysql();
    


private:
    std::string m_dbname;
    std::string m_server;
    std::string m_user;
    std::string m_pass;
    mysqlpp::Connection m_connect;
    mysqlpp::Query m_query;
    mysqlpp::StoreQueryResult m_result;
    mysqlpp::Row m_row;
}
