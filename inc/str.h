#ifndef STRING_H
#define STRING_H

#include <stddef.h>

/** Dynamic string, compliant to da.h interface as a dynamic array of char. */
typedef struct {
    size_t capacity;
    size_t count;
    union {
        char *data;
        char *items;
    };
} String;

#define STR_FORMAT "%.*s"
#define STR_ARGS(str) (int)(str)->count, (str)->data

#define STR_REPR_FORMAT "(String) { .count = %zu, .capacity = %zu, .data = \"" STR_FORMAT "\" }"
#define STR_REPR_ARGS(str) (str)->count, (str)->capacity, STR_ARGS(str)

#endif     // STRING_H