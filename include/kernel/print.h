#ifndef PRINT_H
#define PRINT_H

#include <stddef.h>

int snprintf(char *buf, size_t size, const char *fmt, ...);
int kprintf(const char *fmt, ...);

#endif
