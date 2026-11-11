#ifndef DA_H
#define DA_H

/* =============================== DYNAMIC ARRAY =============================== */
/* Heavily inspired by tsoding (see https://github.com/tsoding/nob.h) */

/**
 * Assuming this structure:
 * struct {
 *      size_t count;
 *      size_t capacity;
 *      item_type *items;
 * };
 *
 * Actually works as 'duck typing': works with every struct with the aforementione fields.
 */

/* ------------------------------- CONFIGURABLE STUFF ------------------------------- */

#include <stddef.h>
#ifndef DA_ASSERT
#include <assert.h>
#define DA_ASSERT(cond, msg) assert((cond) && (msg))
#endif /* DA_ASSERT */

#ifndef DA_REALLOC
#include <stdlib.h>
#define DA_REALLOCARRAY reallocarray
#endif /* DA_REALLOC */

#ifndef DA_FREE
#include <stdlib.h>
#define DA_FREE free
#endif /* DA_FREE */

// memcpy
#include <string.h>

#define DA_INITIAL_CAPACITY 2

/* ------------------------------- MACROS ------------------------------- */

/** Convenient macro to define a new dynamic array struct (with typedef) that has no other fields. */
#define DA_DEFINE(Type)  \
    typedef struct {     \
        size_t capacity; \
        size_t count;    \
        Type *items;     \
    }

/** Convenient macro to define a new dynamic array struct (without typedef) that has no other fields. */
#define DA_DEFINE_NO_TYPEDEF(name, Type) \
    struct name {                        \
        size_t capacity;                 \
        size_t count;                    \
        Type *items;                     \
    }

#define da_init(da)         \
    do {                    \
        (da)->capacity = 0; \
        (da)->count = 0;    \
        (da)->items = 0;    \
    } while (0)

#define da_clear(da) da_deinit(da)

#define da_deinit(da)              \
    do {                           \
        if ((da) && (da)->items) { \
            DA_FREE((da)->items);  \
        }                          \
        (da)->items = NULL;        \
        (da)->capacity = 0;        \
        (da)->count = 0;           \
    } while (0)

#define da_reserve(da, cap)                                                        \
    do {                                                                           \
        if ((cap) <= (da)->capacity)                                               \
            break;                                                                 \
        size_t newcap = (da)->capacity;                                            \
        if (newcap == 0)                                                           \
            newcap = DA_INITIAL_CAPACITY;                                          \
        while ((cap) > newcap)                                                     \
            newcap *= 2;                                                           \
        void *newmem = DA_REALLOCARRAY((da)->items, newcap, sizeof(*(da)->items)); \
        DA_ASSERT(newmem, "DA_REALLOCARRAY returned NULL");                        \
        (da)->capacity = newcap;                                                   \
        (da)->items = newmem;                                                      \
    } while (0)

#define da_reserve_after(da, increase) \
    da_reserve(da, (da)->count + (increase))

#define da_shrink_to_fit(da)                                                              \
    do {                                                                                  \
        if ((da)->count == 0 || (da)->count == (da)->capacity)                            \
            break;                                                                        \
        (da)->capacity = (da)->count;                                                     \
        (da)->items = DA_REALLOCARRAY((da)->items, (da)->capacity, sizeof(*(da)->items)); \
        DA_ASSERT((da)->items, "DA_REALLOCARRAY returned NULL");                          \
    } while (0)

#define da_append(da, item)                  \
    do {                                     \
        da_reserve_after(da, 1);             \
        (da)->items[(da)->count++] = (item); \
    } while (0)

#define da_append_n(da, arr, arr_count)                                               \
    do {                                                                              \
        da_reserve_after(da, arr_count);                                              \
        memcpy((da)->items + (da)->count, (arr), (arr_count) * sizeof(*(da)->items)); \
        (da)->count += (arr_count);                                                   \
    } while (0)

/**  Variadic version of da_append, but needs the Type of the single element */
#define da_append_many(da, Type, ...)                 \
    do {                                              \
        Type _tmp[] = { __VA_ARGS__ };                \
        size_t _count = sizeof(_tmp) / sizeof(*_tmp); \
        da_append_n((da), _tmp, _count);              \
    } while (0)

/** ONLY works with 'real' arrays (not pointer-decayed) */
#define da_append_array(da, arr) \
    da_append_n(da, arr, sizeof(arr) / sizeof(*arr))

/** Actually works with both da and view. */
#define da_append_da(da, da_src) \
    da_append_n(da, (da_src)->items, (da_src)->count)

/** Actually works with both da and view. */
#define da_append_view(da, da_view_src) \
    da_append_n(da, (da_view_src)->items, (da_view_src)->count)

/** 'it' is a pointer to the current item */
#define da_foreach(Type, it, da) \
    for (Type *it = (da)->items; it < (da)->items + (da)->count; ++it)

/** Applies fun(item) to each item in da */
#define da_apply(da, fun)                          \
    do {                                           \
        for (size_t i = 0; i < (da)->count; i++) { \
            fun((da)->items[i]);                   \
        }                                          \
    } while (0)

/** Applies fun(item, ctx) to each item in da */
#define da_apply_ctx(da, fun, ctx)                 \
    do {                                           \
        for (size_t i = 0; i < (da)->count; i++) { \
            fun((da)->items[i], ctx);              \
        }                                          \
    } while (0)

#define da_empty(da) ((da)->count == 0)

/** This macros don't check index bounds, they are just convenient macros to improve readability. */

#define da_get(da, idx) ((da)->items[(idx)])
#define da_front(da) ((da)->items[0])
#define da_back(da) ((da)->items[(da)->count - 1])

/** Overrides the item in the given position by new item (doesn't check if such index is in bound). */
#define da_set(da, idx, item) (da)->items[idx] = (item)
#define da_set_front(da, item) da_front(da) = (item)
#define da_set_back(da, item) da_back(da) = (item)

/** Inserts the item in the given position, moving the next items and preserving order. */
#define da_insert(da, idx, item)     \
    do {                             \
        if ((idx) == (da)->count) {  \
            da_reserve_after(da, 1); \
            da_set_back(da, item);   \
            break;                   \
        }                            \
        da_shift(da, idx, 1);        \
        (da)->items[(idx)] = (item); \
    } while (0)

/** Shifts items on the right by specified amount, increasing capacity if needed. */
#define da_shift(da, idx, shift)                                                                             \
    do {                                                                                                     \
        DA_ASSERT((idx) <= (da)->count, "Index out of bounds");                                              \
        da_reserve_after(da, shift);                                                                         \
        memmove((da)->items + idx + shift, (da)->items + idx, ((da)->count - (idx)) * sizeof(*(da)->items)); \
        (da)->count += (shift);                                                                              \
    } while (0)

/** If index out of bounds, returns default value. Assumes idx is always positive (otherwise if its type is size_t there will be annoying -Wtype-limits warnings) */
#define da_get_or_default(da, idx, def) ((((size_t)idx) >= (da)->count) ? def : (da)->items[idx])

/**
 * Shallow copy (only copies items).
 * Doesn't reallocate memory if dst has enough space (but doesn't shrink neither; call da_shrink_to_fit afterwards if da_dst was big).
 */
#define da_copy(da_dst, da_src)                                                               \
    do {                                                                                      \
        da_reserve(da_dst, (da_src)->count);                                                  \
        memcpy((da_dst)->items, (da_src)->items, (da_src)->count * sizeof(*(da_src)->items)); \
        (da_dst)->count = (da_src)->count;                                                    \
    } while (0)

/** Moves src's items to dst without reallocating memory. Deallocates dst old items if present. */
#define da_move(da_dst, da_src)   \
    do {                          \
        if ((da_dst) == (da_src)) \
            break;                \
        da_deinit(da_dst);        \
        da_init(da_dst);          \
        *da_dst = *da_src;        \
        da_init(da_src);          \
    } while (0)

/**
 * Returns the index where item is located in da, -1 if not found.
 * Uses memcmp for the comparison.
 */
#define da_find(da, item)                                                       \
    __extension__({                                                             \
        int _found_idx = -1;                                                    \
        for (size_t _i = 0; _i < (da)->count; ++_i) {                           \
            if (memcmp(&(item), &(da)->items[_i], sizeof(*(da)->items)) == 0) { \
                _found_idx = (int)_i;                                           \
                break;                                                          \
            }                                                                   \
        }                                                                       \
        _found_idx;                                                             \
    })

/**
 * Returns 1 if da contains the given item, 0 otherwise.
 * Uses memcmp for the comparison.
 */
#define da_contains(da, item) (da_find(da, item) != -1)

/**
 * VIEW
 * Assuming this structure:
 * struct {
 *      size_t count;
 *      item_type *items;
 * };
 *
 * CONST VIEW
 * Assuming this structure:
 * struct {
 *      size_t count;
 *      const item_type *items;
 * };
 */

#define da_as_view(target_type, da) \
    ((target_type) {                \
        .count = (da)->count,       \
        .items = (da)->items,       \
    })

#define daview_slice(view, offset, count)  \
    ((__typeof__(view)) {                  \
        .count = (count),                  \
        .items = (view)->items + (offset), \
    })

#define da_slice(target_type, da, offset, count) \
    ((target_type) {                             \
        .count = (count),                        \
        .items = (da)->items + (offset),         \
    })

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

#define VIEW_FROM_ARRAY(ViewType, arr) \
    ((ViewType) { .count = ARRAY_COUNT(arr), .items = (arr) })

#define VIEW_FROM(ViewType, ItemType, ...)                                \
    ((ViewType) {                                                               \
        .count = sizeof((const ItemType[]) { __VA_ARGS__ }) / sizeof(ItemType), \
        .items = (const ItemType[]) { __VA_ARGS__ },                            \
    })

#endif /* DA_H */