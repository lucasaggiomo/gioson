#ifndef _GIOSON_H_
#define _GIOSON_H_

/* =============================== PUBLIC API OF GIOSON =============================== */

#include "strings.h"

typedef enum {
    NOT_FREED,
    FREED
} node_status;

typedef enum {
    TYPE_UNKNOWN,
    TYPE_STRING,
    TYPE_NUMBER,
    TYPE_BOOLEAN,
    TYPE_NULL,
    TYPE_OBJECT,
    TYPE_ARRAY
} Type_Kind;

#define json_malloc malloc

typedef struct Json_Node Json_Node;

Json_Node *json_parse(String_View *json);
void json_print(Json_Node *node, int indent);

void json_acquire(Json_Node *node);
void json_release(Json_Node *node);

Type_Kind json_get_type(Json_Node *node);

double json_get_number(Json_Node *node);
int json_get_boolean(Json_Node *node);
String_View json_get_string(Json_Node *node);

Json_Node *json_array_get(Json_Node *array, size_t index);
size_t json_array_size(Json_Node *array);

/** Returns the value of the entry with given key in given object or NULL if key was not found. */
Json_Node *json_object_get_sv(Json_Node *object, String_View *sv_key);
Json_Node *json_object_get(Json_Node *object, const char *key);

#endif     // _GIOSON_H_