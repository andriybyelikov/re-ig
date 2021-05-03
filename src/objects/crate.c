#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "objects/crate.h"
#include <math.h>

struct crate {
    vec3f_t pos;
    float y_angle;
};

crate_t new_crate(void)
{
    struct crate *a = malloc(sizeof(struct crate));
    a->pos = malloc_vec3f();
    return a;
}

vec3f_t *crate__pos(crate_t ref)
{
    struct crate *a = ref;

    return &a->pos;
}

float *crate__y_angle(crate_t ref)
{
    struct crate *a = ref;

    return &a->y_angle;
}

static
float scalar_product(vec3f_t a, vec3f_t b)
{
    return *vec3f__x(a) * *vec3f__x(b)
        + *vec3f__y(a) * *vec3f__y(b)
        + *vec3f__z(a) * *vec3f__z(b);
}

int crate__is_point_inside(crate_t ref, float x, float y, float z)
{
    struct crate *a = ref;

    float cy = *vec3f__y(*crate__pos(ref));
    if (!(cy <= y && y < (cy + 1.0f)))
        return 0;

    float cx = *vec3f__x(*crate__pos(ref));
    float cz = *vec3f__z(*crate__pos(ref));

    // point resp. crate base center
    float tx = x - cx;
    float tz = z - cz;

    // rotate point - crate y angle
    float c = cosf(-*crate__y_angle(ref) * M_PI / 180.0f);
    float s = sinf(-*crate__y_angle(ref) * M_PI / 180.0f);
    float rx = tx * c + tz * s;
    float rz = -s * tx + tz * c;

    if (-0.5f <= rx && rx < 0.5f &&
        -0.5f <= rz && rz < 0.5f) {
        return 1;
    } else {
        return 0;
    }

    /*vec3f_t X_local = malloc_vec3f();
    *vec3f__x(X_local) = cosf(*crate__y_angle(ref) * M_PI / 180.0f);
    *vec3f__y(X_local) = 0.0f;
    *vec3f__z(X_local) = sinf(*crate__y_angle(ref) * M_PI / 180.0f);

    vec3f_t Y_local = malloc_vec3f();
    *vec3f__x(Y_local) = 0.0f;
    *vec3f__y(Y_local) = 1.0f;
    *vec3f__z(Y_local) = 0.0f;

    vec3f_t Z_local = malloc_vec3f();
    *vec3f__x(X_local) = sinf(*crate__y_angle(ref) * M_PI / 180.0f);
    *vec3f__y(X_local) = 0.0f;
    *vec3f__z(X_local) = cosf(*crate__y_angle(ref) * M_PI / 180.0f);

    vec3f_t I = malloc_vec3f();
    *vec3f__x(I) = *vec3f__x(*crate__pos(ref));
    *vec3f__y(I) = *vec3f__y(*crate__pos(ref)) + 0.5f;
    *vec3f__z(I) = *vec3f__z(*crate__pos(ref));

    vec3f_t V = malloc_vec3f();
    *vec3f__x(V) = x - *vec3f__x(I);
    *vec3f__y(V) = y - *vec3f__y(I);
    *vec3f__z(V) = z - *vec3f__z(I);

    float px = scalar_product(V, X_local);
    float py = scalar_product(V, Y_local);
    float pz = scalar_product(V, Z_local);

    return 2 * px <= 1.0f && 2 * py <= 1.0f && 2 * pz <= 1.0f;*/
}

static
void f(int ti, int normal)
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
    glBegin(GL_TRIANGLES);
        const int tsl = 4;
        float segl = 1.0f / nseg;
        for (int a = 0; a < tsl; a++) {
            for (int b = 0; b < tsl; b++) {
                //
                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 3; j++) {
                        int x = ti % nseg;
                        int y = ti / nseg;
                        glNormal3f(0.0f, 0.0f, normal);
                        float tx = vtx[i][j][0];
                        float ty = vtx[i][j][1];
                        float ttx = (tx + b) / tsl;
                        float tty = (ty + a) / tsl;
                        if (ttx == 1.0f)
                            ttx = 0.99f;
                        else if (ttx == 0.0f)
                            ttx = 0.01f;
                        
                        if (tty == 1.0f)
                            tty = 0.99f;
                        else if (tty == 0.0f)
                            tty = 0.01f;
                        glTexCoord2f(x * segl + ttx * segl,
                            1 - (y * segl + tty * segl));
                        glVertex2f(1.0f / tsl * b + vtx[i][j][0] / tsl,
                            1.0f / tsl * a + vtx[i][j][1] / tsl);
                    }
                }
                //
            }
        }

    glEnd();
}

static void f1(void)
{
    f(5, -1);
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 1.0f);
        f(5, 1);
    glPopMatrix();
}

void crate__render(void)
{
    glPushMatrix();
        glTranslatef(-0.5f, 0.0f, -0.5f);
        f1();
        glPushMatrix();
            glTranslatef(1.0f, 0.0f, 0.0f);
            glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
            f1();
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 1.0f);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            f1();
        glPopMatrix();
    glPopMatrix();
}