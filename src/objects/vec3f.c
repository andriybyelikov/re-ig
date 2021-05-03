#include "objects/vec3f.h"

#include <stdlib.h>

vec3f_t malloc_vec3f(void)
{
    return malloc(3 * sizeof(float));
}

void free_vec3f(vec3f_t ref)
{
    free(ref);
}

float *vec3f__x(vec3f_t ref)
{
    return (float *)ref + 0;
}

float *vec3f__y(vec3f_t ref)
{
    return (float *)ref + 1;
}

float *vec3f__z(vec3f_t ref)
{
    return (float *)ref + 2;
}
