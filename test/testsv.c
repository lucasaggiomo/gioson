#include "sv.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

static inline int isnotspace(int c) { return !isspace(c); }

void test_find_delim(void) {
    String_View sv = SV("ciao luca come ti chiami");
    assert(sv_find_left_delim(sv, ' ') == 4);
    assert(sv_find_right_delim(sv, ' ') == 17);
}

void test_find_until(void) {
    String_View sv = SV("ciao luca come ti chiami");
    assert(sv_find_left_until(sv, isspace) == 4);
    assert(sv_find_right_until(sv, isspace) == 17);
}

void test_find_while(void) {
    String_View sv = SV("ciao luca come ti chiami");
    assert(sv_find_left_while(sv, isnotspace) == 4);
    assert(sv_find_right_while(sv, isnotspace) == 17);
}

/* =============================== DELIM =============================== */

void test_chop_left_delim_chop(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "ciao "));
    assert(sv_equals_cstr(sv, "luca come ti chiami"));

    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "luca "));
    assert(sv_equals_cstr(sv, "come ti chiami"));

    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "come "));
    assert(sv_equals_cstr(sv, "ti chiami"));

    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "ti "));
    assert(sv_equals_cstr(sv, "chiami"));

    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " "));
    assert(sv_empty(sv));

    sv = SV("  ");
    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " "));
    assert(sv_equals_cstr(sv, " "));

    sv = SV_NULL;
    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_right_delim_chop(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti"));

    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " ti"));
    assert(sv_equals_cstr(sv, "ciao luca come"));

    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " come"));
    assert(sv_equals_cstr(sv, "ciao luca"));

    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " luca"));
    assert(sv_equals_cstr(sv, "ciao"));

    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " "));
    assert(sv_empty(sv));

    sv = SV("  ");
    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " "));
    assert(sv_equals_cstr(sv, " "));

    sv = SV_NULL;
    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_CHOP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_left_delim_drop(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_equals_cstr(sv, "luca come ti chiami"));

    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "come ti chiami"));

    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ti chiami"));

    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "chiami"));

    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));

    sv = SV("  ");
    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, " "));

    sv = SV_NULL;
    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_right_delim_drop(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti"));

    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "ciao luca come"));

    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ciao luca"));

    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "ciao"));

    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));

    sv = SV("  ");
    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, " "));

    sv = SV_NULL;
    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_left_delim_keep(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_equals_cstr(sv, " luca come ti chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, " come ti chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, " ti chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, " chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, " "));

    sv = SV("  ");
    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, "  "));

    sv = SV_NULL;
    tok = sv_chop_left_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_right_delim_keep(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "ciao luca come "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ciao luca "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "ciao "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, " "));

    sv = SV("  ");
    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, "  "));

    sv = SV_NULL;
    tok = sv_chop_right_delim(&sv, ' ', SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

/* =============================== UNTIL =============================== */

void test_chop_left_until_chop(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "ciao "));
    assert(sv_equals_cstr(sv, "luca come ti chiami"));

    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "luca "));
    assert(sv_equals_cstr(sv, "come ti chiami"));

    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "come "));
    assert(sv_equals_cstr(sv, "ti chiami"));

    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "ti "));
    assert(sv_equals_cstr(sv, "chiami"));

    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " "));
    assert(sv_empty(sv));

    sv = SV("  ");
    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " "));
    assert(sv_equals_cstr(sv, " "));

    sv = SV_NULL;
    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_right_until_chop(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti"));

    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " ti"));
    assert(sv_equals_cstr(sv, "ciao luca come"));

    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " come"));
    assert(sv_equals_cstr(sv, "ciao luca"));

    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " luca"));
    assert(sv_equals_cstr(sv, "ciao"));

    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " "));
    assert(sv_empty(sv));

    sv = SV("  ");
    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " "));
    assert(sv_equals_cstr(sv, " "));

    sv = SV_NULL;
    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_CHOP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_left_until_drop(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_equals_cstr(sv, "luca come ti chiami"));

    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "come ti chiami"));

    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ti chiami"));

    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "chiami"));

    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));

    sv = SV("  ");
    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, " "));

    sv = SV_NULL;
    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_right_until_drop(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti"));

    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "ciao luca come"));

    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ciao luca"));

    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "ciao"));

    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));

    sv = SV("  ");
    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, " "));

    sv = SV_NULL;
    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_left_until_keep(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_equals_cstr(sv, " luca come ti chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, " come ti chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, " ti chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, " chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, " "));

    sv = SV("  ");
    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, "  "));

    sv = SV_NULL;
    tok = sv_chop_left_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_right_until_keep(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "ciao luca come "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ciao luca "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "ciao "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, " "));

    sv = SV("  ");
    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, "  "));

    sv = SV_NULL;
    tok = sv_chop_right_until(&sv, isspace, SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

/* =============================== WHILE =============================== */

void test_chop_left_while_chop(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "ciao "));
    assert(sv_equals_cstr(sv, "luca come ti chiami"));

    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "luca "));
    assert(sv_equals_cstr(sv, "come ti chiami"));

    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "come "));
    assert(sv_equals_cstr(sv, "ti chiami"));

    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "ti "));
    assert(sv_equals_cstr(sv, "chiami"));

    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " "));
    assert(sv_empty(sv));

    sv = SV("  ");
    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " "));
    assert(sv_equals_cstr(sv, " "));

    sv = SV_NULL;
    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_right_while_chop(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti"));

    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " ti"));
    assert(sv_equals_cstr(sv, "ciao luca come"));

    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " come"));
    assert(sv_equals_cstr(sv, "ciao luca"));

    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " luca"));
    assert(sv_equals_cstr(sv, "ciao"));

    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " "));
    assert(sv_empty(sv));

    sv = SV("  ");
    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_equals_cstr(tok, " "));
    assert(sv_equals_cstr(sv, " "));

    sv = SV_NULL;
    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_CHOP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_left_while_drop(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_equals_cstr(sv, "luca come ti chiami"));

    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "come ti chiami"));

    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ti chiami"));

    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "chiami"));

    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));

    sv = SV("  ");
    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, " "));

    sv = SV_NULL;
    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_right_while_drop(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti"));

    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "ciao luca come"));

    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ciao luca"));

    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "ciao"));

    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));

    sv = SV("  ");
    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, " "));

    sv = SV_NULL;
    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_DROP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_left_while_keep(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_equals_cstr(sv, " luca come ti chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, " come ti chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, " ti chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, " chiami"));
    sv_skip_left(&sv, 1);

    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, " "));

    sv = SV("  ");
    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, "  "));

    sv = SV_NULL;
    tok = sv_chop_left_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

void test_chop_right_while_keep(void) {
    String_View sv = SV("ciao luca come ti chiami");
    String_View tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "chiami"));
    assert(sv_equals_cstr(sv, "ciao luca come ti "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "ti"));
    assert(sv_equals_cstr(sv, "ciao luca come "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "come"));
    assert(sv_equals_cstr(sv, "ciao luca "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "luca"));
    assert(sv_equals_cstr(sv, "ciao "));
    sv_skip_right(&sv, 1);

    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_equals_cstr(tok, "ciao"));
    assert(sv_empty(sv));

    sv = SV(" ");
    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, " "));

    sv = SV("  ");
    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_equals_cstr(sv, "  "));

    sv = SV_NULL;
    tok = sv_chop_right_while(&sv, isnotspace, SV_BOUND_KEEP);
    assert(sv_empty(tok));
    assert(sv_empty(sv));
}

int main(void) {
    test_find_delim();
    test_find_until();
    test_find_while();

    test_chop_left_delim_chop();
    test_chop_right_delim_chop();
    test_chop_left_delim_drop();
    test_chop_right_delim_drop();
    test_chop_left_delim_keep();
    test_chop_right_delim_keep();

    test_chop_left_until_chop();
    test_chop_right_until_chop();
    test_chop_left_until_drop();
    test_chop_right_until_drop();
    test_chop_left_until_keep();
    test_chop_right_until_keep();

    test_chop_left_while_chop();
    test_chop_right_while_chop();
    test_chop_left_while_drop();
    test_chop_right_while_drop();
    test_chop_left_while_keep();
    test_chop_right_while_keep();

    printf("All tests were successful!\n");
    return 0;
}