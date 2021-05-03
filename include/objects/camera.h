#ifndef OBJECTS_CAMERA_H
#define OBJECTS_CAMERA_H

#include "objects/vec3f.h"

typedef void *camera_t;

camera_t new_camera(void);
void free_camera(camera_t ref);

vec3f_t *camera__pos(camera_t ref);
float *camera__azimuthal_angle(camera_t ref);
float *camera__polar_angle(camera_t ref);

#endif
