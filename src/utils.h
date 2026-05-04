#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <stdlib.h>

void *xmalloc(size_t size);
char *fread_all(const char *pathname);

#endif     // _UTILS_H_