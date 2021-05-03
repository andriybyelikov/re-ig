#ifndef OBJECTS_ELEVATOR_H
#define OBJECTS_ELEVATOR_H

typedef void *elevator_t;

elevator_t new_elevator(void);

void elevator__init(elevator_t ref);
void elevator__proc(elevator_t ref, double deltatime);
void elevator__render(elevator_t ref);

float *elevator__y(elevator_t ref);

#endif
