#include "log.h"

#include <string.h>

const char *log_kind_name[LOG_KIND_COUNT] = {
#define X(NAME, name, stream) [LOG_KIND(NAME)] = LOG_KIND_STR(NAME),
    LIST_OF_LOGS
#undef X
};

Log_Kind log_kind_from_name(const char *logname, Log_Kind def) {
    if (!logname)
        return def;
    size_t len = strlen(logname);
    (void)len;     // to hide warning
#define X(name, stream)                    \
    if (strncmp(#name, logname, len) == 0) \
        return LOG_KIND(name);             \
    LIST_OF_LOGS
#undef X
    return def;
}

Log_Kind CURRENT_LOG_KIND = LOG_DEBUG;