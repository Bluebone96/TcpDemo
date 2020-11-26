#ifndef _TO_REDIS_H_
#define _TO_REDIS_H_

#include <vector>
#include <string>
#include <sstream>
#include <hiredis/hiredis.h>


// enum class RedisResultType {
//     REPLY_INVALID = -1,
//     REPLY_STRING,
//     REPLY_INTEGET,
//     REPLY_ARRAY,
//     REPLY_NULL
// };


// struct  RedisResult
// {
//     RedisResultType ty
//     REDIS_REP
// };

// #define TOSTIRNG(key, value, str) \
// do { \
    
// } while (0);


class ToRedis {
public:
    ToRedis(const std::string& _ip = "127.0.0.1", int port = 6379);
    ~ToRedis();
    
    bool Connect();

    bool DisConnect();

    int FreeReply();

    template<typename T>
    int SetByString(const std::string& key , T& _t)
    {
        m_ss << "SET " << key << " " << _t;
        m_ss >> m_cmd;
        
        m_reply = (redisReply)redisCommand(m_context, m_cmd.c_str());

        if (m_reply == nullptr || m_reply->type != REDIS_REPLY_STATUS) {
            // error
            Restart();
        }
        return 0;
    }

    int SetByBit(const std::string& key, void*, int);

    int GetByBit(const std::string& key, void**, int*);

    int GetByString(std::string &);

    int Restart();

private:
    std::stringstream m_ss;
    std::string m_cmd;
    redisReply* m_reply;
    redisContext* m_context;

    char* m_pcmd;

    std::string m_ip;
    int m_port;
};




#endif
