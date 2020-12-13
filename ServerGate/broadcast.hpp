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

    Task() : isvalied(false), data(nullptr), len(0) 
    {
        TRACER_DEBUG("task ctor! isvalied = %d, data = %p\n", isvalied, (void*)data);
    }
};





// class my_thread {

// private:
//     std::thread m_t;
// };




class Broadcast {
public:
    explicit Broadcast();
    ~Broadcast();

    int8_t init(gate_server* _pg);
    bool runtask(uint8_t* _data, uint32_t _len);


#define MAX_THREAD_COUNT 10
private:
    bool iscomplete();
    void Foo(Task& _task);
    bool alive;

    std::vector<Task> m_tasks;
    std::vector<std::thread> m_threads;
    
    gate_server* pgate;
};



Broadcast::Broadcast() : alive(true), pgate(nullptr)
{

}

Broadcast::~Broadcast()
{
    alive = false;
    for (auto& t : m_threads) {
        t.join();
    }
}

int8_t Broadcast::init(gate_server*)
{
    TRACER("bBroadcast ctor start!\n");
    auto func = [this](Task& _task) {
        static uint8_t  t = 0;
        uint8_t id = t++;
        TRACER("hello thread! id = %d\n", id);
        TRACER_DEBUG("thread id = %d, task isvalied = %d, data = %p\n", id, _task.isvalied, (void*)_task.data);
        while (alive) {
            if (_task.isvalied) {
                for (auto i = _task.start, j = _task.end; i != j; ++i) {
                    if (tcp_socket::tcp_send(i->fd, _task.data, _task.len)) {
                        pgate->m_errorfd.add(*i);
                    }
                }
                _task.isvalied = false;
                TRACER_DEBUG("==================broadcast func debug=========================\n");
            }
            sleep(5);
        }
        TRACER("by thread! id = %d\n", id);
    };
    try {
        for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
            m_tasks.emplace_back(Task());
            std::cout << "init task i = " << i << ", isvalied = " << m_tasks[i].isvalied << std::endl;
            // m_threads.emplace_back(std::thread(&Broadcast::Foo, this, std::ref(m_tasks[i])));
            // m_threads.emplace_back(std::thread(func, std::ref(m_tasks[i])));
        }
        for (auto& x : m_tasks) {
            m_threads.emplace_back(std::thread(func, std::ref(x)));
        }
    } catch (std::exception& e) {
        TRACER_ERROR("broadcast init failed what: %s\n", e.what());
        return -1;
    }
    TRACER("bBroadcast ctor end!\n");

    return 0;
}


bool Broadcast::iscomplete()
{
    for (auto& t : m_tasks) {
        if (t.isvalied) {
            return false;
        }
    }
    return true;
}

// todo 现在默认返回成功，添加异常捕获，考虑失败的情况
bool Broadcast::runtask(uint8_t* _data, uint32_t _len)
{
    TRACER_DEBUG("Broadcast::runtask start, data len = %d\n", _len);
    static uint32_t count = 0;
    if (!(count & 0xff)) {
        TRACER("broadcast::runtask start! num = %d\n", count);
    }

    // uint32_t size = pgate->m_clientsfd.size();
    // uint32_t tc = std::min((size / 0x10), (MAX_THREAD_COUNT - 1u)); // 每个线程最少负责广播16个, 最后一个线程负责剩余的 
    // if (tc == 0) {
    //     TRACER_DEBUG("tc == 0 signal thread start\n");
    //     std::vector<uint32_t> errorfd;
    //     auto i = pgate->m_clientsfd.begin();
    //     auto j = pgate->m_clientsfd.end();

    //     for (; i != j; ) {
    //         if (tcp_socket::tcp_send(i->fd, _data, _len)) {
    //             *i = *(j-1);
    //             --j;
    //         } else {
    //             ++i;
    //         }
    //     }

    //     pgate->m_clientsfd.erase(j, pgate->m_clientsfd.end());

    //     TRACER_DEBUG("tc == 0 signal thread end\n");
    // } else {
    //     TRACER_DEBUG("tc == %d multi-thread start\n", tc);
    //     uint8_t scale = size / tc;
    //     auto start = pgate->m_clientsfd.begin();
    //     for (uint i = 0; i < tc; ++i) {
    //         m_tasks[i].data = _data;
    //         m_tasks[i].len = _len;
    //         m_tasks[i].start = start + scale * i;
    //         m_tasks[i].end = m_tasks[i].start + scale;
    //         m_tasks[i].isvalied = true;
    //     }
    //     // 剩下的
    //     if (size & tc) {
    //         m_tasks[tc].data = _data;
    //         m_tasks[tc].len = _len;
    //         m_tasks[tc].start = start + scale * tc;
    //         m_tasks[tc].end = pgate->m_clientsfd.end();
    //         m_tasks[tc].isvalied = true;
    //     }

    //     while (!iscomplete()) {
    //         usleep(20 * 1000);
    //     }
    //     TRACER_DEBUG("tc == %d multi-thread end\n", tc);
    // }
    if (!(count & 0xff)) {
        TRACER("broadcast::runtask start! num = %d\n", count);
    }
    ++count;

    TRACER_DEBUG("Broadcast::runtask start, data len = %d\n", _len);
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