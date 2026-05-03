#ifndef _PARSER_H_
#define _PARSER_H_

#include "strings.h"

typedef enum {
    TYPE_STRING,
    TYPE_NUMBER,
    TYPE_BOOLEAN,
    TYPE_NULL,
    TYPE_OBJECT,
    TYPE_ARRAY
} Type_Kind;

#define json_malloc malloc

typedef struct Json_Node Json_Node;

typedef struct {
    String_View key;
    Json_Node *value;
} Json_Kvp;

// probably could me made opaque
struct Json_Node {
    unsigned int refcount;
    Type_Kind kind;

    union {
        double num;          // NUMBER
        int boolean;         // BOOLEAN
        String_View str;     // STRING

        // se è un array, ha una lista di Json_Node
        struct {
            size_t count;
            size_t capacity;
            Json_Node **items;     // array of Json_Node* (perché ogni Json_Node è allocato nell'heap separatamente)
        } array;

        // se è un object, ha una lista di Json_Kvp (coppie chiave stringa e valore Json_Node)
        struct {
            size_t count;
            size_t capacity;
            Json_Kvp *items;     // array of Json_Kvp (per non dover gestire allocazione dinamica, qui non è necessaria. Inoltre ho i dati contigui)
        } object;
    } data;
};

typedef enum {
    NOT_FREED,
    FREED
} node_status;

void json_acquire(Json_Node *node);
node_status json_release(Json_Node *node);

Json_Node *json_parse(String_View *json);

void json_print(Json_Node *node, int indent);

/* =============================== Navigazione =============================== */

Json_Node* json_object_get(Json_Node *object, const char *key);

Json_Node* json_array_get(Json_Node *array, size_t index);

double json_get_number(Json_Node *node);

#endif     // _PARSER_H_