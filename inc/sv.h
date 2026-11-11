#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <stddef.h>
#include <string.h>

typedef int (*SV_Predicate)(int c);

typedef enum {
    SV_BOUND_DROP,     // Excluded from output, removed from input: "Giov.anni" -> [ "Giov ", "anni"  ]
    SV_BOUND_CHOP,     // Included in output, removed from input:   "Giov.anni" -> [ "Giov.", "anni"  ]
    SV_BOUND_KEEP      // Excluded from output, kept in input:      "Giov.anni" -> [ "Giov ", ".anni" ]
} SV_Bound_Mode;

/// Points to an allocated string.
/// Doesn't own the string, nor change it.
typedef struct {
    size_t count;
    const char *data;
} String_View;

#define SV_LITERAL(lit)           \
    ((String_View) {              \
        .count = sizeof(lit) - 1, \
        .data = (lit),            \
    })

#define SV(cstr)               \
    ((String_View) {           \
        .count = strlen(cstr), \
        .data = (cstr),        \
    })

#define SV_LEN(str, len) \
    ((String_View) {     \
        .count = (len),  \
        .data = (str),   \
    })

#define SV_NULL \
    (String_View) { .count = 0, .data = NULL }

#define SV_FORMAT "%.*s"
#define SV_ARGS(sv) (int)(sv)->count, (sv)->data

/* =============================== FIND =============================== */

/* ------------------------------- LEFT ------------------------------- */

/** Returns the index of the first found delimiter in 'sv' from left, otherwise returns sv->count. */
size_t sv_find_left_delim(String_View sv, char delim);

/** Returns the index of the first character in 'sv' from left such that the predicate is true, otherwise returns sv->count. */
size_t sv_find_left_until(String_View sv, SV_Predicate predicate);

/** Returns the index of the first character in 'sv' from left such that the predicate is false, otherwise returns sv->count. */
size_t sv_find_left_while(String_View sv, SV_Predicate predicate);

/* ------------------------------- RIGHT ------------------------------- */

/** Returns the index of the first found delimiter in 'sv' from right, otherwise returns sv->count. */
size_t sv_find_right_delim(String_View sv, char delim);

/** Returns the index of the first character in 'sv' from right such that the predicate is false, otherwise returns sv->count. */
size_t sv_find_right_until(String_View sv, SV_Predicate predicate);

/** Returns the index of the first character in 'sv' from right such that the predicate is true, otherwise returns sv->count. */
size_t sv_find_right_while(String_View sv, SV_Predicate predicate);

/* =============================== CHOP =============================== */

/* ------------------------------- RAW ------------------------------- */

/** Returns a new String_View of the first 'count' characters, chopping them by 'sv'. */
String_View sv_chop_left(String_View *sv, size_t count);

/** Returns a new String_View of the last 'count' characters, chopping them by 'sv'. */
String_View sv_chop_right(String_View *sv, size_t count);

/* ------------------------------- LEFT ------------------------------- */

String_View sv_chop_left_delim(String_View *sv, char delim, SV_Bound_Mode mode);
String_View sv_chop_left_until(String_View *sv, SV_Predicate pred, SV_Bound_Mode mode);
String_View sv_chop_left_while(String_View *sv, SV_Predicate pred, SV_Bound_Mode mode);

/* ------------------------------- RIGHT ------------------------------- */

String_View sv_chop_right_delim(String_View *sv, char delim, SV_Bound_Mode mode);
String_View sv_chop_right_until(String_View *sv, SV_Predicate pred, SV_Bound_Mode mode);
String_View sv_chop_right_while(String_View *sv, SV_Predicate pred, SV_Bound_Mode mode);

/* ------------------------------- SHORTCUTS ------------------------------- */

static inline String_View sv_chop_left_splitbydelim(String_View *sv, char delim) { return sv_chop_left_delim(sv, delim, SV_BOUND_DROP); }
static inline String_View sv_chop_left_splitby(String_View *sv, SV_Predicate pred) { return sv_chop_left_until(sv, pred, SV_BOUND_DROP); }

static inline String_View sv_chop_right_splitbydelim(String_View *sv, char delim) { return sv_chop_right_delim(sv, delim, SV_BOUND_DROP); }
static inline String_View sv_chop_right_splitby(String_View *sv, SV_Predicate pred) { return sv_chop_right_until(sv, pred, SV_BOUND_DROP); }

/* =============================== SKIP =============================== */

/** Skips the first 'num' characters in 'sv'. */
void sv_skip_left(String_View *sv, size_t num);

/** Skips the last 'num' characters in 'sv'. */
void sv_skip_right(String_View *sv, size_t num);

typedef enum {
    SV_OK,
    SV_NOT_FOUND,
    SV_IS_EMPTY
} SV_SkipIf_Result;

/**
 * Skips the first character if it's c, does nothing otherwise (or if the sv is empty).
 * Returns SV_OK (0) if it has been skipped, SV_NOT_FOUND (1) if the first character is not c, SV_IS_EMPTY (2) if sv is empty.
 */
SV_SkipIf_Result sv_skip_left_if(String_View *sv, char c);

/**
 * Skips the last character if it's c, does nothing otherwise (or if the sv is empty).
 * Returns SV_OK (0) if it has been skipped, SV_NOT_FOUND (1) if the last character is not c, SV_IS_EMPTY (2) if sv is empty.
 */
SV_SkipIf_Result sv_skip_right_if(String_View *sv, char c);

/** Skips characters in 'sv' from left until the predicate becames true or skips all if it's never true. */
void sv_skip_left_until(String_View *sv, SV_Predicate predicate);

/** Skips characters in 'sv' from right until the predicate becames true or skips all if it's never true. */
void sv_skip_right_until(String_View *sv, SV_Predicate predicate);

/** Skips characters in 'sv' from left until the predicate becames false or skips all if it's never false. */
void sv_skip_left_while(String_View *sv, SV_Predicate predicate);

/** Skips characters in 'sv' from right until the predicate becames false or skips all if it's never false. */
void sv_skip_right_while(String_View *sv, SV_Predicate predicate);

// --- useful shortcuts

void sv_clear(String_View *sv);
void sv_skip_spaces_left(String_View *sv);
void sv_skip_spaces_right(String_View *sv);

int sv_strcmp(String_View sv1, String_View sv2);
int sv_strcmp_cstr(String_View sv, const char *cstr);

static inline int sv_equals(String_View sv1, String_View sv2) {
    return sv_strcmp(sv1, sv2) == 0;
}

static inline int sv_equals_cstr(String_View sv, const char *cstr) {
    return sv_strcmp_cstr(sv, cstr) == 0;
}

static inline int sv_empty(String_View sv) {
    return sv.count == 0;
}

String_View sv_slice_left(String_View sv, size_t count);
String_View sv_slice_right(String_View sv, size_t count);
String_View sv_slice(String_View sv, size_t offset, size_t count);

#endif     // STRING_VIEW_H