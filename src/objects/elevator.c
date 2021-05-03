#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "objects/elevator.h"

static const float await_time = 2.0f;

struct elevator {
    float y;
    int dir;
    float await;
};

elevator_t new_elevator(void)
{
    struct elevator *obj = malloc(sizeof(struct elevator));
}

void elevator__init(elevator_t ref)
{
    struct elevator *a = ref;

    a->y = 2.0f;
    a->dir = 1; // 1 = up, -1 = down
    a->await = await_time;
}

void elevator__proc(elevator_t ref, double deltatime)
{
    struct elevator *a = ref;

    if (a->await > 0.0f) {
        a->await -= deltatime;
        if (a->await < 0.0f) {
            a->await = 0.0f;
        }
    } else {
        a->y += a->dir * deltatime;
        if (a->dir > 0) {
            if (a->y > 2.0f) {
                a->y = 2.0f;
                a->await = await_time;
                a->dir = -1;
            }
        } else { // a->dir < 0
            if (a->y < 0.0f) {
                a->y = 0.0f;
                a->await = await_time;
                a->dir = 1;
            }
        }
    }
}

static
void render_quad(void)
{
    float vtx[2][3][2] = {
        {
            { 1.0f, 1.0f },
            { 0.0f, 0.0f },
            { 0.0f, 1.0f }
        },
        {
            { 1.0f, 1.0f },
            { 0.0f, 0.0f },
            { 1.0f, 0.0f }
        }
    };
    int nseg = 4;
    float segl = 1.0f / nseg;
    int ti = 6;
    glBegin(GL_TRIANGLES);
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 3; j++) {
                int x = ti % nseg;
                int y = ti / nseg;
                glNormal3f(0.0f, 0.0f, 1.0f);
                glTexCoord2f(x * segl + vtx[i][j][0] * segl,
                    1 - (y * segl + vtx[i][j][1] * segl));
                glVertex2f(vtx[i][j][0], vtx[i][j][1]);
            }
        }
    glEnd();
}

void elevator__render(elevator_t ref)
{
    glPushMatrix();
        glTranslatef(0.0f, -2.0f, 1.0f);
        glScalef(1.0f, 2.0f, 1.0f);
        render_quad();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 1.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        render_quad();
    glPopMatrix();
}

float *elevator__y(elevator_t ref)
{
    struct elevator *a = ref;

    return &a->y;
}
