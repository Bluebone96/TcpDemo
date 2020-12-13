#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>
#include <string.h>
#include <errno.h>

// #define NONO_LOG

#define LOG_ERROR
#define LOG_DEBUG


#define __POSITION__ __FILE__, __LINE__

#ifndef NONO_LOG

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
    } while (0)
#endif

#define TRACERERRNO(format, args...) \
    do { \
        TRACER(format, ##args); \
        TRACER("errno = %d : %s\n", errno, strerror(errno)); \
    } while (0)
#else 
    #define TRACER(format, args...) 
    #define TRACERERRNO(format, args...)
#endif

// #define PRINTBIN(bin, len) 
//     do { 
//         for (char* i = (bin), end = (bin) + (len); i < end; ++i) { 
//             std::cout << (int)(*i); 
//         } 
//         std::cout << std::endl; 
//     } while (0)


#ifdef LOG_ERROR
    #define TRACER_ERROR(format, args...) \
        do { \
            printf(format, ##args); \
        } while (0) 
#else
    #define TRACER_ERROR(format, args...)
#endif

#ifdef LOG_DEBUG
    #define TRACER_DEBUG(format, args...) \
        do { \
            printf(format, ##args); \
        } while (0);
#else
    #define TRACER_DEBUG(format, args...)
#endif


#endif




