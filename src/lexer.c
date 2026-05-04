#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "log.h"

/* =============================== LEXER =============================== */

void tok_print(Token *tok) {
    (void)tok;
    return;
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
            printf("\"" SV_FORMAT "\"", SV_ARGS(&(tok->data)));
            break;
        case TOK_NUMBER:
            printf(SV_FORMAT, SV_ARGS(&(tok->data)));
            break;
        case TOK_TRUE:
            printf(SV_FORMAT, SV_ARGS(&(tok->data)));
            break;
        case TOK_FALSE:
            printf(SV_FORMAT, SV_ARGS(&(tok->data)));
            break;
        case TOK_NULL:
            printf(SV_FORMAT, SV_ARGS(&(tok->data)));
            break;
        case TOK_EOF:
            printf("EOF");
            break;
    }
    printf("\n");
}

Token lex_next_token(String_View *json) {
    sv_skip_spaces_left(json);

    if (SV_EMPTY(json)) {
        tok_print(&TOK_EMPTY);     // DEBUG
        return TOK_EMPTY;
    }

    Token_Kind kind;
    String_View result;

    if (json->data[0] == '\"') {     // STRING
        // chops whole string
        // WARNING: for simplicity assumes that string doesn't contain the character "
        sv_skip_left(json, 1);                          // skips first "
        result = sv_chop_splitby_delim(json, '\"');     // discards last "
        kind = TOK_STRING;

    } else if (isdigit(json->data[0])) {     // NUMBER  // TODO: handle numbers starting with '-' or '.'
        // chops whole number
        result = sv_chop_left_while(json, isdigit);
        kind = TOK_NUMBER;

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