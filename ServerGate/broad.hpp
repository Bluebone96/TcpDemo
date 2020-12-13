#include <functional>
#include <stdint.h>
#include <map>
#include <vector>
#include <mutex>
#include <functional>
#include <thread>

#include "gate_server.h"
#include "../Net/tcp_socket.h"

class Broadcast {
public:
    Broadcast() {}
    ~Broadcast() {}

    bool runtask(uint8_t* _data, uint32_t _len);

private:
    void Foo(std::vector<client_info>::iterator& start, std::vector<client_info>::iterator& end, uint8_t*, uint32_t);
    std::thread t1;
    std::thread t2;
    std::thread t3;
    std::thread t4;

    gate_server* pgate;
};

bool Broadcast::runtask(uint8_t* _data, uint32_t _len)
{

}


void Broadcast::Foo(std::vector<client_info>::iterator& start, std::vector<client_info>::iterator& end, uint8_t *_data, uint32_t _len)
{
    for (; start != end; ++start) {
        if (tcp_socket::tcp_send(start->fd, _data, _len)) {
            pgate->m_errorfd.add(*start);
        }
    }

}