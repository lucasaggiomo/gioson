#ifndef NODE_H
#define NODE_H

#include "da.h"
#include "gioson.h"
#include <stddef.h>

typedef struct {
    String_View key;
    Json_Node *value;
} Json_Kvp;

DA_DEFINE(Json_Node *)
Json_Array;

DA_DEFINE(Json_Kvp)
Json_Object;

struct Json_Node {
    unsigned int refcount;
    Type_Kind kind;

    union {
        double doubleNum;
        long long int intNum;
        int boolean;
        String_View str;
        Json_Array array;
        Json_Object object;
    } data;
};

Json_Node *new_string(String_View *str);
Json_Node *new_double(double number);
Json_Node *new_integer(long long int number);
Json_Node *new_boolean(int boolean);
Json_Node *new_null();
Json_Node *new_object();
Json_Node *new_array();

#endif     // NODE_H