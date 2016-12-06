#include "relwarb_opengl.h"
#include "relwarb.h"

#define FUNC_DEF(name) def_##name* name

#if defined(_WIN32)

FUNC_DEF(glGetStringi);
FUNC_DEF(glUseProgram);
FUNC_DEF(glGetIntegerv);
FUNC_DEF(glViewport);
FUNC_DEF(glClearColor);
FUNC_DEF(glClear);
FUNC_DEF(glFlush);
FUNC_DEF(glIsTexture);
FUNC_DEF(glEnable);
FUNC_DEF(glGenTextures);
FUNC_DEF(glDeleteTextures);
FUNC_DEF(glBindTexture);
FUNC_DEF(glTexImage2D);
FUNC_DEF(glTexParameteri);
FUNC_DEF(glGenerateMipmap);

FUNC_DEF(glDebugMessageCallbackARB);

FUNC_DEF(glBegin);
FUNC_DEF(glColor3f);
FUNC_DEF(glVertex2f);
FUNC_DEF(glTexCoord2f);
FUNC_DEF(glEnd);
#else
FUNC_DEF(glGenerateMipmap);
#endif

void RenderBitmap(Bitmap* bitmap, real32 x, real32 y)
{
    // TODO(Charly): Should this be done in an Init step ?
    if (!glIsTexture(bitmap->texture))
    {
        glGenTextures(1, &bitmap->texture);
        glBindTexture(GL_TEXTURE_2D, bitmap->texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                     bitmap->width, bitmap->height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE,
                     bitmap->data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(x, y);
    glTexCoord2f(1, 0); glVertex2f(x + 0.1f, y);
    glTexCoord2f(1, 1); glVertex2f(x + 0.1f, y + 0.1f);
    glTexCoord2f(0, 1); glVertex2f(x, y + 0.1f);
    glEnd();

    glFlush();
}

void ReleaseBitmap(Bitmap* bitmap)
{
    if (glIsTexture(bitmap->texture))
    {
        glDeleteTextures(1, &bitmap->texture);
    }
}
