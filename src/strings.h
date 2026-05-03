#ifndef _STRINGS_H_
#define _STRINGS_H_

#include <stddef.h>

/// Points to an allocated string.
/// Doesn't own the string, nor change it.
typedef struct {
    const char *data;
    size_t count;
} String_View;

#define SV(cstr)   \
    (String_View) {     \
        .data = (cstr), \
        .count = strlen(cstr)  \
    };

#define SV_len(cstr, len)   \
    (String_View) {     \
        .data = (cstr), \
        .count = (len)  \
    };

#define SV_FORMAT "%.*s"
#define SV_ARGS(sv) (int)(sv)->count, (sv)->data

#define SV_EMPTY(sv) ((sv)->count == 0)

String_View sv_chop_left(String_View *sv, size_t num);
String_View sv_chop_right(String_View *sv, size_t num);

/// Delimiter is included in chopped part
String_View sv_chop_until_delim(String_View *sv, char delim);

/// Delimiter is excluded from chopped part
String_View sv_chop_splitby_delim(String_View *sv, char delim);

String_View sv_chop_left_until(String_View *sv, int predicate(int c));
String_View sv_chop_right_until(String_View *sv, int predicate(int c));

String_View sv_chop_left_while(String_View *sv, int predicate(int c));
String_View sv_chop_right_while(String_View *sv, int predicate(int c));

void sv_skip_left(String_View *sv, size_t num);
void sv_skip_right(String_View *sv, size_t num);

void sv_skip_left_until(String_View *sv, int predicate(int c));
void sv_skip_right_until(String_View *sv, int predicate(int c));

void sv_skip_left_while(String_View *sv, int predicate(int c));
void sv_skip_right_while(String_View *sv, int predicate(int c));

// --- useful shortcuts

void sv_clear(String_View *sv);
void sv_skip_spaces_left(String_View *sv);
void sv_skip_spaces_right(String_View* sv);

int sv_strcmp(String_View *sv1, String_View *sv2);
int sv_strcmp_cstr(String_View *sv, const char *cstr);

#endif     // _STRINGS_H_