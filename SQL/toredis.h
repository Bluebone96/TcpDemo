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
        FreeReply();

        m_ss << "SET " << key << " " << _t;
        m_ss >> m_cmd;
        
        m_reply = (redisReply)redisCommand(m_context, m_cmd.c_str());

        if (m_reply == nullptr || m_reply->type != REDIS_REPLY_STATUS) {
            // error
            Restart();
        }
        return 0;
    }


    int Set(const char *_key, const char *format, ...);

    int Get(const char *_key, char *_buf, int* _len);



    int HSetField(const char *_key, const char *_field, const char *_format, ...);

    int HGetField(const char *key, const char *_field, char *_usrbuf, int *_len);


    int HMSET(const char *_key, const char *_format, ...);

    // 传递二维数组, 获取整个hash表
    int HMGET(const char* _key, char **_bufv, int *_count);

    int Del(const char *_key);

private:

    std::stringstream m_ss;
    std::string m_cmd;

    redisReply* m_reply;
    redisContext* m_context;

    std::string m_ip;
    int m_port;

    char *m_pbuf;
    int m_bufsize;
};




#endif
