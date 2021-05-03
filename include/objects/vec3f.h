#ifndef OBJECTS_VEC3F_H
#define OBJECTS_VEC3F_H

typedef void *vec3f_t;

vec3f_t malloc_vec3f(void);
void free_vec3f(vec3f_t ref);

float *vec3f__x(vec3f_t ref);
float *vec3f__y(vec3f_t ref);
float *vec3f__z(vec3f_t ref);

#endif
