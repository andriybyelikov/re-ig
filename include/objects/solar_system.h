#ifndef OBJECTS_SOLAR_SYSTEM_H
#define OBJECTS_SOLAR_SYSTEM_H

#include <math.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

typedef void *solar_system_t;

solar_system_t new_solar_system(void);
void solar_system__init(solar_system_t ref);
void solar_system__proc(solar_system_t ref);
void solar_system__render(solar_system_t ref);

#endif
