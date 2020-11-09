#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>
#include <string.h>
#include <errno.h>


#ifdef LOGFILE


#define TRACER(format, args...) \
    do { \
        FILE* fout = fopen("trace.log", "a+"); \
        if (!fout) { \
            std::cout << "Open file failed" << std::endl; \
            break; \
        } \
        fprintf(fout, format, ##args); \
    } while (0)
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

