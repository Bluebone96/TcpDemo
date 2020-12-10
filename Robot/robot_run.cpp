#include <iostream>
#include <unistd.h>
#include <thread>


#include "../Common/basetype.h"
#include "../Net/tcp_socket.h"
#include "../Net/message.h"
#include "../Proto/PlayerInfo.pb.h"

#define CLEAN_LOG


static unsigned int BKDRHash(const std::string& _str)
{
    unsigned int seed = 1313;
    unsigned int hash = 0;
    for (auto& c : _str) {
        hash = hash * seed + c;
    }
    return (hash & 0x7FFFFFFF);
}


inline const char* get_name(int x)
{
    static char name[20];
    snprintf(name, 30, "robot_%d", x);
    return name;
}



bool g_flag = true;

int32_t robots_fd[500];

char g_buff_null[4096];

tcp_socket* robots[500];
Proto::Unity::Authentication g_robots_auth[500];
message msgs[500];

struct robot_range
{
    int m_start;
    int m_end;
    bool m_flag;
};



static int robots_init(robot_range* _range)
{
    const char* p = nullptr;
    for (int i = _range->m_start; i < _range->m_end; ++i) {
        robots[i] = new tcp_socket;
        robots[i]->tcp_init(-1, 1024);
        p = get_name(i);
        g_robots_auth[i].set_name(p);
        g_robots_auth[i].set_password(p);
        msgs[i].m_head.m_usrID = BKDRHash(p);
    }
    

    for (int i = _range->m_start; i < _range->m_end; ++i) {
        if (robots[i]->tcp_connect("192.168.80.3", 2222) < 0) {
            exit(1);
        } 
        std::cout << "robot_" << i  << " connect login server success\n";
        msgs[i].m_head.m_type = LOGIN_REQUEST;
        msgs[i].m_head.m_errID = 0;
        msgs[i].encode_pb(g_robots_auth[i]);
        if (robots[i]->tcp_send(msgs[i].m_data, msgs[i].m_head.m_len + MSG_HEAD_SIZE) < 0) {
            std::cout << "robot_" << i << " send login request message failed\n" << std::endl;
        }
        usleep(100 * 1000);
    }

    for (int i = _range->m_start; i < _range->m_end; ++i) {
        if (robots[i]->tcp_recv(msgs[i].m_data, MSG_HEAD_SIZE) < MSG_HEAD_SIZE) {
            std::cout << "robot_" << i << " login failed, try again\n" << std::endl;
            msgs[i].m_head.m_type = LOGIN_REQUEST;
            msgs[i].m_head.m_errID = 0;
            msgs[i].encode_pb(g_robots_auth[i]);
            if (robots[i]->tcp_send(msgs[i].m_data, msgs[i].m_head.m_len + MSG_HEAD_SIZE) < 0) {
                std::cout << "robot_" << i << " send login request message failed\n" << std::endl;
            }
        }

        msgs[i].decode();
        // robots[i]->tcp_recv(msgs[i].m_pdata, msgs[i].m_head.m_len);

        if (msgs[i].m_head.m_type == GET_ALLINFO) {
            std::cout << "robot_" << i  << " login success\n";
        } else {
            std::cout << "robot_" << i << " login failed\n";
            std::cout << "recv msg type is " << msgs[i].m_head.m_type
                      << " usrid is " << msgs[i].m_head.m_usrID
                      << " errID is " << msgs[i].m_head.m_errID
                      << " len is " << msgs[i].m_head.m_len << std::endl;
        }
    }

    for (int i = _range->m_start; i < _range->m_end; ++i) {
        close(robots[i]->getfd());
        if ( (robots_fd[i] = robots[i]->tcp_connect("192.168.80.3", 3333)) < 0) {
            std::cout << "connect gate server failed" << std::endl;
            return 3;
        }

        usleep(10 * 1000);
        msgs[i].m_head.m_type = 0;
        msgs[i].m_head.m_len = 0;
        msgs[i].encode();
        robots[i]->tcp_send(msgs[i].m_data, MSG_HEAD_SIZE);
    }

    return 0;
}

static void test_sendmsg(robot_range* _range)
{
    Proto::Unity::Operation op;
    
    for (int i = _range->m_start; i < _range->m_end; ++i) {
        msgs[i].m_head.m_type = USERUP;
        msgs[i].m_head.m_errID = 0;
    }

    for (int i = 0; i < 10000; ++i) {
        for (int j = _range->m_start; j < _range->m_end; ++j) {
            op.set_h(random() % 2 - 1);
            op.set_v(random() % 2 - 1);
            msgs[j].encode_pb(op);
            if (robots[j]->tcp_send(msgs[j].m_data, msgs[j].m_head.m_len + MSG_HEAD_SIZE) < 0) {
                std::cout << "send msg  failed" << std::endl;
            }
            usleep(10 * 1000);
        }
        if (!(i & 0xff)) {
            std::cout << "send msg num is " << i << std::endl;
        }
    }
    
    _range->m_flag = false;
    std::cout << "send end\n";
}

static void test_recvmsg(robot_range* _range)
{

    while (_range->m_flag) {
        for (int i = _range->m_start; i < _range->m_end; ++i) {
            read(robots_fd[i], g_buff_null, 4096);
        }
        usleep(100 * 1000);
    }
}

int main()
{

    robot_range range_0{0, 400, true};

    robots_init(&range_0);

    std::cout << "robots init complete" << std::endl;


    // robot_range range_1{0, 50, true};
    // robot_range range_2{50, 100, true};
    // robot_range range_3{100, 150, true};
    // robot_range range_4{150, 200, true};

    // std::thread t1(test_sendmsg, &range_1);
    // std::thread t2(test_recvmsg, &range_1);

    // std::thread t3(test_sendmsg, &range_2);
    // std::thread t4(test_recvmsg, &range_2);

    // std::thread t5(test_sendmsg, &range_3);
    // std::thread t6(test_recvmsg, &range_3);

    // std::thread t7(test_sendmsg, &range_4);
    // std::thread t8(test_recvmsg, &range_4);

    // t1.join();
    // t2.join();

    // t3.join();
    // t4.join();

    // t5.join();
    // t6.join();

    // t7.join();
    // t8.join();

}



//g++ -std=c++11 -O2  -fprofile-arcs -ftest-coverage robot_run.cpp ./libnet.a ./libproto.a -lgcov -lpthread -lprotobuf -o test_robot_run
