#include "sv.h"
#include "da.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
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

String_View sv_chop_left(String_View *sv, size_t count) {
    int min = MIN(count, sv->count);

    String_View result = {
        .count = min,
        .data = sv->data
    };

    sv->data += min;
    sv->count -= min;
    return result;
}

String_View sv_chop_right(String_View *sv, size_t count) {
    int min = MIN(count, sv->count);

    String_View result = {
        .count = min,
        .data = sv->data + sv->count - min
    };

    sv->count -= min;
    return result;
}

/* ------------------------------- INTERNAL HELPERS ------------------------------- */

static inline String_View sv_chop_left_withmode(String_View *sv, size_t index, SV_Bound_Mode mode) {
    // chops everything if beyond count
    if (index >= sv->count) return sv_chop_left(sv, sv->count);

    switch (mode) {
        // chops also the delimiter, so it will be removed from sv
        case SV_BOUND_CHOP: return sv_chop_left(sv, index + 1);

        // chops without the delimiter and skips the delimiter in sv
        case SV_BOUND_DROP: {
            String_View chopped = sv_chop_left(sv, index);
            sv_skip_left(sv, 1);
            return chopped;
        }

        default:
        // chops without the delimiter, so it will remain in sv
        case SV_BOUND_KEEP: return sv_chop_left(sv, index);
    }
}

static inline String_View sv_chop_right_withmode(String_View *sv, size_t index, SV_Bound_Mode mode) {
    // chops everything if beyond count
    if (index >= sv->count) return sv_chop_right(sv, sv->count);

    size_t rightIndex = sv->count - 1 - index;
    switch (mode) {
        // chops also the delimiter, so it will be removed from sv
        case SV_BOUND_CHOP: return sv_chop_right(sv, rightIndex + 1);

        // chops without the delimiter and skips the delimiter in sv
        case SV_BOUND_DROP: {
            String_View chopped = sv_chop_right(sv, rightIndex);
            sv_skip_right(sv, 1);
            return chopped;
        }

        default:
        // chops without the delimiter, so it will remain in sv
        case SV_BOUND_KEEP: return sv_chop_right(sv, rightIndex);
    }
}

/* ------------------------------- LEFT ------------------------------- */

String_View sv_chop_left_delim(String_View *sv, char delim, SV_Bound_Mode mode) {
    return sv_chop_left_withmode(sv, sv_find_left_delim(*sv, delim), mode);
}

String_View sv_chop_left_until(String_View *sv, SV_Predicate pred, SV_Bound_Mode mode) {
    return sv_chop_left_withmode(sv, sv_find_left_until(*sv, pred), mode);
}

String_View sv_chop_left_while(String_View *sv, SV_Predicate pred, SV_Bound_Mode mode) {
    return sv_chop_left_withmode(sv, sv_find_left_while(*sv, pred), mode);
}

/* ------------------------------- RIGHT ------------------------------- */

String_View sv_chop_right_delim(String_View *sv, char delim, SV_Bound_Mode mode) {
    return sv_chop_right_withmode(sv, sv_find_right_delim(*sv, delim), mode);
}

String_View sv_chop_right_until(String_View *sv, SV_Predicate pred, SV_Bound_Mode mode) {
    return sv_chop_right_withmode(sv, sv_find_right_until(*sv, pred), mode);
}

String_View sv_chop_right_while(String_View *sv, SV_Predicate pred, SV_Bound_Mode mode) {
    return sv_chop_right_withmode(sv, sv_find_right_while(*sv, pred), mode);
}

/* ------------------------------- PATTERN ------------------------------- */

// clang-format off

/** Helper that returns the result of the matcher 'm' evaluated on given character 'c'. */
static inline int sv_matcher_matches(SV_Matcher m, char c) {
    switch (m.kind) {
        case SV_MATCH_CHAR:     return c == m.as.character;
        case SV_MATCH_CHARS:    return sv_find_left_delim(m.as.chars, c) != m.as.chars.count;
        case SV_MATCH_PRED:     return m.as.predicate((unsigned char)c) != 0;
        case SV_MATCH_NOT_PRED: return m.as.predicate((unsigned char)c) == 0;
        default: return 0;
    }
}
// clang-format on

size_t sv_chop_left_pattern(String_View *sv, SV_Pattern pattern, String_View *out) {
    size_t cursor = 0;

    // for each SV_Pattern_Node, tries to greedly match until node->max
    for (size_t i = 0; i < pattern.count; ++i) {
        const SV_Pattern_Node *node = &pattern.items[i];

        // matches at most node->max characters, breaking if sv->count is reached or the match fails
        size_t matched = 0;
        while (cursor < sv->count && matched < node->max) {
            // tries to match
            if (!sv_matcher_matches(node->matcher, sv->data[cursor])) {
                break;
            }
            ++cursor;
            ++matched;
        }

        // match failed, returns the index of the failed SV_Pattern_Node
        if (matched < node->min) return i;
    }

    // chops cursor characters from the left of 'sv'
    *out = sv_chop_left(sv, cursor);
    return pattern.count;
}

size_t sv_chop_right_pattern(String_View *sv, SV_Pattern pattern, String_View *out) {
    size_t cursor = 0;

    // for each SV_Pattern_Node, tries to greedly match until node->max
    for (size_t i = 0; i < pattern.count; ++i) {
        const SV_Pattern_Node *node = &pattern.items[i];

        // matches at most node->max characters, breaking if sv->count is reached or the match fails
        size_t matched = 0;
        while (cursor < sv->count && matched < node->max) {
            // tries to match
            if (!sv_matcher_matches(node->matcher, sv->data[sv->count - 1 - cursor])) {
                break;
            }
            ++cursor;
            ++matched;
        }

        // match failed, returns the index of the failed SV_Pattern_Node
        if (matched < node->min) return i;
    }

    // chops cursor characters from the right of 'sv'
    *out = sv_chop_right(sv, sv->count - 1 - cursor);
    return sv->count;
}

size_t sv_chop_left_pattern_any(String_View *sv, SV_Patterns patterns, String_View *out) {
    for (size_t i = 0; i < patterns.count; ++i) {
        size_t succeededNodes = sv_chop_left_pattern(sv, patterns.items[i], out);
        if (succeededNodes == patterns.items[i].count)
            return i;
    }
    return SVP_NO_ONE_MATCHED;
}

size_t sv_chop_right_pattern_any(String_View *sv, SV_Patterns patterns, String_View *out) {
    for (size_t i = 0; i < patterns.count; ++i) {
        size_t succeededNodes = sv_chop_right_pattern(sv, patterns.items[i], out);
        if (succeededNodes == patterns.items[i].count)
            return i;
    }
    return SVP_NO_ONE_MATCHED;
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

SV_SkipIf_Result sv_skip_left_if(String_View *sv, char c) {
    if (sv_empty(*sv)) return SV_IS_EMPTY;
    if (sv->data[0] != c) return SV_NOT_FOUND;
    sv_skip_left(sv, 1);
    return SV_OK;
}

SV_SkipIf_Result sv_skip_right_if(String_View *sv, char c) {
    if (sv_empty(*sv)) return SV_IS_EMPTY;
    if (sv->data[sv->count - 1] != c) return SV_NOT_FOUND;
    sv_skip_left(sv, 1);
    return SV_OK;
}

void sv_skip_left_until(String_View *sv, SV_Predicate predicate) {
    sv_skip_left(sv, sv_find_left_until(*sv, predicate));
}

void sv_skip_right_until(String_View *sv, SV_Predicate predicate) {
    sv_skip_right(sv, sv_find_right_until(*sv, predicate));
}

void sv_skip_left_while(String_View *sv, SV_Predicate predicate) {
    sv_skip_left(sv, sv_find_left_while(*sv, predicate));
}

void sv_skip_right_while(String_View *sv, SV_Predicate predicate) {
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

String_View sv_slice_left(String_View sv, size_t count) {
    count = MIN(count, sv.count);
    return SV_LEN(sv.data, count);
}

String_View sv_slice_right(String_View sv, size_t count) {
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

/* =============================== STRING OPERATIONS =============================== */

String sv_strcat(String_View sv1, String_View sv2) {
    String output = { 0 };
    da_init(&output);
    da_append_view(&output, &sv1);
    da_append_view(&output, &sv2);
    return output;
}

String sv_strcat_cstr(String_View sv1, const char *cstr) {
    String output = { 0 };
    da_init(&output);
    da_append_view(&output, &sv1);
    da_append_n(&output, cstr, strlen(cstr));
    return output;
}

String sv_strcat_many(size_t argc, ...) {
    String output = { 0 };
    da_init(&output);

    va_list ap;
    va_start(ap, argc);
    for (size_t i = 0; i < argc; ++i) {
        String_View curr = va_arg(ap, String_View);
        da_append_view(&output, &curr);
    }
    va_end(ap);

    return output;
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