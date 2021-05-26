#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <libgameaudio.h>

#include "my_stb.h"

#include "objects/camera.h"
#include "objects/elevator.h"
#include "objects/ceil_light.h"
#include "objects/crate.h"
#include "objects/solar_system.h"

#include "map_data.h"

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

int main(int argc, char *argv[])
{
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        exit(EXIT_FAILURE);
    }

    GLFWwindow *window = glfwCreateWindow(960, 540, "re-ig", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //
    stbi_set_flip_vertically_on_load(1);

    int width, height, channels;
    unsigned char *data = stbi_load("../data/textures/tiles.png", &width, &height, &channels,
        STBI_rgb);
    if (data != NULL) {
        fprintf(stderr, "tex %d %d\n", width, height);
    }
    GLuint atlas;
    glGenTextures(1, &atlas);
    glBindTexture(GL_TEXTURE_2D, atlas);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
        GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    
    glShadeModel(GL_SMOOTH);

    //
    camera_t cam = new_camera();
    *vec3f__x(*camera__pos(cam)) = 15.5f;
    *vec3f__y(*camera__pos(cam)) = 0.1f;
    *vec3f__z(*camera__pos(cam)) = 12.0f;
    *camera__azimuthal_angle(cam) = -1.57f;
    *camera__polar_angle(cam) = 1.57f;
    double cur_x, cur_y;
    glfwGetCursorPos(window, &cur_x, &cur_y);
    int rev_playing = 0;
    elevator_t elv = new_elevator();
    elevator__init(elv);
    float elv_prev_y = *elevator__y(elv);
    //
    solar_system_t solar = new_solar_system();
    solar_system__init(solar);
    //
    const int num_crates = 11;
    crate_t crates[11];
    float crates_data[11][4] = {
        { 14.8f, 0.0f, 16.5f, 20.0f },
        { 16.2f, 0.0f, 17.3f, 30.0f },
        { 15.7f, 1.0f, 16.9f, 15.0f },

        { 1.3f, 2.0f, 2.9f, 15.0f },
        { 7.7f, 0.0f, 7.9f, 70.0f },
        { 12.7f, 0.0f, 10.9f, 15.0f },

        { 1.7f, 0.0f, 7.0f, 30.0f },
        { 2.0f, 0.0f, 8.5f, 0.0f },
        { 1.5f, 0.0f, 9.5f, 0.0f },
        { 1.1f, 0.0f, 10.5f, 0.0f },
        { 1.4f, 1.0f, 8.9f, 25.0f }
    };
    for (int i = 0; i < num_crates; i++) {
        crates[i] = new_crate();
        *vec3f__x(*crate__pos(crates[i])) = crates_data[i][0];
        *vec3f__y(*crate__pos(crates[i])) = crates_data[i][1];
        *vec3f__z(*crate__pos(crates[i])) = crates_data[i][2];
        *crate__y_angle(crates[i]) = crates_data[i][3];
    }
    //


    lgaInit(1, 2, 1, 1, 1, 2);
    lgaLoad(LGA_BGM, LGA_OPUS, "../data/audio/bgm/shadows.opus");
    lgaLoad(LGA_BGS, LGA_OPUS, "../data/audio/bgs/rev_w.opus");
    lgaLoad(LGA_BGS, LGA_OPUS, "../data/audio/bgs/rev_s.opus");
    lgaLoad(LGA_ME, LGA_OPUS, "../data/audio/me/chime.opus");
    lgaLoad(LGA_SE, LGA_OPUS, "../data/audio/se/fall.opus");
    lgaPlay(LGA_BGM, 0, 0);

    double ptime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double deltatime = glfwGetTime() - ptime;
        ptime = glfwGetTime();
        solar_system__proc(solar);
        elv_prev_y = *elevator__y(elv);
        elevator__proc(elv, deltatime);
        //
        /* on elevator */
        {
            float cam_x = *vec3f__x(*camera__pos(cam));
            float cam_y = *vec3f__y(*camera__pos(cam));
            float cam_z = *vec3f__z(*camera__pos(cam));
            int tile = map_geometry_0[(int)cam_z][(int)cam_x];
            if (   3.0f <= cam_x && cam_x < 4.0f
                && 3.0f <= cam_z && cam_z < 4.0f
                && fabsf(cam_y - *elevator__y(elv)) <= 0.2f) {
                *vec3f__y(*camera__pos(cam)) = *elevator__y(elv) + 0.1f;
                if (*elevator__y(elv) == 2.0f && *elevator__y(elv) > elv_prev_y) {
                    lgaPlay(LGA_ME, 0, 0);
                }
            } else if (cam_y > 0.1f && (tile == 0 || tile == 3)) {
                //fprintf(stderr, "x = %d, z = %d, tile = %d\n",
                //    (int)cam_x, (int)cam_z, tile);
                /* in air / gravity */
                float next_y = *vec3f__y(*camera__pos(cam)) - 9.81f / 2.0f * deltatime;
                // 2 meters high fall max, not worth complicating acceleration
                *vec3f__y(*camera__pos(cam)) = next_y; 
                if (*vec3f__y(*camera__pos(cam)) < 0.1f) {
                    *vec3f__y(*camera__pos(cam)) = 0.1f;
                    lgaPlay(LGA_SE, 0, 0);
                }
            }
        }
        //
        {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            *camera__azimuthal_angle(cam) += (x - cur_x) * 0.01f;
            *camera__polar_angle(cam) += (y - cur_y) * 0.01f;
            if (*camera__polar_angle(cam) < 0.0314f) {
                *camera__polar_angle(cam) = 0.0314f;
            } else if (*camera__polar_angle(cam) > 2.1f) {
                *camera__polar_angle(cam) = 2.1f;
            }
            cur_x = x;
            cur_y = y;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
            float cam_x0 = *vec3f__x(*camera__pos(cam));
            float cam_y = *vec3f__y(*camera__pos(cam));
            float cam_z0 = *vec3f__z(*camera__pos(cam));
            float cam_x1 = cam_x0 + cosf(*camera__azimuthal_angle(cam)) * 0.25f;
            float cam_z1 = cam_z0 + sinf(*camera__azimuthal_angle(cam)) * 0.25f;
            int tile = map_geometry_0[(int)cam_z1][(int)cam_x1];
            int next_point_in_crate = 0;
            for (int i = 0; i < num_crates; i++) {
                if (crate__is_point_inside(crates[i], cam_x1, cam_y, cam_z1)) {
                    next_point_in_crate = 1;
                    break;
                }
            }
            if (0.0f <= cam_x1 && cam_x1 < 20.0f
                &&
                0.0f <= cam_z1 && cam_z1 < 20.0f
                &&
                tile != 3
                &&
                /* tile == 1 || tile == 2 -> cam_y >= 2.1 */
                (!(tile == 1 || tile == 2) || cam_y >= 2.1f)
                &&
                /* elevator collision */
                /* (int)cam_x1 == 3 && (int)cam_z1 == 3 -> *elevator__y(elv) < cam_y */
                (!((int)cam_x1 == 3 && (int)cam_z1 == 3) || *elevator__y(elv) < cam_y)
                && !next_point_in_crate) {
                *vec3f__x(*camera__pos(cam)) += cosf(*camera__azimuthal_angle(cam)) * 0.05f;
                *vec3f__z(*camera__pos(cam)) += sinf(*camera__azimuthal_angle(cam)) * 0.05f;
                if (!rev_playing) {
                    lgaPlay(LGA_BGS, 0, 0);
                    rev_playing = 1;
                }
            } else {
                if (rev_playing) {
                    lgaStop(LGA_E_QT, LGA_BGS, 0);
                    rev_playing = 0;
                }
            }
        } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
            float cam_x0 = *vec3f__x(*camera__pos(cam));
            float cam_y = *vec3f__y(*camera__pos(cam));
            float cam_z0 = *vec3f__z(*camera__pos(cam));
            float cam_x1 = cam_x0 - cosf(*camera__azimuthal_angle(cam)) * 0.25f;
            float cam_z1 = cam_z0 - sinf(*camera__azimuthal_angle(cam)) * 0.25f;
            int tile = map_geometry_0[(int)cam_z1][(int)cam_x1];
            int next_point_in_crate = 0;
            for (int i = 0; i < num_crates; i++) {
                if (crate__is_point_inside(crates[i], cam_x1, cam_y, cam_z1)) {
                    next_point_in_crate = 1;
                    break;
                }
            }
            if (0.0f <= cam_x1 && cam_x1 < 20.0f
                &&
                0.0f <= cam_z1 && cam_z1 < 20.0f
                &&
                tile != 3
                &&
                /* tile == 1 || tile == 2 -> cam_y >= 2.1 */
                (!(tile == 1 || tile == 2) || cam_y >= 2.1f)
                &&
                /* elevator collision */
                /* (int)cam_x1 == 3 && (int)cam_z1 == 3 -> *elevator__y(elv) < cam_y */
                (!((int)cam_x1 == 3 && (int)cam_z1 == 3) || *elevator__y(elv) < cam_y)
                && !next_point_in_crate) {
                *vec3f__x(*camera__pos(cam)) -= cosf(*camera__azimuthal_angle(cam)) * 0.05f;
                *vec3f__z(*camera__pos(cam)) -= sinf(*camera__azimuthal_angle(cam)) * 0.05f;
                if (!rev_playing) {
                    lgaPlay(LGA_BGS, 0, 0);
                    rev_playing = 1;
                }
            } else {
                if (rev_playing) {
                    lgaStop(LGA_E_QT, LGA_BGS, 0);
                    rev_playing = 0;
                }
            }
        } else {
            if (rev_playing) {
                lgaStop(LGA_E_QT, LGA_BGS, 0);
                rev_playing = 0;
            }
        }

        //
        lgaProc();
        //
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        //
        {
            int w, h;
            glfwGetWindowSize(window, &w, &h);
            glViewport(0, 0, w, h);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(70.0f, (GLdouble)w / h, 0.05f, 150.0f);
        }
        //
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //
        gluLookAt(  *vec3f__x(*camera__pos(cam)),
                    *vec3f__y(*camera__pos(cam)),
                    *vec3f__z(*camera__pos(cam)),

                    *vec3f__x(*camera__pos(cam)) + cosf(*camera__azimuthal_angle(cam)) * sinf(*camera__polar_angle(cam)),
                    *vec3f__y(*camera__pos(cam)) + cosf(*camera__polar_angle(cam)),
                    *vec3f__z(*camera__pos(cam)) + sinf(*camera__azimuthal_angle(cam)) * sinf(*camera__polar_angle(cam)),

                    0.0f, 1.0f, 0.0f);
        //
        glBindTexture(GL_TEXTURE_2D, atlas);
        //
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                float height = 0.0f;
                int tfloor = 0, tceil = 2, twall = 1;
                switch (map_geometry_0[i][j]) {
                    case 0:
                        height = 0.0f;
                        tfloor = 0;
                        break;
                    case 1:
                        height = 2.0f;
                        tfloor = 0;
                        break;
                    case 2:
                        height = 2.0f;
                        tfloor = 4;
                        tceil = 4;
                        twall = 4;
                        break;
                }
                // floors
                if (map_geometry_0[i][j] < 3) {
                    if (!(i == 3 && j == 3)) {
                        glPushMatrix();
                            glTranslatef(j, height, i);
                            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                            f(tfloor, -1);
                        glPopMatrix();
                    }
                    glPushMatrix();
                        glTranslatef(j, 3.0f, i);
                        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                        f(tceil, 1);
                    glPopMatrix();
                }
                /* walls:
                    every tile is responsible for painting its top-left corner walls
                    */
                {
                    switch (map_geometry_0[i][j]) {
                    case 0:
                        if (j == 0 || j > 0 && map_geometry_0[i][j -1] == 3) {
                            // draw left wall
                            for (int h = 0; h < 3; h++) {
                                glPushMatrix();
                                    glTranslatef(j, h, i);
                                    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
                                    f(twall, -1);
                                glPopMatrix();
                            }
                        }
                        if (i - 1 >= 0 && map_geometry_0[i - 1][j] == 1) {
                            // draw top wall to platform
                            for (int h = 0; h < 2; h++) {
                                glPushMatrix();
                                    glTranslatef(j, h, i);
                                    f(twall, 1);
                                glPopMatrix();
                            }
                        }
                        if (i - 1 >= 0 && map_geometry_0[i - 1][j] == 2) {
                            // draw top wall to vent
                            for (int h = 0; h < 2; h++) {
                                glPushMatrix();
                                    glTranslatef(j, h, i);
                                    f(twall, 1);
                                glPopMatrix();
                            }
                        }
                        if (i - 1 >= 0 && map_geometry_0[i - 1][j] == 3) {
                            // draw top wall
                            for (int h = 0; h < 3; h++) {
                                glPushMatrix();
                                    glTranslatef(j, h, i);
                                    f(twall, 1);
                                glPopMatrix();
                            }
                        }
                        if (j - 1 >= 0 && map_geometry_0[i][j - 1] == 1) {
                            // draw left wall to platform
                            for (int h = 0; h < 2; h++) {
                                glPushMatrix();
                                    glTranslatef(j, h, i);
                                    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
                                    
                                    f(twall, -1);
                                glPopMatrix();
                            }
                        }
                        break;
                    case 1:
                        if (j == 0) {
                            // draw left wall on platform
                            for (int h = 2; h < 3; h++) {
                                glPushMatrix();
                                    glTranslatef(j, h, i);
                                    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
                                    f(twall, -1);
                                glPopMatrix();
                            }
                        }
                        if (j - 1 >= 0 && map_geometry_0[i][j - 1] == 0) {
                            // draw left wall from platform
                            for (int h = 0; h < 2; h++) {
                                glPushMatrix();
                                    glTranslatef(j, h, i);
                                    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
                                    f(twall, 1);
                                glPopMatrix();
                            }
                        }
                        if (i - 1 >= 0 && map_geometry_0[i - 1][j] == 3) {
                            // draw top wall on platform
                            for (int h = 2; h < 3; h++) {
                                glPushMatrix();
                                    glTranslatef(j, h, i);
                                    f(twall, 1);
                                glPopMatrix();
                            }
                        }
                        break;
                    case 2:
                        if (i == 0) {
                            // draw top wall
                            glPushMatrix();
                                glTranslatef(j, 2, i);
                                f(twall, -1);
                            glPopMatrix();
                        }
                        if (j - 1 >= 0 && map_geometry_0[i][j - 1] == 3) {
                            // draw left wall
                            glPushMatrix();
                                glTranslatef(j, 2, i);
                                glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
                                f(twall, -1);
                            glPopMatrix();
                        }
                        break;
                    case 3:
                        // draw vent bottom wall
                        if (i - 1 >= 0 && map_geometry_0[i - 1][j] == 2) {
                            glPushMatrix();
                                glTranslatef(j, 2, i);
                                f(4, -1);
                            glPopMatrix();
                        }
                        // draw vent left wall
                        if (j - 1 >= 0 && map_geometry_0[i][j - 1] == 2) {
                            glPushMatrix();
                                glTranslatef(j, 2, i);
                                glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
                                f(4, -1);
                            glPopMatrix();
                        }
                        // draw platform left wall
                        if (j - 1 >= 0 && map_geometry_0[i][j - 1] == 1) {
                            glPushMatrix();
                                glTranslatef(j, 2, i);
                                glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
                                f(twall, 1);
                            glPopMatrix();
                        }
                        // draw floor left wall
                        if (j - 1 >= 0 && map_geometry_0[i][j - 1] == 0) {
                            for (int h = 0; h < 3; h++) {
                                glPushMatrix();
                                    glTranslatef(j, h, i);
                                    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
                                    f(twall, 1);
                                glPopMatrix();
                            }
                        }
                        // draw floor bottom wall
                        if (i - 1 >= 0 && map_geometry_0[i - 1][j] == 0) {
                            for (int h = 0; h < 3; h++) {
                                glPushMatrix();
                                    glTranslatef(j, h, i);
                                    
                                    f(twall, -1);
                                glPopMatrix();
                            }
                        }
                        break;
                    }
                }
            }
        }
        for (int i = 0; i < 20; i++) {
            // draw planetarium room right wall
            int j = 20;
            if (j - 1 >= 0 && map_geometry_0[i][j - 1] == 0) {
                for (int h = 0; h < 3; h++) {
                    glPushMatrix();
                        glTranslatef(j, h, i);
                        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
                        f(1, 1);
                    glPopMatrix();
                }
            }
        }
        //
        glPushMatrix();
            glTranslatef(3.0f, *elevator__y(elv), 3.0f);
            elevator__render(elv);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(3.5f, 2.99f, 7.5f);
            ceil_light__render();
        glPopMatrix();
        glPushMatrix();
            glTranslatef(9.5f, 2.99f, 7.5f);
            ceil_light__render();
        glPopMatrix();
        glPushMatrix();
            glTranslatef(15.5f, 1.5f, 7.5f);
            glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);
            {
                float s = 0.0125f;
                glScalef(s, s, s);
            }
            solar_system__render(solar);
        glPopMatrix();
        glBindTexture(GL_TEXTURE_2D, atlas);
        for (int i = 0; i < num_crates; i++) {
            glPushMatrix();
                glTranslatef(
                    *vec3f__x(*crate__pos(crates[i])),
                    *vec3f__y(*crate__pos(crates[i])),
                    *vec3f__z(*crate__pos(crates[i])));
                glRotatef(*crate__y_angle(crates[i]), 0.0f, 1.0f, 0.0f);
                crate__render();
            glPopMatrix();
        }

        //
        const GLfloat l0_pos[] = { 9.5f, 2.5f, 7.5f, 1.0f };
        const GLfloat l0_amb[] = { 0.48f, 0.48f, 0.48f, 1.0f };
        const GLfloat l0_dif[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        const GLfloat l0_spe[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, l0_pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, l0_amb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_dif);
        glLightfv(GL_LIGHT0, GL_SPECULAR, l0_spe);

        const GLfloat l1_pos[] = { 3.5f, 2.75f, 7.5f, 1.0f };
        glLightfv(GL_LIGHT1, GL_POSITION, l1_pos);
        glLightfv(GL_LIGHT1, GL_AMBIENT, l0_amb);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, l0_dif);
        glLightfv(GL_LIGHT1, GL_SPECULAR, l0_spe);

        const GLfloat l2_pos[] = { 15.5f, 1.5f, 7.5f, 1.0f };
        const GLfloat l2_amb[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        const GLfloat l2_dif[] = { 0.75f, 0.5f, 0.5f, 1.0f };
        glLightfv(GL_LIGHT2, GL_POSITION, l2_pos);
        glLightfv(GL_LIGHT2, GL_AMBIENT, l2_amb);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, l2_dif);
        glLightfv(GL_LIGHT2, GL_SPECULAR, l0_spe);
        //
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    lgaTerminate();
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
