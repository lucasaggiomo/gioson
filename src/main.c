#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "da.h"
#include "lexer.h"
#include "parser.h"
#include "strings.h"
#include "utils.h"

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

    log(LOG_INFO, "Opening file %s\n", jsonpath);

    char *log_kind = shift_args(&argc, &argv);
    CURRENT_LOG_KIND = log_kind_from_name(log_kind, LOG_DEFAULT_KIND);

    log(LOG_DEBUG, "Current log kind set to %s\n", log_kind_name[CURRENT_LOG_KIND]);

    // opens itself
    char *s = fread_all(jsonpath);

    String_View sv = SV(s);
    Json_Node *node = json_parse(&sv);

    json_print(node, 0);

    json_release(node);

    free(s);

    if (isatty(STDOUT_FILENO))
        printf("\n");

    return 0;
}