#include "lexer.h"

#include <ctype.h>
#include <string.h>

#include "da.h"
#include "log.h"

/* =============================== LEXER =============================== */

#ifdef _DEBUG
#include <stdio.h>
void tok_print(Token *tok) {
    switch (tok->kind) {
        case TOK_CURLY_OPEN:
            putchar('{');
            break;
        case TOK_CURLY_CLOSE:
            putchar('}');
            break;
        case TOK_SQUARE_OPEN:
            putchar('[');
            break;
        case TOK_SQUARE_CLOSE:
            putchar(']');
            break;
        case TOK_COLON:
            putchar(':');
            break;
        case TOK_COMMA:
            putchar(',');
            break;
        case TOK_STRING:
            printf("\"" SV_FORMAT "\"", SV_ARGS(&(tok->content)));
            break;
        case TOK_NUMBER:
            printf(SV_FORMAT, SV_ARGS(&(tok->content)));
            break;
        case TOK_TRUE:
            printf(SV_FORMAT, SV_ARGS(&(tok->content)));
            break;
        case TOK_FALSE:
            printf(SV_FORMAT, SV_ARGS(&(tok->content)));
            break;
        case TOK_NULL:
            printf(SV_FORMAT, SV_ARGS(&(tok->content)));
            break;
        case TOK_EOF:
            printf("EOF");
            break;
    }
    printf("\n");
}
#else
void tok_print(Token *tok) { (void)tok; }
#endif

static inline int issign(int c) { return c == '-' || c == '+'; }
static inline int isdot(int c) { return c == '.'; }
static inline int issignordot(int c) { return issign(c) || isdot(c); }

/// in regex, a floating number matches the pattern "[+-]?\d*\.?\d+"
/// i.e.: optional sign (single) -> any digits (zero or more) -> optional dot -> many digits (one or more)
/// NOTE: For simplicity, it's not yet supported standard notation with exponent 'e'.
static const SV_Pattern DOUBLE_PATTERN = SV_PATTERN(
    SVP_OPT(SVP_CHARS("+-")),       // sign
    SVP_ANY(SVP_PRED(isdigit)),     // integer part
    SVP_OPT(SVP_CHAR('.')),         // dot
    SVP_MANY(SVP_PRED(isdigit))     // decimal part
);

// in regex, a number matches the pattern "[+-]?\d+"
// i.e.: optional sign (single) -> many digits (one or more)
/// NOTE: For simplicity, it's not yet supported standard notation with exponent 'e'.
static const SV_Pattern INTEGER_PATTERN = SV_PATTERN(
    SVP_OPT(SVP_CHARS("+-")),       // sign
    SVP_MANY(SVP_PRED(isdigit))     // decimal part
);

Token lex_next_token(String_View *json) {
    sv_skip_spaces_left(json);

    if (sv_empty(*json)) {
        tok_print(&TOK_EMPTY);     // DEBUG
        return TOK_EMPTY;
    }

    Token_Kind kind;
    String_View result;

    if (json->data[0] == '\"') {     // STRING
        // chops whole string
        // WARNING: for simplicity assumes that string doesn't contain the character "
        sv_skip_left(json, 1);                              // skips first "
        result = sv_chop_left_splitbydelim(json, '\"');     // discards last "
        kind = TOK_STRING;

    } else if (isdigit(json->data[0]) || issignordot(json->data[0])) {     // DOUBLE | INTEGER
        // tries to match one of the number patterns

        SV_Patterns patterns = SV_PATTERNS(DOUBLE_PATTERN, INTEGER_PATTERN);
        size_t patternIndex = sv_chop_left_pattern_any(json, patterns, &result);
        require(patternIndex != SVP_NO_ONE_MATCHED, "Expected TOK_DOUBLE or TOK_INTEGER, but the patterns didn't match. Failed node of index %zu\n");

        kind = (patternIndex == 0)
            ? TOK_DOUBLE
            : TOK_INTEGER;

    } else if (strncmp(json->data, "true", 4) == 0) {
        result = sv_chop_left(json, 4);
        kind = TOK_TRUE;

    } else if (strncmp(json->data, "false", 5) == 0) {
        result = sv_chop_left(json, 5);
        kind = TOK_FALSE;

    } else if (strncmp(json->data, "null", 4) == 0) {
        result = sv_chop_left(json, 4);
        kind = TOK_NULL;

    } else {
        result = sv_chop_left(json, 1);
        switch (result.data[0]) {
            case '{':
                kind = TOK_CURLY_OPEN;
                break;
            case '}':
                kind = TOK_CURLY_CLOSE;
                break;
            case '[':
                kind = TOK_SQUARE_OPEN;
                break;
            case ']':
                kind = TOK_SQUARE_CLOSE;
                break;
            case ':':
                kind = TOK_COLON;
                break;
            case ',':
                kind = TOK_COMMA;
                break;
            case '\0':
                // actually not reachable because of previous SV_EMPTY check
                kind = TOK_EOF;
                break;
            default:
                error("Invalid character encountered: '%c'\n", result.data[0]);
        }
    }

    // DEBUG
    tok_print(&((Token) { kind, result }));

    return (Token) { kind, result };
}

Token lex_peek_token(String_View *json) {
    String_View copy = *json;
    return lex_next_token(&copy);
}