#include "sv.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

static inline int isnotspace(int c) { return !isspace(c); }

void test_find_delim() {
    String_View sv = SV("ciao luca come ti chiami");
    assert(sv_find_left_delim(sv, ' ') == 4);
    assert(sv_find_right_delim(sv, ' ') == 17);
}

void test_find_until() {
    String_View sv = SV("ciao luca come ti chiami");
    assert(sv_find_left_until(sv, isspace) == 4);
    assert(sv_find_right_until(sv, isspace) == 17);
}

void test_find_while() {
    String_View sv = SV("ciao luca come ti chiami");
    assert(sv_find_left_while(sv, isnotspace) == 4);
    assert(sv_find_right_while(sv, isnotspace) == 17);
}

void test_chop_left_untildelim() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_untildelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "ciao "));
    assert(sv_equals_cstr(sv, "luca come ti chiami"));

    tok = sv_chop_left_untildelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "luca "));
    assert(sv_equals_cstr(sv, "come ti chiami"));

    tok = sv_chop_left_untildelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "come "));
    assert(sv_equals_cstr(sv, "ti chiami"));

    tok = sv_chop_left_untildelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "ti "));
    assert(sv_equals_cstr(sv, "chiami"));

    tok = sv_chop_left_untildelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));
}

void test_chop_right_untildelim() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_untildelim(&sv, ' ');
    assert(sv_equals_cstr(tok, " chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti"));

    tok = sv_chop_right_untildelim(&sv, ' ');
    assert(sv_equals_cstr(tok, " ti"));
    assert(sv_equals_cstr(sv, "ciao luca come"));

    tok = sv_chop_right_untildelim(&sv, ' ');
    assert(sv_equals_cstr(tok, " come"));
    assert(sv_equals_cstr(sv, "ciao luca"));

    tok = sv_chop_right_untildelim(&sv, ' ');
    assert(sv_equals_cstr(tok, " luca"));
    assert(sv_equals_cstr(sv, "ciao"));

    tok = sv_chop_right_untildelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));
}

void test_chop_left_splitbydelim() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_splitbydelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_equals_cstr(sv, "luca come ti chiami"));

    tok = sv_chop_left_splitbydelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "come ti chiami"));

    tok = sv_chop_left_splitbydelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ti chiami"));

    tok = sv_chop_left_splitbydelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "chiami"));

    tok = sv_chop_left_splitbydelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));
}

void test_chop_right_splitbydelim() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_splitbydelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti"));

    tok = sv_chop_right_splitbydelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "ciao luca come"));

    tok = sv_chop_right_splitbydelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ciao luca"));

    tok = sv_chop_right_splitbydelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "ciao"));

    tok = sv_chop_right_splitbydelim(&sv, ' ');
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));
}

void test_chop_left_until() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_until(&sv, isspace);
    assert(sv_equals_cstr(tok, "ciao "));
    assert(sv_equals_cstr(sv, "luca come ti chiami"));

    tok = sv_chop_left_until(&sv, isspace);
    assert(sv_equals_cstr(tok, "luca "));
    assert(sv_equals_cstr(sv, "come ti chiami"));

    tok = sv_chop_left_until(&sv, isspace);
    assert(sv_equals_cstr(tok, "come "));
    assert(sv_equals_cstr(sv, "ti chiami"));

    tok = sv_chop_left_until(&sv, isspace);
    assert(sv_equals_cstr(tok, "ti "));
    assert(sv_equals_cstr(sv, "chiami"));

    tok = sv_chop_left_until(&sv, isspace);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));
}

void test_chop_right_until() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_until(&sv, isspace);
    assert(sv_equals_cstr(tok, " chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti"));

    tok = sv_chop_right_until(&sv, isspace);
    assert(sv_equals_cstr(tok, " ti"));
    assert(sv_equals_cstr(sv, "ciao luca come"));

    tok = sv_chop_right_until(&sv, isspace);
    assert(sv_equals_cstr(tok, " come"));
    assert(sv_equals_cstr(sv, "ciao luca"));

    tok = sv_chop_right_until(&sv, isspace);
    assert(sv_equals_cstr(tok, " luca"));
    assert(sv_equals_cstr(sv, "ciao"));

    tok = sv_chop_right_until(&sv, isspace);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));
}

void test_chop_left_splitby() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_splitby(&sv, isspace);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_equals_cstr(sv, "luca come ti chiami"));

    tok = sv_chop_left_splitby(&sv, isspace);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "come ti chiami"));

    tok = sv_chop_left_splitby(&sv, isspace);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ti chiami"));

    tok = sv_chop_left_splitby(&sv, isspace);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "chiami"));

    tok = sv_chop_left_splitby(&sv, isspace);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));
}

void test_chop_right_splitby() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_splitby(&sv, isspace);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti"));

    tok = sv_chop_right_splitby(&sv, isspace);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "ciao luca come"));

    tok = sv_chop_right_splitby(&sv, isspace);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ciao luca"));

    tok = sv_chop_right_splitby(&sv, isspace);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "ciao"));

    tok = sv_chop_right_splitby(&sv, isspace);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));
}

void test_chop_left_whilewith() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_whilewith(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "ciao "));
    assert(sv_equals_cstr(sv, "luca come ti chiami"));

    tok = sv_chop_left_whilewith(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "luca "));
    assert(sv_equals_cstr(sv, "come ti chiami"));

    tok = sv_chop_left_whilewith(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "come "));
    assert(sv_equals_cstr(sv, "ti chiami"));

    tok = sv_chop_left_whilewith(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "ti "));
    assert(sv_equals_cstr(sv, "chiami"));

    tok = sv_chop_left_whilewith(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));
}

void test_chop_right_whilewith() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_whilewith(&sv, isnotspace);
    assert(sv_equals_cstr(tok, " chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti"));

    tok = sv_chop_right_whilewith(&sv, isnotspace);
    assert(sv_equals_cstr(tok, " ti"));
    assert(sv_equals_cstr(sv, "ciao luca come"));

    tok = sv_chop_right_whilewith(&sv, isnotspace);
    assert(sv_equals_cstr(tok, " come"));
    assert(sv_equals_cstr(sv, "ciao luca"));

    tok = sv_chop_right_whilewith(&sv, isnotspace);
    assert(sv_equals_cstr(tok, " luca"));
    assert(sv_equals_cstr(sv, "ciao"));

    tok = sv_chop_right_whilewith(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));
}

void test_chop_left_while() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_while(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_equals_cstr(sv, "luca come ti chiami"));

    tok = sv_chop_left_while(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "come ti chiami"));

    tok = sv_chop_left_while(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ti chiami"));

    tok = sv_chop_left_while(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "chiami"));

    tok = sv_chop_left_while(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));
}

void test_chop_right_while() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_while(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti"));

    tok = sv_chop_right_while(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "ciao luca come"));

    tok = sv_chop_right_while(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ciao luca"));

    tok = sv_chop_right_while(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "ciao"));

    tok = sv_chop_right_while(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));
}

void test_chop_left_while_retain() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_while_retain(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_equals_cstr(sv, " luca come ti chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_while_retain(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, " come ti chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_while_retain(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, " ti chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_while_retain(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, " chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_while_retain(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));
}

void test_chop_right_while_retain() {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_while_retain(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_while_retain(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "ciao luca come "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_while_retain(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ciao luca "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_while_retain(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "ciao "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_while_retain(&sv, isnotspace);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));
}

int main(void) {
    test_find_delim();
    test_find_until();
    test_find_while();
    test_chop_left_untildelim();
    test_chop_right_untildelim();
    test_chop_left_splitbydelim();
    test_chop_right_splitbydelim();
    test_chop_left_until();
    test_chop_right_until();
    test_chop_left_splitby();
    test_chop_right_splitby();
    test_chop_left_whilewith();
    test_chop_right_whilewith();
    test_chop_left_while();
    test_chop_right_while();
    test_chop_left_while_retain();
    test_chop_right_while_retain();

    printf("All tests were successful!\n");
}