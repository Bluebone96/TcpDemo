#ifndef _GATE_SERVER_H_
#define _GATE_SERVER_H_

#include <stdint.h>
#include <map>
#include <vector>
#include <mutex>

#include "../Net/message.h"

extern msg_queue g_recv_queue;
extern msg_queue g_send_queue;

extern std::map<uint32_t, uint32_t> g_connet_server;

struct client_info {
    uint32_t usrid;
    uint32_t fd;

    client_info(uint32_t _u = 0, uint32_t _fd = 0) : usrid(_u), fd(_fd) {}

    bool operator==(const client_info& _rhs) const
    {
       return this->usrid == _rhs.usrid && this->fd == _rhs.fd;
    }
};

class gate_server
{
public:
    gate_server(/* args */);
    ~gate_server();

    int8_t init_gate();
    int8_t run();


    // 广播专用
    void broadcaster(message* _msg);
private:
    std::map<uint32_t, uint32_t> m_usrkey; // 验证令牌

    std::map<uint32_t, uint32_t> m_usrfd;   // bug  客户端断开后无法进行有效清除
    
    std::map<uint32_t, uint32_t> m_usrserver; // usr所属的game服务器

    // 广播用的，暂时顶一下
    // std::vector<client_info> m_clients;
    std::vector<client_info> m_clientsfd;

    class safevector{
    public:
        safevector() {}
        ~safevector() {}
        
        void add(uint32_t _u, uint32_t _fd) {
            std::lock_guard<std::mutex> lock(_mutex);
            _errorfd.push_back(client_info(_u, _fd));
        }
        
        void add(client_info& info) {
            std::lock_guard<std::mutex> lock(_mutex);
            _errorfd.push_back(info);
        }

        uint32_t size() { return _errorfd.size(); }

        void clear() { _errorfd.clear(); } // 或 swap

        client_info operator[](uint32_t _i) {
            client_info fd;
            try
            {
                fd = _errorfd.at(_i);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                return 0;
            }
            return fd;
        }

    private:
        std::mutex _mutex;
        std::vector<client_info> _errorfd;
    };

    safevector m_errorfd; // 无效fd, 删除掉
};


#endif
