#include "sv.h"
#include <ctype.h>
#include <string.h>

#define MIN(a, b) ((a) <= (b) ? (a) : (b))

/* =============================== FIND =============================== */

/* ------------------------------- LEFT ------------------------------- */

size_t sv_find_left_delim(String_View sv, char delim) {
    for (size_t i = 0; i < sv.count; ++i) {
        if (sv.data[i] == delim) return i;
    }
    return sv.count;
}

size_t sv_find_left_until(String_View sv, int predicate(int c)) {
    for (size_t i = 0; i < sv.count; ++i) {
        if (predicate(sv.data[i])) return i;
    }
    return sv.count;
}

size_t sv_find_left_while(String_View sv, int predicate(int c)) {
    for (size_t i = 0; i < sv.count; ++i) {
        if (!predicate(sv.data[i])) return i;
    }
    return sv.count;
}

/* ------------------------------- RIGHT ------------------------------- */

size_t sv_find_right_delim(String_View sv, char delim) {
    for (size_t i = sv.count; i-- > 0;) {
        if (sv.data[i] == delim) return i;
    }
    return sv.count;
}

size_t sv_find_right_until(String_View sv, int predicate(int c)) {
    for (size_t i = sv.count; i-- > 0;) {
        if (predicate(sv.data[i])) return i;
    }
    return sv.count;
}

size_t sv_find_right_while(String_View sv, int predicate(int c)) {
    for (size_t i = sv.count; i-- > 0;) {
        if (!predicate(sv.data[i])) return i;
    }
    return sv.count;
}

/* =============================== CHOP =============================== */

/* ------------------------------- RAW ------------------------------- */

String_View sv_chop_left(String_View *sv, size_t num) {
    int min = MIN(num, sv->count);

    String_View result = {
        .count = min,
        .data = sv->data
    };

    sv->data += min;
    sv->count -= min;
    return result;
}

String_View sv_chop_right(String_View *sv, size_t num) {
    int min = MIN(num, sv->count);

    String_View result = {
        .count = min,
        .data = sv->data + sv->count - min
    };

    sv->count -= min;
    return result;
}

/* ------------------------------- DELIM ------------------------------- */

String_View sv_chop_left_untildelim(String_View *sv, char delim) {
    size_t i = sv_find_left_delim(*sv, delim);
    if (i < sv->count) ++i;     // chops also the delimiter in the return sv
    return sv_chop_left(sv, i);
}

String_View sv_chop_right_untildelim(String_View *sv, char delim) {
    size_t i = sv_find_right_delim(*sv, delim);
    if (i == sv->count) i = 0;
    return sv_chop_right(sv, sv->count - i);
}

String_View sv_chop_left_splitbydelim(String_View *sv, char delim) {
    size_t i = sv_find_left_delim(*sv, delim);
    String_View chopped = sv_chop_left(sv, i);
    if (i < sv->count) sv_skip_left(sv, 1);     // skips the delimiter in sv
    return chopped;
}

String_View sv_chop_right_splitbydelim(String_View *sv, char delim) {
    size_t i = sv_find_right_delim(*sv, delim);
    String_View chopped = sv_chop_right(sv, sv->count - i - 1);
    if (i < sv->count) sv_skip_right(sv, 1);     // skips the delimiter in sv
    return chopped;
}

String_View sv_chop_left_until(String_View *sv, int predicate(int c)) {
    size_t i = sv_find_left_until(*sv, predicate);
    if (i < sv->count) ++i;
    return sv_chop_left(sv, i);
}

String_View sv_chop_right_until(String_View *sv, int predicate(int c)) {
    size_t i = sv_find_right_until(*sv, predicate);
    if (i == sv->count) i = 0;
    return sv_chop_right(sv, sv->count - i);
}

String_View sv_chop_left_splitby(String_View *sv, int predicate(int c)) {
    size_t i = sv_find_left_until(*sv, predicate);
    String_View chopped = sv_chop_left(sv, i);
    if (i < sv->count) sv_skip_left(sv, 1);     // skips the delimiter in sv
    return chopped;
}

String_View sv_chop_right_splitby(String_View *sv, int predicate(int c)) {
    size_t i = sv_find_right_until(*sv, predicate);
    String_View chopped = sv_chop_right(sv, sv->count - i - 1);
    if (i < sv->count) sv_skip_right(sv, 1);     // skips the delimiter in sv
    return chopped;
}

String_View sv_chop_left_whilewith(String_View *sv, int predicate(int c)) {
    size_t i = sv_find_left_while(*sv, predicate);
    if (i < sv->count) ++i;
    return sv_chop_left(sv, i);
}

String_View sv_chop_right_whilewith(String_View *sv, int predicate(int c)) {
    size_t i = sv_find_right_while(*sv, predicate);
    if (i == sv->count) i = 0;
    return sv_chop_right(sv, sv->count - i);
}

String_View sv_chop_left_while(String_View *sv, int predicate(int c)) {
    size_t i = sv_find_left_while(*sv, predicate);
    String_View chopped = sv_chop_left(sv, i);
    if (i < sv->count) sv_skip_left(sv, 1);     // skips the delimiter in sv
    return chopped;
}

String_View sv_chop_right_while(String_View *sv, int predicate(int c)) {
    size_t i = sv_find_right_while(*sv, predicate);
    String_View chopped = sv_chop_right(sv, sv->count - i - 1);
    if (i < sv->count) sv_skip_right(sv, 1);     // skips the delimiter in sv
    return chopped;
}

String_View sv_chop_left_while_retain(String_View *sv, int predicate(int c)) {
    size_t i = sv_find_left_while(*sv, predicate);
    return sv_chop_left(sv, i);
}

String_View sv_chop_right_while_retain(String_View *sv, int predicate(int c)) {
    size_t i = sv_find_right_while(*sv, predicate);
    return sv_chop_right(sv, sv->count - i - 1);
}

/* =============================== SKIP =============================== */

void sv_skip_left(String_View *sv, size_t num) {
    int min = MIN(num, sv->count);
    sv->data += min;
    sv->count -= min;
}

void sv_skip_right(String_View *sv, size_t num) {
    int min = MIN(num, sv->count);
    sv->count -= min;
}

void sv_skip_left_until(String_View *sv, int predicate(int c)) {
    sv_skip_left(sv, sv_find_left_until(*sv, predicate));
}

void sv_skip_right_until(String_View *sv, int predicate(int c)) {
    sv_skip_right(sv, sv_find_right_until(*sv, predicate));
}

void sv_skip_left_while(String_View *sv, int predicate(int c)) {
    sv_skip_left(sv, sv_find_left_while(*sv, predicate));
}

void sv_skip_right_while(String_View *sv, int predicate(int c)) {
    sv_skip_right(sv, sv_find_right_while(*sv, predicate));
}

void sv_clear(String_View *sv) {
    sv->count = 0;
}

void sv_skip_spaces_left(String_View *sv) {
    sv_skip_left_while(sv, isspace);
}

void sv_skip_spaces_right(String_View *sv) {
    sv_skip_right_while(sv, isspace);
}

int sv_strcmp(String_View sv1, String_View sv2) {
    if (sv1.count > sv2.count)
        return 1;
    else if (sv1.count < sv2.count)
        return -1;

    return strncmp(sv1.data, sv2.data, sv1.count);
}

int sv_strcmp_cstr(String_View sv, const char *cstr) {
    size_t len = (size_t)strlen(cstr);
    if (sv.count > len)
        return 1;
    else if (sv.count < len)
        return -1;

    return strncmp(sv.data, cstr, len);
}

String_View sv_left_slice(String_View sv, size_t count) {
    count = MIN(count, sv.count);
    return SV_LEN(sv.data, count);
}

String_View sv_right_slice(String_View sv, size_t count) {
    count = MIN(count, sv.count);
    return (String_View) {
        .data = sv.data + (sv.count - count),
        .count = count
    };
}

String_View sv_slice(String_View sv, size_t offset, size_t count) {
    offset = MIN(offset, sv.count);
    if (offset == 0) return SV_NULL;

    count = MIN(sv.count - offset, count);
    return (String_View) {
        .data = sv.data + offset,
        .count = count
    };
}