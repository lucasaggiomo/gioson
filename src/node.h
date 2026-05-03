#ifndef _NODE_H_
#define _NODE_H_

#include "da.h"
#include "gioson.h"
#include <stddef.h>

typedef struct {
    String_View key;
    Json_Node *value;
} Json_Kvp;

DA_DEFINE(Json_Node *) Json_Array;

DA_DEFINE(Json_Kvp) Json_Object;

struct Json_Node {
    unsigned int refcount;
    Type_Kind kind;

    union {
        double num;
        int boolean;
        String_View str;
        Json_Array array;
        Json_Object object;
    } data;
};

Json_Node *new_string(String_View *str);
Json_Node *new_number(double number);
Json_Node *new_boolean(int boolean);
Json_Node *new_null();
Json_Node *new_object();
Json_Node *new_array();

#endif     // _NODE_H_