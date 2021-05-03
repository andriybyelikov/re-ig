#include <math.h>
#include "objects/solar_system.h"
#include <stdlib.h>
#include <stdio.h>

#include "my_stb.h"

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

struct celestial_object {
    float radius;
    float distance; // to sun?
    float rotation_period;
    float revolution_period;
    float tilt;
    float orbital_inclination;

    float rotation;
    float revolution;

    GLUquadric *quadric;
    GLuint texture;
};

static
GLuint load_texture(const char *file)
{
    stbi_set_flip_vertically_on_load(1);
    int width, height, channels;
    unsigned char *data = stbi_load(file, &width, &height, &channels,
        STBI_rgb);
    if (data != NULL) {
        fprintf(stderr, "tex %d %d\n", width, height);
    }
    GLuint tid;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
        GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    return tid;
}

static
void celestial_object__init(void *ref, float radius, float distance,
    float rotation_period, float revolution_period, float tilt,
    float orbital_inclination, const char *file)
{
    struct celestial_object *a = ref;

    a->radius = log(radius);
    a->distance = log(distance);
    a->rotation_period = rotation_period;
    a->revolution_period = revolution_period;
    a->tilt = tilt;
    a->orbital_inclination = orbital_inclination;

    a->rotation = 0.0f;
    a->revolution = 0.0f;

    a->quadric = gluNewQuadric();
    gluQuadricTexture(a->quadric, GL_TRUE);

    a->texture = load_texture(file);
}

static
void celestial_object__proc(void *ref)
{
    struct celestial_object *a = ref;

    if (a->rotation_period != 0.0f)
        a->rotation = fmod(a->rotation + (1.0f / a->rotation_period), 360.0f);
    if (a->revolution_period != 0.0f)
        a->revolution = fmod(a->revolution + (1.0f / a->revolution_period), 360.0f);
}

static
void celestial_object__render(void *ref, int is_sun)
{
    struct celestial_object *a = ref;

    glPushMatrix();
        glScalef(a->radius, a->radius, a->radius);
        glBindTexture(GL_TEXTURE_2D, a->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        if (is_sun) {
            GLfloat mat_emission_value[4] = {1.0f,1.0f,1.0f,1.0f};
            glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission_value);
        }
        gluSphere(a->quadric, 1.0, 32, 32);
        if (is_sun) {
            GLfloat mat_emission_value[4] = {0.0f,0.0f,0.0f,0.0f};
            glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission_value);
        }
    glPopMatrix();
}


struct solar_system {
    struct celestial_object objects[9];
};

solar_system_t new_solar_system(void)
{
    return malloc(sizeof(struct solar_system));
}

void solar_system__init(solar_system_t ref)
{
    struct solar_system *a = ref;
    

    // Sun
    celestial_object__init(&a->objects[0], 695510.0f, 1.0f, 24.47f, 0.0f, 0.0f, 0.0f, "../planets/sun.jpg");

    // Mercury
    celestial_object__init(&a->objects[1], 2439.7f, 57.9e6f, 58.65f, 87.97f, 0.0f, 3.38f, "../planets/mercury.jpg");

    // Venus
    celestial_object__init(&a->objects[2], 6051.8f, 108.2e6f, -243.02f, 224.70f, 178.0f, 3.86f, "../planets/venus.jpg");

    // Earth
    celestial_object__init(&a->objects[3], 6371.0f, 149.6e6f, 1.00f, 365.26f, 23.4f, 7.155f, "../planets/earth.jpg");

    // Mars
    celestial_object__init(&a->objects[4], 3389.5f, 227.9e6f, 1.02f, 686.98f, 25.0f, 5.65f, "../planets/mars.jpg");

    // Jupiter
    celestial_object__init(&a->objects[5], 69911.0f,  778.5e6f, 0.41f,  4328.90f, 3.08f, 6.09f, "../planets/jupiter.jpg");
    
    // Saturn
    celestial_object__init(&a->objects[6], 58232.0f, 1434.0e6f, 0.44f, 10752.90f, 26.7f, 5.51f, "../planets/saturn.jpg");

    // Uranus
    celestial_object__init(&a->objects[7], 25362.0f, 2871.0e6f, -0.72f, 30685.00f, 97.9f, 6.48f, "../planets/uranus.jpg");

    // Neptune
    celestial_object__init(&a->objects[8], 24622.0f, 4495.0e6f,  0.67f, 60190.00f, 29.6f, 6.43f, "../planets/neptune.jpg");
}

void solar_system__proc(solar_system_t ref)
{
    struct solar_system *a = ref;

    for (int i = 0; i < 9; i++) {
        celestial_object__proc(&a->objects[i]);
    }
}

void solar_system__render(solar_system_t ref)
{
    struct solar_system *a = ref;

    float d = 0.0f;
    for (int i = 0; i < 9; i++) {
        struct celestial_object *b = &a->objects[i];
        d += 1.5f * b->distance;
        glPushMatrix();
            glRotatef(b->orbital_inclination, 1.0f, 0.0f, 1.0f);
            glRotatef(b->revolution * d * (i > 4 ? 10 : 1), 0.0f, 1.0f, 0.0f);
            glTranslatef(d, 0.0f, 0.0f);
            glRotatef(b->tilt, 0.0f, 0.0f, 1.0f);
            glRotatef(b->rotation, 0.0f, 1.0f, 0.0f);
            //
            celestial_object__render(b, i == 0);
        glPopMatrix();
        d += b->radius;
    }
}
