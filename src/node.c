#include "node.h"

#include "da.h"
#include "utils.h"

#include "gioson.h"
#include <stdlib.h>
#include <string.h>

#define print_indent(indent)               \
    do {                                   \
        for (int i = 0; i < indent; i++) { \
            printf("\t");                  \
        }                                  \
    } while (0)

/* ------------------------------- PRIVATE FUNCTIONS ------------------------------- */

static void json_recursive_print(Json_Node *node, int indent) {
    if (!node)
        return;

    printf("[%d] ", node->refcount);
    switch (node->kind) {
        case TYPE_UNKNOWN:
            printf("UNKNOWN");
            break;

        case TYPE_STRING:
            printf("\"" SV_FORMAT "\"", SV_ARGS(&node->data.str));
            break;

        case TYPE_NUMBER:
            printf("%g", node->data.num);
            break;

        case TYPE_BOOLEAN:
            printf("%s", node->data.boolean ? "true" : "false");
            break;

        case TYPE_NULL:
            printf("null");
            break;

        case TYPE_OBJECT:
            printf("{");
            if (!da_empty(&node->data.object)) {
                printf("\n");
                for (size_t i = 0; i < node->data.object.count; i++) {
                    Json_Kvp kvp = node->data.object.items[i];

                    print_indent(indent + 1);

                    printf("\"" SV_FORMAT "\": ", SV_ARGS(&kvp.key));
                    json_recursive_print(kvp.value, indent + 1);

                    if (i < node->data.object.count - 1)
                        printf(",");
                    printf("\n");
                }
                print_indent(indent);
            }
            printf("}");
            break;

        case TYPE_ARRAY:
            printf("[");
            if (!da_empty(&node->data.array)) {
                printf("\n");
                for (size_t i = 0; i < node->data.array.count; i++) {
                    Json_Node *item = node->data.array.items[i];

                    print_indent(indent + 1);
                    json_recursive_print(item, indent + 1);
                    if (i < node->data.array.count - 1)
                        printf(",");
                    printf("\n");
                }
                print_indent(indent);
            }
            printf("]");
            break;
    }
}

static void free_node(Json_Node *node) {
    if (!node)
        return;

    log(LOG_DEBUG, "Freeing node %p (kind %d)\n", node, node->kind);

    switch (node->kind) {
        case TYPE_OBJECT:
            // releases all subobjects
            da_foreach(Json_Kvp, kvp, &node->data.object) {
                json_release(kvp->value);
            }
            da_free(&(node->data.object));
            break;

        case TYPE_ARRAY:
            // releases all items
            da_apply(&node->data.array, json_release);
            da_free(&node->data.object);
            break;

        case TYPE_STRING:
        case TYPE_NUMBER:
        case TYPE_BOOLEAN:
        case TYPE_NULL:
        default:
            break;
    }

    free(node);
}

static Json_Node *new_node(Type_Kind kind) {
    Json_Node *node = json_malloc(sizeof(*node));
    node->kind = kind;
    node->refcount = 1;

    log(LOG_DEBUG, "Allocating new node %p (kind %d)\n", node, kind);

    return node;
}

/* ------------------------------- PUBLIC FUNCTIONS ------------------------------- */

Json_Node *new_string(String_View *str) {
    Json_Node *node = new_node(TYPE_STRING);
    node->data.str = *str;     // copies String_View
    return node;
}

Json_Node *new_number(double number) {
    Json_Node *node = new_node(TYPE_NUMBER);
    node->data.num = number;
    return node;
}

Json_Node *new_boolean(int boolean) {
    Json_Node *node = new_node(TYPE_BOOLEAN);
    node->data.boolean = (boolean == 1);
    return node;
}

Json_Node *new_null() {
    Json_Node *node = new_node(TYPE_NULL);
    memset(&(node->data), 0, sizeof(node->data));     // in teoria è inutile
    return node;
}

Json_Node *new_object() {
    Json_Node *node = new_node(TYPE_OBJECT);
    da_init(&(node->data.object));
    return node;
}

Json_Node *new_array() {
    Json_Node *node = new_node(TYPE_ARRAY);
    da_init(&(node->data.array));
    return node;
}

void json_print(Json_Node *node, int indent) {
    json_recursive_print(node, indent);
    printf("\n");
}

void json_acquire(Json_Node *node) {
    if (!node) {
        log(LOG_WARNING, "Tried to acquire NULL node");
        return;
    }

    node->refcount++;
}

void json_release(Json_Node *node) {
    if (!node) {
        log(LOG_WARNING, "Tried to release NULL node");
        return;
    }

    if (node->refcount == 0) {
        log(LOG_WARNING, "Tried to release a node with refcount 0");
        return;
    }

    node->refcount--;
    if (node->refcount == 0) {
        free_node(node);
    }
}

Type_Kind json_get_type(Json_Node *node) {
    if (!node)
        return TYPE_UNKNOWN;
    return node->kind;
}

double json_get_number(Json_Node *node) {
    if (!node || node->kind != TYPE_NUMBER)
        return 0.0;
    return node->data.num;
}

int json_get_boolean(Json_Node *node) {
    if (!node || node->kind != TYPE_BOOLEAN)
        return 0;
    return node->data.boolean;
}

String_View json_get_string(Json_Node *node) {
    if (!node || node->kind != TYPE_STRING)
        return (String_View) { NULL, 0 };
    return node->data.str;
}

Json_Node *json_array_get(Json_Node *array, size_t index) {
    if (!array || array->kind != TYPE_ARRAY)
        return NULL;
    return da_get_or_default(&array->data.array, index, NULL);
}

size_t json_array_size(Json_Node *array) {
    if (!array || array->kind != TYPE_ARRAY)
        return 0;
    return array->data.array.count;
}

// O(n) implementation
Json_Node *json_object_get_sv(Json_Node *object, String_View *sv_key) {
    if (!object || object->kind != TYPE_OBJECT)
        return NULL;

    da_foreach(Json_Kvp, kvp, &object->data.object) {
        if (SV_EQUALS(&kvp->key, sv_key))
            return kvp->value;
    }
    return NULL;
}

// O(n) implementation
Json_Node *json_object_get(Json_Node *object, const char *key) {
    if (!object || object->kind != TYPE_OBJECT)
        return NULL;

    da_foreach(Json_Kvp, kvp, &object->data.object) {
        if (SV_EQUALS_CSTR(&kvp->key, key))
            return kvp->value;
    }
    return NULL;
}
