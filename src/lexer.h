#ifndef _LEXER_H_
#define _LEXER_H_

#include "strings.h"

typedef enum {
    TOK_EOF,
    TOK_CURLY_OPEN,       // {
    TOK_CURLY_CLOSE,      // }
    TOK_SQUARE_OPEN,      // [
    TOK_SQUARE_CLOSE,     // ]
    TOK_COLON,            // :
    TOK_COMMA,            // ,
    TOK_STRING,
    TOK_NUMBER,
    TOK_TRUE,
    TOK_FALSE,
    TOK_NULL
} Token_Kind;

typedef struct {
    Token_Kind kind;
    String_View data;     // content of the token
} Token;

/// equivalent to `{0}`
#define TOK_EMPTY        \
    (Token) {            \
        .kind = TOK_EOF, \
        .data = { 0 }    \
    }

void tok_print(Token *tok);

/**
 * Gets the next token of json file from the given string view.
 * Such token is chopped from the json input.
 *
 * NOTE: assumes that tokens are separated by spaces (i.e. something for which isspace returns 1)
 */
Token lex_next_token(String_View *json);

/**
 * Gets the next token of json file from the given string view.
 * Such token is NOT chopped from the json input.
 * This is made by just calling lex_next_token on a local copy of the parameter json.
 *
 * NOTE: assumes that tokens are separated by spaces (i.e. something for which isspace returns 1)
 */
Token lex_peek_token(String_View *json);

#endif		// _LEXER_H_