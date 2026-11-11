#ifndef GIOSON_H
#define GIOSON_H

/* =============================== PUBLIC API OF GIOSON =============================== */

#include "sv.h"

typedef enum {
    TYPE_UNKNOWN,
    TYPE_STRING,
    TYPE_DOUBLE,
    TYPE_INTEGER,
    TYPE_BOOLEAN,
    TYPE_NULL,
    TYPE_OBJECT,
    TYPE_ARRAY
} Type_Kind;

#ifndef JSON_MALLOC
#include <stdlib.h>
#define JSON_MALLOC malloc
#endif

typedef struct Json_Node Json_Node;

Json_Node *json_parse(String_View *json);
void json_print(Json_Node *node, int indent);

void json_acquire(Json_Node *node);
void json_release(Json_Node *node);

Type_Kind json_get_type(Json_Node *node);

double json_get_double(Json_Node *node);
long long int json_get_integer(Json_Node *node);
int json_get_boolean(Json_Node *node);
String_View json_get_string(Json_Node *node);

Json_Node *json_array_get(Json_Node *array, size_t index);
size_t json_array_size(Json_Node *array);

/** Returns the value of the entry with given key in given object or NULL if key was not found. */
Json_Node *json_object_get(Json_Node *object, String_View key);

/** Returns the value of the entry found with the given pattern string in given object or NULL if pattern didn't match anything. */
Json_Node *json_object_get_pattern(Json_Node *object, String_View pattern);

#endif     // GIOSON_H