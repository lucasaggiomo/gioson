#ifndef _LOG_H_
#define _LOG_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define LIST_OF_LOGS            \
    X(DEBUG, debug, stdout)     \
    X(INFO, info, stdout)       \
    X(WARNING, warning, stderr) \
    X(ERROR, error, stderr)

#define LOG_KIND(NAME) LOG_##NAME
#define LOG_KIND_STR(NAME) #NAME

typedef enum {
#define X(NAME, name, stream) LOG_KIND(NAME),
    LIST_OF_LOGS
#undef X
        LOG_KIND_COUNT
} Log_Kind;

extern const char *log_kind_name[LOG_KIND_COUNT];
Log_Kind log_kind_from_name(const char *name, Log_Kind def);

/** ignores all logs of lower level */
extern Log_Kind CURRENT_LOG_KIND;

#define X(NAME, name, stream)                             \
    static inline void log_##name(const char *fmt, ...) { \
        if (LOG_KIND(NAME) < CURRENT_LOG_KIND)            \
            return;                                       \
        fprintf(stream, "[" #name "] ");                  \
                                                          \
        va_list args;                                     \
        va_start(args, fmt);                              \
        vfprintf(stream, fmt, args);                      \
        va_end(args);                                     \
    }
LIST_OF_LOGS
#undef X

#define error(fmt, ...)                                              \
    do {                                                             \
        log_error("%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
        exit(1);                                                     \
    } while (0)

#define p_error(fmt, ...)                                            \
    do {                                                             \
        log_error("%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
        perror("Error");                                             \
        exit(1);                                                     \
    } while (0)

#define require(expr, fmt, ...)    \
    do {                           \
        if ((expr))                \
            break;                 \
        error(fmt, ##__VA_ARGS__); \
    } while (0)

#define p_require(expr, fmt, ...)    \
    do {                             \
        if ((expr))                  \
            break;                   \
        p_error(fmt, ##__VA_ARGS__); \
    } while (0)

#define unsupported(fmt, ...) \
    error(fmt, ##__VA_ARGS__)

#endif     // _LOG_H_