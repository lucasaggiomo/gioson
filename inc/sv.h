#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <stddef.h>
#include <string.h>

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

/** Returns the index of the first character in 'sv' from left such that the predicate is false, otherwise returns sv->count. */
size_t sv_find_left_until(String_View sv, int predicate(int c));

/** Returns the index of the first character in 'sv' from left such that the predicate is true, otherwise returns sv->count. */
size_t sv_find_left_while(String_View sv, int predicate(int c));

/* ------------------------------- RIGHT ------------------------------- */

/** Returns the index of the first found delimiter in 'sv' from right, otherwise returns sv->count. */
size_t sv_find_right_delim(String_View sv, char delim);

/** Returns the index of the first character in 'sv' from right such that the predicate is false, otherwise returns sv->count. */
size_t sv_find_right_until(String_View sv, int predicate(int c));

/** Returns the index of the first character in 'sv' from right such that the predicate is true, otherwise returns sv->count. */
size_t sv_find_right_while(String_View sv, int predicate(int c));

/* =============================== CHOP =============================== */

/* ------------------------------- RAW ------------------------------- */

/** Returns a new String_View of the first 'num' characters, chopping them by 'sv'. */
String_View sv_chop_left(String_View *sv, size_t num);

/** Returns a new String_View of the last 'num' characters, chopping them by 'sv'. */
String_View sv_chop_right(String_View *sv, size_t num);

/* ------------------------------- DELIM ------------------------------- */

/**
 * Chops from the left until the first delimiter is found or chops all if not found. The delimiter is included in chopped part.
 * e.g.: 'Giov.anni' with delim '.' results in:
 *   - output = 'Giov.'
 *   - sv = 'anni'
 */
String_View sv_chop_left_untildelim(String_View *sv, char delim);

/**
 * Chops from the right until the first delimiter is found or chops all if not found. The delimiter is included in chopped part.
 * e.g.: 'Giov.anni' with delim '.' results in:
 *   - output = '.anni'
 *   - sv = 'Giov'
 */
String_View sv_chop_right_untildelim(String_View *sv, char delim);

/**
 * Chops from the left until the first delimiter is found or chops all if not found. The delimiter is excluded in chopped part.
 * e.g.: 'Giov.anni' with delim '.' results in:
 *   - output = 'Giov'
 *   - sv = 'anni'
 */
String_View sv_chop_left_splitbydelim(String_View *sv, char delim);

/**
 * Chops from the left until the first delimiter is found or chops all if not found. The delimiter is excluded in chopped part.
 * e.g.: 'Giov.anni' with delim '.' results in:
 *   - output = 'anni'
 *   - sv = 'Giov'
 */
String_View sv_chop_right_splitbydelim(String_View *sv, char delim);

/**
 * Chops from the left until the predicate becomes true or chops all if it's never true. The delimiter is included in chopped part.
 * e.g.: 'Giov anni' with predicate isspace results in:
 *   - output = 'Giov '
 *   - sv = 'anni'
 */
String_View sv_chop_left_until(String_View *sv, int predicate(int c));

/**
 * Chops from the right until the predicate becomes true or chops all if it's never true. The delimiter is included in chopped part.
 * e.g.: 'Giov anni' with predicate isspace results in:
 *   - output = ' anni'
 *   - sv = 'Giov'
 */
String_View sv_chop_right_until(String_View *sv, int predicate(int c));

/**
 * Chops from the left until the predicate becomes true or chops all if it's never true. The delimiter is excluded in chopped part.
 * e.g.: 'Giov anni' with predicate isspace results in:
 *   - output = 'Giov'
 *   - sv = 'anni'
 */
String_View sv_chop_left_splitby(String_View *sv, int predicate(int c));

/**
 * Chops from the right until the predicate becomes true or chops all if it's never true. The delimiter is excluded in chopped part.
 * e.g.: 'Giov anni' with predicate isspace results in:
 *   - output = 'anni'
 *   - sv = 'Giov'
 */
String_View sv_chop_right_splitby(String_View *sv, int predicate(int c));

/**
 * Chops from the left until the predicate becomes false or chops all if it's never false. The delimiter is included in chopped part.
 * e.g.: 'aaaaBcccc' with predicate isvowel results in:
 *   - output = 'aaaaB'
 *   - sv = 'cccc'
 */
String_View sv_chop_left_whilewith(String_View *sv, int predicate(int c));     // naming sucks

/**
 * Chops from the right until the predicate becomes false or chops all if it's never false. The delimiter is included in chopped part.
 * e.g.: 'aaaaBcccc' with predicate isvowel results in:
 *   - output = 'Bcccc'
 *   - sv = 'aaaa'
 */
String_View sv_chop_right_whilewith(String_View *sv, int predicate(int c));     // naming sucks

/**
 * Chops from the left until the predicate becomes false or chops all if it's never false. The delimiter is excluded in chopped part.
 * e.g.: 'aaaaBcccc' with predicate isvowel results in:
 *   - output = 'aaaa'
 *   - sv = 'cccc'
 */
String_View sv_chop_left_while(String_View *sv, int predicate(int c));

/**
 * Chops from the right until the predicate becomes false or chops all if it's never false. The delimiter is excluded in chopped part.
 * e.g.: 'aaaaBcccc' with predicate isvowel results in:
 *   - output = 'cccc'
 *   - sv = 'aaaa'
 */
String_View sv_chop_right_while(String_View *sv, int predicate(int c));

/**
 * Chops from the left until the predicate becomes false or chops all if it's never false. The delimiter is excluded in chopped part but remains in 'sv'.
 * e.g.: 'aaaaBcccc' with predicate isvowel results in:
 *   - output = 'aaaa'
 *   - sv = 'Bcccc'
 */
String_View sv_chop_left_while_retain(String_View *sv, int predicate(int c));

/**
 * Chops from the right until the predicate becomes false or chops all if it's never false. The delimiter is excluded in chopped part but remains in 'sv'.
 * e.g.: 'aaaaBcccc' with predicate isvowel results in:
 *   - output = 'cccc'
 *   - sv = 'aaaaB'
 */
String_View sv_chop_right_while_retain(String_View *sv, int predicate(int c));

/* =============================== SKIP =============================== */

/** Skips the first 'num' characters in 'sv'. */
void sv_skip_left(String_View *sv, size_t num);

/** Skips the last 'num' characters in 'sv'. */
void sv_skip_right(String_View *sv, size_t num);

/** Skips characters in 'sv' from left until the predicate becames true or skips all if it's never true. */
void sv_skip_left_until(String_View *sv, int predicate(int c));

/** Skips characters in 'sv' from right until the predicate becames true or skips all if it's never true. */
void sv_skip_right_until(String_View *sv, int predicate(int c));

/** Skips characters in 'sv' from left until the predicate becames false or skips all if it's never false. */
void sv_skip_left_while(String_View *sv, int predicate(int c));

/** Skips characters in 'sv' from right until the predicate becames false or skips all if it's never false. */
void sv_skip_right_while(String_View *sv, int predicate(int c));

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

String_View sv_left_slice(String_View sv, size_t count);
String_View sv_right_slice(String_View sv, size_t count);
String_View sv_slice(String_View sv, size_t offset, size_t count);

#endif     // STRING_VIEW_H