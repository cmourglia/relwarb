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

FUNC_DEF(glDebugMessageCallbackARB);

FUNC_DEF(glBegin);
FUNC_DEF(glColor3f);
FUNC_DEF(glVertex2f);
FUNC_DEF(glTexCoord2f);
FUNC_DEF(glEnd);
#endif

FUNC_DEF(glGenerateMipmap);
FUNC_DEF(glGenVertexArrays);
FUNC_DEF(glGenBuffers);
FUNC_DEF(glBindBuffer);
FUNC_DEF(glBufferData);

global_variable GLuint g_bitmapProg;

global_variable const char* bitmapVert = R"(
#version 330

layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_uv;

uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;

out vec2 uv;

void main()
{
    gl_Position = u_proj * u_view * u_model * vec4(in_pos);
    uv = in_uv;
}
)";

global_variable const char* bitmapFrag = R"(
#version 330

in vec2 uv;
out vec4 color;

uniform sampler2D u_tex;

void main()
{
    color = texture(u_tex, uv);
}
)";

global_variable GLfloat vertices[] = 
{
    -.5f, -.5f, 0.f, 0.f, 
     .5f, -.5f, 1.f, 0.f,
     .5f,  .5f, 1.f, 1.f,
    -.5f,  .5f, 0.f, 1.f,
};

global_variable GLuint indices[] = 
{
    0, 1, 2,
    0, 2, 3,
};

global_variable GLuint vao, vbo, ibo;

void InitializeRenderer()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    Assert(vbo);
    Assert(ibo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderBitmap(Bitmap* bitmap, Transform* transform)
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

    // glEnable(GL_TEXTURE_2D);
    // glBegin(GL_QUADS);
    // glTexCoord2f(0, 0); glVertex2f(x, y);
    // glTexCoord2f(1, 0); glVertex2f(x + 0.1f, y);
    // glTexCoord2f(1, 1); glVertex2f(x + 0.1f, y + 0.1f);
    // glTexCoord2f(0, 1); glVertex2f(x, y + 0.1f);
    // glEnd();

    glFlush();
}

void ReleaseBitmap(Bitmap* bitmap)
{
    if (glIsTexture(bitmap->texture))
    {
        glDeleteTextures(1, &bitmap->texture);
    }
}
