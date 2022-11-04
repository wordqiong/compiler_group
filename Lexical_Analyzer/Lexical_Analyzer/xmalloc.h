#pragma once
#include <sys/types.h>
#include <iostream>
void* xmalloc(size_t size)
{
    register void* value = malloc(size);
    if (value == 0)
        exit(2);
    return value;
}