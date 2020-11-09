#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>


#ifdef LOGFILE
    #define LOGMAXLEN 100
    static char logbuf[LOGMAXLEN] = {0};

    void WriteLog()
    {
        const char* filename = "trace.log";
        std::ofstream fout;
        fout.open("trace.log");
        if (!fout) {
            std::cout << "Open file failed" << std::endl;
            return;
        }
        fout << std::endl;
        fout.close();
        return;
    }

    #define TRACER(format, args...) \
    do { \
        bzero(logbuf, 0, LOGMAXLEN); \
        (void)snprintf(strlog, LOGMAXLEN, format, ##args); \
        WriteLog(); \
    } while(0)
#else
    #define TRACER(format, args...) \
    do { \
        printf(format, ##args); \
       /* std::cout << std::endl; */ \
    } while (0)
#endif

#define TRACERERRNO(format, args...) \
    do { \
        TRACER(format, ##args); \
        TRACER("errno = %d : %s\n", errno, strerror(errno)); \
    } while (0)

#endif

