#include <functional>
#include <stdint.h>
#include <map>
#include <vector>
#include <mutex>
#include <functional>
#include <thread>
#include <unistd.h>

#include "gate_server.h"
#include "../Net/tcp_socket.h"
#include "../Common/log.h"



struct Task {
    bool isvalied;
    uint8_t *data;
    uint32_t len;
    std::vector<client_info>::iterator start;
    std::vector<client_info>::iterator end;

    Task() : isvalied(false), data(nullptr), len(0) {}
};





// class my_thread {

// private:
//     std::thread m_t;
// };




class Broadcast {
public:
    explicit Broadcast(gate_server* _pg);
    ~Broadcast();

    bool runtask(uint8_t* _data, uint32_t _len);
#define MAX_THREAD_COUNT 30
private:
    bool iscomplete();
    void Foo(Task& _task);
    bool alive;

    std::vector<Task> m_tasks;
    std::vector<std::thread> m_threads;
    
    gate_server* pgate;
};



Broadcast::Broadcast(gate_server* _pg) : alive(true), m_tasks(0), m_threads(0), pgate(_pg)
{
    for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
        m_tasks.emplace_back(Task());
        m_threads.emplace_back(std::thread(Foo, std::ref(m_tasks[i])));
    }
}

Broadcast::~Broadcast()
{
    alive = false;
    for (auto& t : m_threads) {
        t.join();
    }
}

// todo 现在默认返回成功，添加异常捕获，考虑失败的情况
bool Broadcast::runtask(uint8_t* _data, uint32_t _len)
{
    uint32_t size = pgate->m_clientsfd.size();
    uint32_t tc = std::min((size / 0x10), 29u); // 每个线程最少负责广播16个, 最后一个线程负责剩余的 
    if (tc = 0) {
        // // erase m_clientfd.end 会变化
        // for (auto i = pgate->m_clientsfd.begin() /*, j = m_clientsfd.end()*/; i != pgate->m_clientsfd.end();) {
        //     if (tcp_socket::tcp_send(i->fd, _data, _len)) {
        //         pgate->m_usrfd.erase(i->usrid);
        //         i = pgate->m_clientsfd.erase(i);
        //     } else {
        //         ++i;
        //     }
        // }
        std::vector<uint32_t> errorfd;
        auto i = pgate->m_clientsfd.begin();
        auto j = pgate->m_clientsfd.end();

        for (; i != j; ) {
            if (tcp_socket::tcp_send(i->fd, _data, _len)) {
                *i = *(j-1);
                --j;
            } else {
                ++i;
            }
        }

        pgate->m_clientsfd.erase(j, pgate->m_clientsfd.end());
    } else {
        uint8_t scale = size / tc;
        auto start = pgate->m_clientsfd.begin();
        for (int i = 0; i < tc; ++i) {
            m_tasks[i].data = _data;
            m_tasks[i].len = _len;
            m_tasks[i].start = start + scale * i;
            m_tasks[i].end = m_tasks[i].start + scale;
            m_tasks[i]->isvalied = true;
        }
        // 剩下的
        if (size & tc) {
            m_tasks[tc].data = _data;
            m_tasks[tc].len = _len;
            m_tasks[tc].start = start + scale * tc;
            m_tasks[tc].end = pgate->m_clientsfd.end();
            m_tasks[tc]->isvalied = true;
        }

        while (!iscomplete()) {
            usleep(20 * 1000);
        }
    }
    return true;
}


void Broadcast::Foo(Task& _task)
{
    static uint8_t  t = 1;
    uint8_t id = t++;
    TRACER("hello thread! id = %d\n", id);

    while (alive) {
        if (_task.isvalied) {
            for (auto i = _task.start, j = _task.end; i != j; ++i) {
                if (tcp_socket::tcp_send(i->fd, _task.data, _task.len)) {
                    pgate->m_errorfd.add(*i);
                }
            }
            _task.isvalied = false;
        }
        usleep(50 * 1000);
    }

    TRACER("by thread! id = %d\n", id);
}