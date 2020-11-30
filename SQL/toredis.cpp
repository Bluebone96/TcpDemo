#include "toredis.h"
#include <iostream>
#include <cstring>

ToRedis::ToRedis() : m_reply(nullptr),  m_context(nullptr), m_pbuf(new char[512]), m_bufsize(512)
{
    std::cout << "toredis ctor!" << std::endl;
}

ToRedis::~ToRedis()
{
    FreeReply();
    DisConnect();
    delete [] m_pbuf;
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


int ToRedis::Set(const char *_key, const char *_format, ...)
{
    FreeReply();
    va_list ap;
    va_start(ap, _format);
    snprintf(m_pbuf, 512, "SET %s %s", _key, _format);
    
    m_reply = (redisReply*)redisvCommand(m_context, m_pbuf, ap);
    va_end(ap);

    if (m_reply == nullptr || m_reply->type == REDIS_REPLY_ERROR) {
        // error
        std::cerr << "set faild " << std::endl;
        Restart();
        return -1;
    }
    return 0;
}

int ToRedis::Get(const char *_key, char *_buf, int* _len)
{
    FreeReply();

    m_reply = (redisReply*)redisCommand(m_context, "GET %s", _key);
    if (m_reply == nullptr || m_reply->type == REDIS_REPLY_ERROR) {
        // error
        std::cerr << "get faild" << std::endl;
        Restart();
        return -1;
    }

    if (m_reply->type == REDIS_REPLY_STRING) {
        strncpy(_buf, m_reply->str, m_reply->len);
        *_len = m_reply->len;
    }

    return 0;
}



int ToRedis::HSetField(const char *_key, const char *_field, const char *_format, ...)
{
    FreeReply();

    va_list ap;
    va_start(ap, _format);
    snprintf(m_pbuf, m_bufsize, "HSET %s %s %s", _key, _field, _format);

    m_reply = (redisReply*)redisvCommand(m_context, m_pbuf, ap);
    va_end(ap);

    if (m_reply == nullptr || m_reply->type == REDIS_REPLY_ERROR) {
        // error
        std::cerr << "set faild" << std::endl;
        Restart();
        return -1;
    }
    std::cout << "set sucess" << std::endl;
    return 0;
}


int ToRedis::HGetField(const char *_key, const char *_field, char *_usrbuf, int* _len)
{
    FreeReply();

    m_reply = (redisReply*)redisCommand(m_context, "HGET %s %s", _key, _field);
    if (m_reply == nullptr || m_reply->type == REDIS_REPLY_ERROR) {
        // error
        std::cout << "set faild" << std::endl;
        Restart();
        return -1;
    }
    if (m_reply->type == REDIS_REPLY_STRING) {
        strncpy(_usrbuf, m_reply->str, m_reply->len);
        *_len = m_reply->len;
    }

    return 0;
}




int ToRedis::HMSET(const char *_key, const char *format, ...)
{
    FreeReply();

    va_list ap;
    va_start(ap, format);
    snprintf(m_pbuf, m_bufsize, "HMSet %s %s", _key, format);

    m_reply = (redisReply*)redisvCommand(m_context, m_pbuf, ap);
    va_end(ap);

    if (m_reply == nullptr || m_reply->type == REDIS_REPLY_ERROR) {
        FreeReply();
        return -1;
    }
    
    return 0;
}


int ToRedis::HMGET(const char* _key, std::vector<std::string>& _bufs , int* _count)
{
    FreeReply();

    std::cout << "hmget cmd is " << _key << std::endl;

    m_reply = (redisReply*)redisCommand(m_context, "HGETALL %s", _key);
    std::cout << "hmget complete\n";
    if (m_reply == nullptr || m_reply->type == REDIS_REPLY_ERROR) {
        std::cout << "hmget failed\n"; 
        FreeReply();
        return -1;
    }

    if (m_reply->type == REDIS_REPLY_ARRAY) {
        std::cout << "copy item\n";
        for (uint i = 0; i < m_reply->elements; ++i) {
            _bufs.emplace_back(std::string(m_reply->element[i]->str, m_reply->element[i]->len));
            // strncpy(_bufv[i], m_reply->element[i]->str, m_reply->element[i]->len);
        }

        *_count = m_reply->elements;
    }
    return m_reply->elements;
}


int ToRedis::Del(const char *_key)
{
    FreeReply();

    m_reply = (redisReply*)redisCommand(m_context, "DEL %s", _key);

    if (m_reply == NULL || m_reply->type == REDIS_REPLY_ERROR) {
        std::cerr << "redis del faild" << std::endl;
        Restart();
        return -1;
    }
    return 0;
}


