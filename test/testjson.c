#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../inc/da.h"
#include "../inc/gioson.h"
#include "../inc/log.h"
#include "../inc/sv.h"
#include "../inc/utils.h"

/**
Example:

{
    "key1": "value",
    "key2": 123,
    "key3": {
        "key4": true,
        "key5": [
            42,
            "ciao",
            true,
            null,
            [1, 2, 3],
            {"id": 99}
        ]
    }
}
 */

extern Log_Kind CURRENT_LOG_KIND;
#define LOG_DEFAULT_KIND LOG_INFO

/* =============================== MAIN =============================== */

char *shift_args(int *argc, char **argv[]) {
    if (*argc == 0)
        return NULL;

    (*argc)--;
    char *output = (*argv)[0];
    (*argv)++;

    return output;
}

int main(int argc, char *argv[]) {
    char *program = shift_args(&argc, &argv);

    char *jsonpath = shift_args(&argc, &argv);
    require(jsonpath, "\nUsage: %s\n\t<json-path>\n\t[<log-level among {DEBUG, INFO, ERROR}, default %s>]\n", program, log_kind_name[LOG_DEFAULT_KIND]);

    log_info("Opening file %s\n", jsonpath);

    char *log_kind = shift_args(&argc, &argv);
    CURRENT_LOG_KIND = log_kind_from_name(log_kind, LOG_DEFAULT_KIND);

    log_debug("Current log kind set to %s\n", log_kind_name[CURRENT_LOG_KIND]);

    // opens itself
    char *filecontent = fread_all(jsonpath);

    String_View sv = SV(filecontent);
    Json_Node *node = json_parse(&sv);

    json_print(node, 0);

    // navigazione albero
    Type_Kind type = json_get_type(node);

    require(type == TYPE_OBJECT, "Expected object type\n");

    String_View key = SV_LITERAL("key3.key5[4][1]");
    Json_Node *key_value = json_object_get_pattern(node, key);
    require(key_value != NULL, "'" SV_FORMAT "' not found!\n", SV_ARGS(&key));

    json_acquire(key_value);
    json_release(node);

    // now key_value should not have been deallocated
    require(json_get_type(key_value) == TYPE_NUMBER, "Expected object type\n");
    double number = json_get_number(key_value);
    printf("key value is: %f\n", number);

    // String_View key_content = json_get_string(key_value);
    // printf("key value is: " SV_FORMAT "\n", SV_ARGS(&key_content));

    json_release(key_value);

    free(filecontent);

    if (isatty(STDOUT_FILENO))
        printf("\n");

    return 0;
}