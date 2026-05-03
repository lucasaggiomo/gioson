#include "strings.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) ((a) <= (b) ? (a) : (b))

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

String_View sv_chop_until_delim(String_View *sv, char delim) {
    size_t i = 0;     // chopped_count
    while (i < sv->count && sv->data[i] != delim) {
        i++;
    }

    if (i < sv->count) {
        // delimiter found, chops over it
        String_View result = sv_chop_left(sv, i + 1);
        return result;
    }

    // delimiter not found, chops everything
    // note that result is just a copy of sv before chopping
    String_View result = sv_chop_left(sv, sv->count);
    return result;
}

String_View sv_chop_splitby_delim(String_View *sv, char delim) {
    size_t i = 0;     // chopped_count
    while (i < sv->count && sv->data[i] != delim) {
        i++;
    }

    if (i < sv->count) {
        // delimiter found, chops over it
        String_View result = sv_chop_left(sv, i);
        sv_skip_left(sv, 1);
        return result;
    }

    // delimiter not found, chops everything
    // note that result is just a copy of sv before chopping
    String_View result = sv_chop_left(sv, sv->count);
    return result;
}

String_View sv_chop_left_until(String_View *sv, int predicate(int c)) {
    size_t i = 0;     // chopped_count
    while (i < sv->count && !predicate(sv->data[i])) {
        i++;
    }

    if (i < sv->count) {
        // delimiter found, chops over it
        return sv_chop_left(sv, i + 1);
    }

    // delimiter found, chops over it
    // note that result is just a copy of sv before chopping
    return sv_chop_left(sv, sv->count);
}

String_View sv_chop_right_until(String_View *sv, int predicate(int c)) {
    size_t i = 0;     // chopped_count
    while (i < sv->count && !predicate(sv->data[sv->count - i - 1])) {
        i++;
    }

    if (i < sv->count) {
        // delimiter found, chops over it
        return sv_chop_right(sv, i + 1);
    }

    // delimiter found, chops over it
    // note that result is just a copy of sv before chopping
    return sv_chop_right(sv, sv->count);
}

String_View sv_chop_left_while(String_View *sv, int predicate(int c)) {
    size_t i = 0;
    while (i < sv->count && predicate(sv->data[i])) {
        i++;
    }
    return sv_chop_left(sv, i);
}

String_View sv_chop_right_while(String_View *sv, int predicate(int c)) {
    size_t i = 0;
    while (i < sv->count && predicate(sv->data[sv->count - i - 1])) {
        i++;
    }
    return sv_chop_right(sv, i);
}

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
    size_t i = 0;
    while (i < sv->count && !predicate(sv->data[i])) {
        i++;
    }
    sv_skip_left(sv, i);
}

void sv_skip_right_until(String_View *sv, int predicate(int c)) {
    size_t i = 0;
    while (i < sv->count && !predicate(sv->data[sv->count - i - 1])) {
        i++;
    }
    sv_skip_right(sv, i);
}

void sv_skip_left_while(String_View *sv, int predicate(int c)) {
    size_t i = 0;
    while (i < sv->count && predicate(sv->data[i])) {
        i++;
    }
    sv_skip_left(sv, i);
}

void sv_skip_right_while(String_View *sv, int predicate(int c)) {
    size_t i = 0;
    while (i < sv->count && predicate(sv->data[sv->count - i - 1])) {
        i++;
    }
    sv_skip_right(sv, i);
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

int sv_strcmp(String_View *sv1, String_View *sv2) {
    if (sv1->count > sv2->count)
        return 1;
    else if (sv1->count < sv2->count)
        return -1;

    return strncmp(sv1->data, sv2->data, sv1->count);
}

int sv_strcmp_cstr(String_View *sv, const char *cstr) {
    size_t len = (size_t)strlen(cstr);
    if (sv->count > len)
        return 1;
    else if (sv->count < len)
        return -1;

    return strncmp(sv->data, cstr, len);
}

// #include <assert.h>

// void test1() {
//     String_View sv = SV("ciao,luca");
//     String_View tok1 = sv_chop_until_delim(&sv, ',');
//     assert(!sv_strcmp_cstr(&tok1, "ciao,"));
//     assert(!sv_strcmp_cstr(&sv, "luca"));
// }

// void test2() {
//     String_View sv = SV("ciao,luca");
//     String_View tok1 = sv_chop_splitby_delim(&sv, ',');
//     assert(!sv_strcmp_cstr(&tok1, "ciao"));
//     assert(!sv_strcmp_cstr(&sv, "luca"));
// }

// void test3() {
//     String_View sv = SV("123,luca");
//     String_View tok1 = sv_chop_left_while(&sv, isdigit);
//     assert(!sv_strcmp_cstr(&tok1, "123"));
//     assert(!sv_strcmp_cstr(&sv, ",luca"));
// }

// void test4() {
//     String_View sv = SV("123,luca");
//     String_View tok1 = sv_chop_right_while(&sv, isalpha);
//     assert(!sv_strcmp_cstr(&tok1, "luca"));
//     assert(!sv_strcmp_cstr(&sv, "123,"));
// }

// int main(void) {
//     test1();
//     test2();
//     test3();
//     test4();
// }