#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_ERROR,
    LOG_KIND_COUNT
} Log_Kind;

/** ignores all logs of lower level */
extern Log_Kind CURRENT_LOG_KIND;
extern const char *log_kind_name[];

/** returns default_kind if name was not found or name is NULL */
Log_Kind log_kind_from_name(const char *name, Log_Kind default_kind);

FILE *get_log_stream(Log_Kind kind);

void *xmalloc(size_t size);
char *fread_all(const char *pathname);

#define log(kind, fmt, ...)                            \
    do {                                               \
        if ((kind) < CURRENT_LOG_KIND)                 \
            break;                                     \
        void *stream = get_log_stream(kind);           \
        fprintf(stream, "[%s] ", log_kind_name[kind]); \
        fprintf(stream, fmt, ##__VA_ARGS__);           \
    } while (0)

#define error(fmt, ...)                                                   \
    do {                                                                  \
        log(LOG_ERROR, "%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
        exit(1);                                                          \
    } while (0)

#define p_error(fmt, ...)                                                 \
    do {                                                                  \
        log(LOG_ERROR, "%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
        perror("Error");                                                  \
        exit(1);                                                          \
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

#endif     // _UTILS_H_