#include <iostream>
#include <unistd.h>
#include <thread>
#include <sys/wait.h>
#include <sys/types.h>


#include "../Common/basetype.h"
#include "../Net/tcp_socket.h"
#include "../Net/message.h"
#include "../Proto/PlayerInfo.pb.h"



std::vector<std::thread> g_thread;

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

robot_range g_range;


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


static int robots_init()
{
    const char* p = nullptr;
    for (int i = g_range.m_start; i < g_range.m_end; ++i) {
        robots[i] = new tcp_socket;
        robots[i]->tcp_init_buf();
        p = get_name(i);
        g_robots_auth[i].set_name(p);
        g_robots_auth[i].set_password(p);
        msgs[i].m_head.m_usrID = BKDRHash(p);
    }
    

    for (int i = g_range.m_start; i < g_range.m_end; ++i) {
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

    sleep(3);

    for (int i = g_range.m_start; i < g_range.m_end; ++i) {
        while (robots[i]->tcp_recv(msgs[i].m_data, MSG_HEAD_SIZE) < MSG_HEAD_SIZE) {
               std::cout << "\nrobot_" << i << " login failed, try again" << std::endl;
                usleep(10 * 1000);
        }

        msgs[i].decode();

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

    for (int i = g_range.m_start; i < g_range.m_end; ++i) {
        close(robots[i]->getfd());

        if ( (robots_fd[i] = robots[i]->tcp_connect("192.168.80.3", 4445)) < 0) {
            std::cout << "connect gate server failed" << std::endl;
            return 3;
        }

        usleep(50 * 1000);
        msgs[i].m_head.m_type = 0;
        msgs[i].m_head.m_len = 0;
        msgs[i].encode();
        robots[i]->tcp_send(msgs[i].m_data, MSG_HEAD_SIZE);
    }

    return 0;
}

static void test_sendmsg()
{
    Proto::Unity::Operation op;
    
    for (int i = g_range.m_start; i < g_range.m_end; ++i) {
        msgs[i].m_head.m_type = USERUP;
        msgs[i].m_head.m_errID = 0;
    }
    while (g_range.m_flag) {
        for (int i = 0;( i < 10000) && g_range.m_flag; ++i) {
            for (int j = g_range.m_start; (j < g_range.m_end) && g_range.m_flag; ++j) {
                op.set_h(random() % 3 - 1);
                op.set_v(random() % 3 - 1);
                msgs[j].encode_pb(op);
                if (robots[j]->tcp_send(msgs[j].m_data, msgs[j].m_head.m_len + MSG_HEAD_SIZE) < 0) {
                    std::cout << "send msg  failed" << std::endl;
                }
                usleep(50 * 1000);
            }
            if (!(i & 0x7f)) {
                printf("\n==================== %d message send ===============\n", i);
            }
        }
    }
    g_range.m_flag = false;
}

static void test_recvmsg()
{

    while (g_range.m_flag) {
        for (int i = g_range.m_start; i < g_range.m_end && g_range.m_flag; ++i) {
            read(robots_fd[i], g_buff_null, 4096);
        }
        usleep(50 * 1000);
    }
}


static void exit_all_robots(int signo)
{   
    std::cout << "\n===============start exit all robots=========================" << std::endl;
    g_range.m_flag = false;

    if (g_thread[0].joinable()) {
        g_thread[0].join();
        std::cout << "send thread joined" << std::endl;
    } 
    if (g_thread[1].joinable()) 
    {
        g_thread[1].join();
        std::cout << "recv thread joined" << std::endl;
    }


    std::cout << "\n===============send exit message=========================" << std::endl;
    for (int i = g_range.m_start, j = g_range.m_end; i < j; ++i) {
        msgs[i].m_head.m_type = USEREXIT;
        msgs[i].m_head.m_len = 0;
        msgs[i].encode();
        robots[i]->tcp_send(msgs[i].m_data, MSG_HEAD_SIZE);
        usleep(50 * 1000);
    }

    std::cout << "\n=============== free memory =========================" << std::endl;

    for (int i = g_range.m_start, j= g_range.m_end; i < j; ++i) {
        delete robots[i];
        usleep(50 * 1000);
    }

    std::cout << "\n===============all robots exit success=======================" << std::endl;

    exit(0);
}



int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cout << "./robot_run range[start, end)" << std::endl;
        exit(1);
    }
    
    g_range.m_start = atoi(argv[1]);
    g_range.m_end  = atoi(argv[2]);
    g_range.m_flag = true;
    printf("\n=========================test range[%d, %d), %d robots run start!=======================\n",  g_range.m_start, g_range.m_end, g_range.m_end - g_range.m_start);

    signal(SIGINT, exit_all_robots);


    robots_init();

    std::cout << "\n=============robots init complete======================" << std::endl;

    g_thread.emplace_back(std::thread(test_sendmsg));
    g_thread.emplace_back(std::thread(test_recvmsg));



    if (g_thread[0].joinable()) {
        g_thread[0].join();
    } 
    if (g_thread[1].joinable()) 
    {
        g_thread[1].join();
    }

    std::cout << "\n=============all message send======================" << std::endl;
}



//g++ -std=c++11 -O2  -fprofile-arcs -ftest-coverage robot_run.cpp ./libnet.a ./libproto.a -lgcov -lpthread -lprotobuf -o robot_run
//g++ -std=c++11 -O2  robot_run.cpp ./libnet.a ./libproto.a -lpthread -lprotobuf -o robot_run
