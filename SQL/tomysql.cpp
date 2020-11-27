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
}


int ToMysql::Connect()
{
    if (m_connect.connect(m_dbname.c_str(), m_server.c_str(), m_user.c_str(), m_pass.c_str())) {
        m_query = m_connect.query(); // 
        return 0;
    }
    std::cerr << "mariadb connect failed " << m_connect.error() << std::endl;
    return -1;
}

int ToMysql::DisConnect()
{

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

int ToMysql::GetByString(const std::string& _table, const std::string& _filed, mysqlpp::StoreQueryResult& _data)
{
    char cmd [50] =  {0};
    snprintf(cmd, 50, "SELECT %s FROM %s ", _filed.c_str(), _table.c_str());
    Query(cmd, _data);
    
    return 0;
}

int ToMysql::GetByString(const std::string& _table, const std::string& _filed, const std::string& _where, mysqlpp::StoreQueryResult& _data)
{
    char cmd [50] =  {0};
    snprintf(cmd, 50, "SELECT %s FROM %s where %s", _filed.c_str(), _table.c_str(), _where.c_str());
    Query(cmd, _data);

    return 0;
}

// TODO 改为设置 query 的 模板参数查询
// %###(modifier)(:name)(:)
// query << "select (%1:filed1, %2:filed2) from (%0:table) where %3:wheref = %4q:what"
// query.parse();
// select * from (%0:table) where %1:wheref=%4q:what"
int ToMysql::FormatCmd(int _argc, const char * const _argv[])
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
            return -1;
    }
    return 0;
}



