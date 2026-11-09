#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

void *xmalloc(size_t size);
char *fread_all(const char *pathname);

#endif     // UTILS_H