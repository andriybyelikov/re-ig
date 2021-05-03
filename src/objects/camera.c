#include "objects/camera.h"
#include "objects/vec3f.h"
#include <stdlib.h>

struct camera {
    vec3f_t pos;
    float azimuthal_angle;
    float polar_angle;
};

camera_t new_camera(void)
{
    struct camera *ref = malloc(sizeof(struct camera));
    ref->pos = malloc_vec3f();
    return ref;
}

void free_camera(camera_t ref)
{
    struct camera *a = ref;

    free(a->pos);
    free(a);
}

vec3f_t *camera__pos(camera_t ref)
{
    struct camera *a = ref;

    return &a->pos;
}

float *camera__azimuthal_angle(camera_t ref)
{
    struct camera *a = ref;

    return &a->azimuthal_angle;
}

float *camera__polar_angle(camera_t ref)
{
    struct camera *a = ref;

    return &a->polar_angle;
}
