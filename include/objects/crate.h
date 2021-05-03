#ifndef OBJECTS_CRATE_H
#define OBJECTS_CRATE_H

#include "objects/vec3f.h"

typedef void *crate_t;

crate_t new_crate(void);
vec3f_t *crate__pos(crate_t ref);
float *crate__y_angle(crate_t ref);
int crate__is_point_inside(crate_t ref, float x, float y, float z);
void crate__render(void);

#endif
