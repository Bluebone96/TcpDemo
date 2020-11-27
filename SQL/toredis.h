#ifndef _TO_REDIS_H_
#define _TO_REDIS_H_

#include <vector>
#include <string>
#include <sstream>
#include <hiredis/hiredis.h>



class ToRedis {
public:
    ToRedis();
    ~ToRedis();
    
    int Init(const std::string& _ip = "127.0.0.1", int port = 6379);
    
    int Connect();

    int DisConnect();

    int FreeReply();

    int Restart();

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

    int HSetByBit(const std::string& key, const std::string& field, void*, int);

    int HGetByBit(const std::string& key, const std::string& field, void**, int*);


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
