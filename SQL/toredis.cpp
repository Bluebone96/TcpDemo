#include "toredis.h"
#include <iostream>

ToRedis::ToRedis() : m_context(nullptr), m_reply(nullptr), m_pcmd(new char[40])
{
    std::cout << "toredis ctor!" << std::endl;
}

ToRedis::~ToRedis()
{
    DisConnect();
    FreeReply();
    delete [] m_pcmd;
    std::cout << "toredis dtor!" << std::endl;
}


int ToRedis::Init(const std::string& _ip, int _port)
{
    m_ip = _ip;
    m_port = _port;
    return 0;
}

int ToRedis::Restart()
{
    DisConnect();
    FreeReply();
    if (Connect()) {
        return 0;
    }
    return -1;
}

int ToRedis::Connect()
{
    m_context = redisConnect(m_ip.c_str(), m_port);
    
    if (m_context == nullptr || m_context->err) {
        // error tracer log
        std::cout << "connect faild" << std::endl;
        return -1;
    }
    // demo 用的 redis 没有设置验证
    std::cout << "connect sucess" << std::endl;
    return 0;
}

int ToRedis::DisConnect()
{
    if (m_context) {
        redisFree(m_context);
        m_context = nullptr;
    }
    return 0;
}

int ToRedis::FreeReply()
{
    if (m_reply) {
        freeReplyObject(m_reply);
        m_reply = nullptr;
    }
    return 0;
}


int ToRedis::SetByBit(const std::string& _key, void* _buf, int _len)
{
    FreeReply();
    std::cout << "start set\n";
    snprintf(m_pcmd, 40, "SET %s \%b", _key.c_str());
    std::cout << "cmd is " << m_pcmd << std::endl;
    m_reply = (redisReply*)redisCommand(m_context, m_pcmd, _buf, _len);

    if (m_reply == nullptr || m_reply->type == REDIS_REPLY_ERROR) {
        // error
        std::cout << "set faild" << std::endl;
        Restart();
        return -1;
    }
    std::cout << "set sucess" << std::endl;
    return 0;
}

int ToRedis::GetByBit(const std::string& _key, void** _buf, int* _len)
{
    FreeReply();
    snprintf(m_pcmd, 40, "GET %s", _key.c_str());

    m_reply = (redisReply*)redisCommand(m_context, m_pcmd);
    if (m_reply == nullptr || m_reply->type == REDIS_REPLY_ERROR) {
        // error
        std::cout << "get faild" << std::endl;
        Restart();
        return -1;
    }
    std::cout << "get sucess" << std::endl;
    *_buf = m_reply->str;
    *_len = m_reply->len;
    return 0;
}


int ToRedis::HSetByBit(const std::string& _key, const std::string& _filed, void* _buf, int _len)
{
    FreeReply();

    snprintf(m_pcmd, 40, "HSET %s %s", _filed.c_str(), _key.c_str());

    m_reply = (redisReply*)redisCommand(m_context, m_pcmd, _buf, _len);
    if (m_reply == nullptr || m_reply->type == REDIS_REPLY_ERROR) {
        // error
        std::cout << "set faild" << std::endl;
        Restart();
        return -1;
    }
    std::cout << "set sucess" << std::endl;
    return 0;
}

int ToRedis::HGetByBit(const std::string& _key, const std::string& _filed, void** _buf, int* _len)
{
    FreeReply();

    snprintf(m_pcmd, 40, "HSET %s %s", _filed.c_str(), _key.c_str());

    m_reply = (redisReply*)redisCommand(m_context, m_pcmd);
    if (m_reply == nullptr || m_reply->type == REDIS_REPLY_ERROR) {
        // error
        std::cout << "set faild" << std::endl;
        Restart();
        return -1;
    }

    std::cout << "get sucess" << std::endl;
    *_buf = m_reply->str;
    *_len = m_reply->len;
    return 0;
}

