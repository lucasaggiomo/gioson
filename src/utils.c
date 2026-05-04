#include "utils.h"
#include "log.h"

#include <string.h>

void *xmalloc(size_t size) {
    void *out = malloc(size);
    if (!out)
        error("Malloc failed\n");
    return out;
}

char *fread_all(const char *pathname) {
    // doesn't check errors for the sake of simplicity

    FILE *f = fopen(pathname, "rb");
    p_require(f, "Error opening file '%s'\n", pathname);

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