#include "tomysql.h"

ToMysql::ToMysql(const std::string& _db, const std::string& _servr, const std::string& _usr, const std::string& _pass)
    : m_dbname(_db), m_server(_servr), m_user(_usr), m_pass(_pass),
      m_connect(mysqlpp::Connection(false)), m_query(mysqlpp::Query(&m_connect))
{
    
}

ToMysql::~ToMysql()
{

}

int ToMysql::Connect()
{
    if (m_connect.connect(m_dbname.c_str(), m_server.c_str(), m_user.c_str(), m_pass.c_str())) {
        return 0;
    }

    
}


int ToMysql::Query(const std::string& cmd, mysqlpp::StoreQueryResult& _data)
{
    m_query << cmd;
    _data = m_query.store();

    return 0;
}

int ToMysql::GetByString(const std::string& _tabel, mysqlpp::StoreQueryResult& _data)
{
    char cmd [50] =  {0};
    snprintf(cmd, 50, "SELECT * FROM %s ", _tabel.c_str());
    Query(cmd, _data);

    return 0;
}

int ToMysql::GetByString(const std::string& _table, const std::string& _what, mysqlpp::StoreQueryResult& _data)
{
    char cmd [50] =  {0};
    snprintf(cmd, 50, "SELECT %s FROM %s ", _what.c_str(), _table.c_str());
    Query(cmd, _data);
    
    return 0;
}

int ToMysql::GetByString(const std::string& _table, const std::string& _what, const std::string& _where, mysqlpp::StoreQueryResult& _data)
{
    char cmd [50] =  {0};
    snprintf(cmd, 50, "SELECT %s FROM %s where %s", _what.c_str(), _table.c_str(), _where.c_str());
    Query(cmd, _data);

    return 0;
}



