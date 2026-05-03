#ifndef _DA_H_
#define _DA_H_

#include <stddef.h>

/* =============================== DYNAMIC ARRAY =============================== */
/**
 * Assuming this structure:
 * struct {
 *      size_t capacity;
 *      size_t count;
 *      item_type **items;
 * };
 */

#define da_reallocarray reallocarray
#define DA_INITIAL_CAPACITY 2

#define da_reserve(da, cap)                                                               \
    do {                                                                                  \
        if (cap <= (da)->capacity)                                                        \
            break;                                                                        \
        if ((da)->capacity == 0)                                                          \
            (da)->capacity = DA_INITIAL_CAPACITY;                                         \
        while (cap > (da)->capacity)                                                      \
            (da)->capacity *= 2;                                                          \
        (da)->items = da_reallocarray((da)->items, (da)->capacity, sizeof(*(da)->items)); \
    } while (0)

#define da_append(da, item)                  \
    do {                                     \
        da_reserve(da, (da)->count + 1);     \
        (da)->count += 1;                    \
        (da)->items[(da)->count - 1] = item; \
    } while (0)

#define da_init(da)         \
    do {                    \
        (da)->capacity = 0; \
        (da)->count = 0;    \
        (da)->items = 0;    \
    } while (0)

#define da_free(da)                  \
    do {                             \
        if (!(da) || !((da)->items)) \
            break;                   \
        free((da)->items);           \
        (da)->items = NULL;          \
        (da)->capacity = 0;          \
        (da)->count = 0;             \
    } while (0)

#define da_foreach(da, fun)                    \
    for (size_t i = 0; i < (da)->count; i++) { \
        fun((da)->items[i]);                   \
    }

#define da_empty(da) (da)->count == 0

#endif     // _DA_H_