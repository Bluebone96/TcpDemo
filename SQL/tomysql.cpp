#include "tomysql.h"

ToMysql::ToMysql() : m_connect(mysqlpp::Connection(false)), m_query(m_connect.query())
{
    
}

ToMysql::~ToMysql()
{

}

int ToMysql::Init(const std::string& _db, const std::string& _server, const std::string& _usr, const std::string& _pass)
{
    m_dbname = _db;
    m_server = _server;
    m_user = _usr;
    m_pass = _pass;

    return Connect();
}


int ToMysql::Connect()
{
    if (m_connect.connect(m_dbname.c_str(), m_server.c_str(), m_user.c_str(), m_pass.c_str()) == true) {
        m_query = m_connect.query();
        return 0;
    }
    std::cerr << "mariadb connect failed " << m_connect.error() << std::endl;
    return -1;
}

int ToMysql::DisConnect()
{
    // TODO
    return 0;
}
