#include "utils.h"

#include <string.h>

Log_Kind CURRENT_LOG_KIND = LOG_INFO;

const char *log_kind_name[] = { "DEBUG", "INFO", "ERROR" };

FILE *get_log_stream(Log_Kind kind) {
    switch (kind) {
        case LOG_INFO:
        case LOG_DEBUG:
            return stdout;
        case LOG_ERROR:
            return stderr;
        default:
            return stderr;
    }
}

Log_Kind log_kind_from_name(const char *name, Log_Kind default_kind) {
    if (!name)
        return default_kind;
        
    for (int i = 0; i < LOG_KIND_COUNT; i++) {
        if (strcmp(name, log_kind_name[i]) == 0)
            return (Log_Kind)i;
    }
    return default_kind;
}

void *xmalloc(size_t size) {
    void *out = malloc(size);
    if (!out)
        error("Malloc failed\n");
    return out;
}

char *fread_all(const char *pathname) {
    // doesn't check errors for the sake of simplicity

    FILE *f = fopen(pathname, "rb");

    // gets size of file
    fseek(f, 0, SEEK_END);
    size_t filesize = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = xmalloc(filesize + 1);     // 1 for null terminator
    size_t nread = fread(buffer, 1, filesize, f);
    if (nread != filesize) {
        perror("Error occurred\n");
    }

    fclose(f);

    return buffer;
}