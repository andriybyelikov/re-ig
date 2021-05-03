#include <GLFW/glfw3.h>

#ifndef __USE_MISC
#define __USE_MISC
#endif
#include <math.h>

void ceil_light__render(void)
{
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_TRIANGLE_FAN);
        glColor3ub(255, 255, 255);
        glVertex2f(0.0f, 0.0f);
        for (int i = 0; i <= 16; i++) {
            glVertex2f(cosf(2 * M_PI * ((float)i / 16)),
                sinf(2 * M_PI * ((float)i / 16)));
        }
    glEnd();
    glEnable(GL_TEXTURE_2D);
}
