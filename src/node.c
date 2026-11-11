#include "node.h"
#include "da.h"
#include "gioson.h"
#include "log.h"

#include <ctype.h>
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

        case TYPE_DOUBLE:
            printf("%g", node->data.doubleNum);
            break;

        case TYPE_INTEGER:
            printf("%lld", node->data.intNum);
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

    log_debug("Freeing node %p (kind %d)\n", node, node->kind);

    switch (node->kind) {
        case TYPE_OBJECT:
            // releases all subobjects
            da_foreach(Json_Kvp, kvp, &node->data.object) {
                json_release(kvp->value);
            }
            da_deinit(&node->data.object);
            break;

        case TYPE_ARRAY:
            // releases all items
            da_apply(&node->data.array, json_release);
            da_deinit(&node->data.object);
            break;

        case TYPE_STRING:
        case TYPE_DOUBLE:
        case TYPE_INTEGER:
        case TYPE_BOOLEAN:
        case TYPE_NULL:
        default:
            break;
    }

    free(node);
}

static Json_Node *new_node(Type_Kind kind) {
    Json_Node *node = JSON_MALLOC(sizeof(*node));
    node->kind = kind;
    node->refcount = 1;

    log_debug("Allocating new node %p (kind %d)\n", node, kind);

    return node;
}

/* ------------------------------- PUBLIC FUNCTIONS ------------------------------- */

Json_Node *new_string(String_View *str) {
    Json_Node *node = new_node(TYPE_STRING);
    node->data.str = *str;     // copies String_View
    return node;
}

Json_Node *new_double(double number) {
    Json_Node *node = new_node(TYPE_DOUBLE);
    node->data.doubleNum = number;
    return node;
}

Json_Node *new_integer(long long int number) {
    Json_Node *node = new_node(TYPE_INTEGER);
    node->data.intNum = number;
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
        log_warning("Tried to acquire NULL node\n");
        return;
    }

    node->refcount++;
}

void json_release(Json_Node *node) {
    if (!node) {
        log_warning("Tried to release NULL node\n");
        return;
    }

    if (node->refcount == 0) {
        log_warning("Tried to release a node with refcount 0\n");
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

double json_get_double(Json_Node *node) {
    if (!node || node->kind != TYPE_DOUBLE)
        return 0.0;
    return node->data.doubleNum;
}

long long int json_get_integer(Json_Node *node) {
    if (!node || node->kind != TYPE_INTEGER)
        return 0LL;
    return node->data.intNum;
}

int json_get_boolean(Json_Node *node) {
    if (!node || node->kind != TYPE_BOOLEAN)
        return 0;
    return node->data.boolean;
}

String_View json_get_string(Json_Node *node) {
    if (!node || node->kind != TYPE_STRING)
        return SV_NULL;
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
Json_Node *json_object_get(Json_Node *object, String_View key) {
    if (!object || object->kind != TYPE_OBJECT)
        return NULL;

    da_foreach(Json_Kvp, kvp, &object->data.object) {
        if (sv_equals(kvp->key, key))
            return kvp->value;
    }
    return NULL;
}

/**
 * Returns the indexed item from the array in the given indexPattern, otherwise returns the same array.
 * Assumes the given indexPattern has the form of subsequent indexing with square brackets, like this:
 *   indexPattern = "[1][7]...[12][42]"
 */
static Json_Node *json_getbyindex_if_any(Json_Node *arrayNode, String_View indexPattern) {
    // indexPattern = "[1][7]...[12][42]"
    switch (sv_skip_left_if(&indexPattern, '[')) {
        case SV_IS_EMPTY: return arrayNode;     // no more indexing, end of recursion
        case SV_NOT_FOUND: error("Expected '[', but found '%c'\n", indexPattern.data[0]); return NULL;
        case SV_OK: break;     // goes on
    }

    require(arrayNode->kind == TYPE_ARRAY, "Tried to use array indexing with non-array node\n");

    // SV_OK: chops the index, doing splitby ']'
    // indexPattern = "1][7]...[12][42]"
    String_View indexStr = sv_chop_left_splitbydelim(&indexPattern, ']');     // assumes ']' is present, otherwise UB
    // indexStr = "1", indexPattern = "[7]...[12][42]"

    // parses the index
    size_t index = (size_t)atoll(indexStr.data);

    Json_Array *arr = &arrayNode->data.array;
    require(index < arr->count, "Index out of bounds\n");

    // indexes the array and goes on recursively with next indexPattern if any
    return json_getbyindex_if_any(arr->items[index], indexPattern);
}

/**
 * Pattern is a sequence of [name] separated by '.', where 'name' can be
 * - a simple string name (name of the key in the current object)
 * - a simple name with one or more indexer (name of an array in the current object followed by one or more 0-indexing with square brackets [])
 *
 * e.g.: "object.array[1].multiarray[2][9][0].field"
 *
 * TODO: support wildcard '*'
 */
Json_Node *json_object_get_pattern(Json_Node *object, String_View pattern) {
    if (sv_empty(pattern)) return object;
    if (!object || object->kind != TYPE_OBJECT) return NULL;

    // pattern = "giovanni[1][4][7].other"
    String_View curr = sv_chop_left_while(&pattern, isalnum, SV_BOUND_KEEP);
    // curr = "giovanni", pattern = "[1][4][7].other"

    Json_Node *currNode = json_object_get(object, curr);
    if (currNode == NULL) return NULL;     // if not found, returns NULL and breaks the recursion

    String_View indexPattern = sv_chop_left_splitbydelim(&pattern, '.');
    // indexPattern = "[1][4][7]", pattern = "other"

    // indexes currNode with all the indexes in indexPattern (otherwise does nothing if indexPattern is empty)
    currNode = json_getbyindex_if_any(currNode, indexPattern);

    // recursively call json_object_get_pattern for the rest of the pattern
    return json_object_get_pattern(currNode, pattern);
}