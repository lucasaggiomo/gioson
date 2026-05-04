#include "lexer.h"
#include "log.h"
#include "node.h"

#include "da.h"
#include <stdlib.h>
#include <string.h>

/** `{ something }`
 * parses next tokens to find 'something'
 * what could 'something' be?
 *
 * 1. empty             `{}`
 *    Then the next tokens must be:
 *      TOK_CURLY_CLOSE
 *
 * 2. a Json_Kvp:       `{ "key": Json_Node }`
 *    Then the next tokens must be:
 *      TOK_STRING(key) -> TOK_COLON -> json_parse -> TOK_CURLY_CLOSE
 *
 * 3. (Generalized version of case 2)
 *    A list of Json_Kvps:       `{ "key1": Json_Node, "key2": Json_Node, ..., "keyn": Json_Node }`
 *    Then the next tokens must be:
 *      TOK_STRING(key1) -> TOK_COLON -> json_parse -> TOK_COMMA ->
 *      TOK_STRING(key2) -> TOK_COLON -> json_parse -> TOK_COMMA ->
 *      ...
 *      TOK_STRING(keyn) -> TOK_COLON -> parse_node -> TOK_CURLY_CLOSE
 *
 * In all cases, everything ends if TOK_CURLY_CLOSE is found and a new node of kind TYPE_OBJECT is parsed.
 * Il nodo restituito ha refcount 1 e anche i figli.
 */
static Json_Node *parse_object(String_View *json) {
    Json_Node *obj = new_object();

    if (lex_peek_token(json).kind == TOK_CURLY_CLOSE) {
        // case 1
        lex_next_token(json);
        return obj;
    }

    // case 2-3
    Token tok;
    do {
        // NOTE: first token (string key) has already been found
        tok = lex_next_token(json);
        require(tok.kind == TOK_STRING, "Expected key but found token of kind %d\n", tok.kind);

        Json_Kvp kvp = { .key = tok.data };

        tok = lex_next_token(json);
        require(tok.kind == TOK_COLON, "Expected colon but found token of kind %d\n", tok.kind);

        Json_Node *value = json_parse(json);
        require(value != NULL, "Expected value but found NULL\n");

        kvp.value = value;
        da_append(&obj->data.object, kvp);

        tok = lex_next_token(json);
        require(tok.kind == TOK_CURLY_CLOSE || tok.kind == TOK_COMMA,
                "Expected '}' or comma but found token of kind %d\n", tok.kind);

    } while (tok.kind == TOK_COMMA);

    return obj;
}

/**
 * 1. []
 *    tokens: TOK_SQUARE_CLOSE
 * 2. [ literals, objects, arrays (ordine arbitrario) ] -> in pratica lista di Json_Node separati da virgole
 *    tokens:
 *      Json_Node -> TOK_COMMA -> ... -> TOK_COMMA -> Json_Node -> TOK_SQUARE_CLOSE
 *
 * Il nodo restituito ha refcount 1 e anche gli elementi dell'array.
 */
static Json_Node *parse_array(String_View *json) {
    Json_Node *arr = new_array();

    if (lex_peek_token(json).kind == TOK_SQUARE_CLOSE) {
        lex_next_token(json);
        return arr;
    }

    // here token is not chopped from json (because part of next node)
    Token tok;
    do {
        Json_Node *node = json_parse(json);
        require(node != NULL, "Expected JSON object, array or literal but found NULL\n");
        da_append(&arr->data.array, node);

        tok = lex_next_token(json);
        require(tok.kind == TOK_SQUARE_CLOSE || tok.kind == TOK_COMMA,
                "Expected ']' or comma but found token of kind %d\n", tok.kind);
    } while (tok.kind == TOK_COMMA);

    return arr;
}

/* =============================== PARSER =============================== */

/** Il nodo restituito ha valore refcount 1. */
Json_Node *json_parse(String_View *json) {
    Token tok = lex_next_token(json);
    switch (tok.kind) {
        case TOK_EOF:
            return NULL;

        case TOK_CURLY_OPEN:
            return parse_object(json);

        case TOK_SQUARE_OPEN:
            return parse_array(json);

        case TOK_STRING:
            return new_string(&tok.data);

        case TOK_NUMBER:
            double num = atof(tok.data.data);     // uses atof for simplicity, assuming no errors
            return new_number(num);

        case TOK_TRUE:
            return new_boolean(1);

        case TOK_FALSE:
            return new_boolean(0);

        case TOK_NULL:
            return new_null();

        case TOK_CURLY_CLOSE:
        case TOK_SQUARE_CLOSE:
        case TOK_COLON:
        case TOK_COMMA:
        default:
            error("Expected a JSON object, array or literal, but found token of kind %d\n", tok.kind);
    }
}