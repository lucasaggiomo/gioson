#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include "str.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef int (*SV_Predicate)(int c);

/** non-owning, readonly string + count of characters, compliant to da.h as a Const_View of a dynamic array of char. */
typedef struct {
    size_t count;
    union {
        const char *data;
        const char *items;
    };
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

static inline String_View sv_from_str(String str) {
    return (String_View) { .count = str.count, .data = str.data };
}

#define SV_FORMAT "%.*s"
#define SV_ARGS(sv) (int)(sv)->count, (sv)->data

#define SV_REPR_FORMAT "(String_View) { .count = %zu, .data = \"" SV_FORMAT "\" }"
#define SV_REPR_ARGS(sv) (sv)->count, SV_ARGS(sv)

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

typedef enum {
    SV_BOUND_DROP,     // Excluded from output, removed from input: "Giov.anni" -> [ "Giov ", "anni"  ]
    SV_BOUND_CHOP,     // Included in output, removed from input:   "Giov.anni" -> [ "Giov.", "anni"  ]
    SV_BOUND_KEEP      // Excluded from output, kept in input:      "Giov.anni" -> [ "Giov ", ".anni" ]
} SV_Bound_Mode;

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

/* =============================== PATTERN =============================== */

typedef enum {
    SV_MATCH_CHAR,
    SV_MATCH_CHARS,
    SV_MATCH_PRED,
    SV_MATCH_NOT_PRED,
} SV_Match_Kind;

typedef struct {
    SV_Match_Kind kind;
    union {
        char character;             // SV_MATCH_CHAR
        String_View chars;          // SV_MATCH_CHARS
        SV_Predicate predicate;     // SV_MATCH_PRED | SV_MATCH_NOT_PRED
    } as;
} SV_Matcher;

typedef struct {
    SV_Matcher matcher;
    size_t min;
    size_t max;
} SV_Pattern_Node;

typedef struct {
    size_t count;
    const SV_Pattern_Node *items;
} SV_Pattern;

/* ------------------------------- MATCHERS ------------------------------- */

// clang-format off

#define SVP_CHAR(c)      ((SV_Matcher){ .kind = SV_MATCH_CHAR,     .as.character = (c)       })
#define SVP_CHARS(s)     ((SV_Matcher){ .kind = SV_MATCH_CHARS,    .as.chars = SV_LITERAL(s) })
#define SVP_CHARS_SV(sv) ((SV_Matcher){ .kind = SV_MATCH_CHARS,    .as.chars = (sv)          })
#define SVP_PRED(p)      ((SV_Matcher){ .kind = SV_MATCH_PRED,     .as.predicate = (p)       })
#define SVP_NOT_PRED(p)  ((SV_Matcher){ .kind = SV_MATCH_NOT_PRED, .as.predicate = (p)       })

/* ------------------------------- QUANTIFIERS ------------------------------- */

#define SVP_OPT(m)              ((SV_Pattern_Node){ .matcher = (m), .min = 0,    .max = 1        })
#define SVP_ONCE(m)             ((SV_Pattern_Node){ .matcher = (m), .min = 1,    .max = 1        })
#define SVP_MANY(m)             ((SV_Pattern_Node){ .matcher = (m), .min = 1,    .max = SIZE_MAX })
#define SVP_ANY(m)              ((SV_Pattern_Node){ .matcher = (m), .min = 0,    .max = SIZE_MAX })
#define SVP_AT_LEAST(n, m)      ((SV_Pattern_Node){ .matcher = (m), .min = (n),  .max = SIZE_MAX })
#define SVP_EXACTLY(n, m)       ((SV_Pattern_Node){ .matcher = (m), .min = (n),  .max = (n)      })
#define SVP_IN_RANGE(lo, hi, m) ((SV_Pattern_Node){ .matcher = (m), .min = (lo), .max = (hi)     })

// clang-format on

/**
 * SV_Pattern builder.
 *
 * E.g. that matches Regex: `[+-]? [0-9]* \.? [0-9]+` (spaces added for readability):
 * ```c
 *   SV_Pattern float_pattern = SV_PATTERN(
 *       SVP_OPT(SVP_CHARS("+-")),
 *       SVP_ANY(SVP_PRED(isdigit)),
 *       SVP_OPT(SVP_CHAR('.')),
 *       SVP_MANY(SVP_PRED(isdigit))
 *   );
 * ```
 */
#define SV_PATTERN(...) VIEW_FROM(SV_Pattern, SV_Pattern_Node, __VA_ARGS__)

/**
 * Chops from the left the given pattern if sv matches it.
 * Returns the number of the successful SV_Pattern_Node, i.e.:
 * - if the match fails, nothing is done to 'sv' and returns the index of the failed SV_Pattern_Node.
 * - if the match succeeds, the matched string is left-chopped from 'sv' and pattern.count is returned.
 *
 * NOTE: the pattern matching is done greedily without backtracking.
 */
size_t sv_chop_left_pattern(String_View *sv, SV_Pattern pattern, String_View *out);

/**
 * Chops from the right the given pattern if sv matches it.
 * Returns the number of the successful SV_Pattern_Node, i.e.:
 * - if the match fails, nothing is done to 'sv' and returns the index of the failed SV_Pattern_Node.
 * - if the match succeeds, the matched string is right-chopped from 'sv' and pattern.count is returned.
 *
 * NOTE: the pattern matching is done greedily without backtracking.
 */
size_t sv_chop_right_pattern(String_View *sv, SV_Pattern pattern, String_View *out);

typedef struct {
    size_t count;
    const SV_Pattern *items;
} SV_Patterns;

#define SV_PATTERNS(...) VIEW_FROM(SV_Patterns, SV_Pattern, __VA_ARGS__)

#define SVP_NO_ONE_MATCHED (size_t)-1

/**
 * Left-chops the first pattern in 'patterns' that 'sv' matches.
 * The chopped part is saved in '*out'.
 * Returns the index of the pattern that sv matched in patterns, or SVP_NO_ONE_MATCHED = (size_t)-1 if no one matched.
 */
size_t sv_chop_left_pattern_any(String_View *sv, SV_Patterns patterns, String_View *out);

/**
 * Right-chops the first pattern in 'patterns' that 'sv' matches.
 * The chopped part is saved in '*out'.
 * Returns the index of the pattern that sv matched in patterns, or SVP_NO_ONE_MATCHED = (size_t)-1 if no one matched.
 */
size_t sv_chop_right_pattern_any(String_View *sv, SV_Patterns patterns, String_View *out);

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

String_View sv_slice_left(String_View sv, size_t count);
String_View sv_slice_right(String_View sv, size_t count);
String_View sv_slice(String_View sv, size_t offset, size_t count);

/* =============================== STRING OPERATIONS =============================== */

String sv_strcat(String_View sv1, String_View sv2);
String sv_strcat_cstr(String_View sv1, const char *cstr);
String sv_strcat_many(size_t argc, ...);

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

#endif     // STRING_VIEW_H