#ifndef RELWARB_OPENGL_H
#define RELWARB_OPENGL_H

#include "relwarb_defines.h"
#include "relwarb_math.h"

#if _WIN32
#define FUNC_VAR(name) extern def_##name* name

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;

#define GL_TRUE true
#define GL_FALSE false

/* StringName */
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03

#define GL_NUM_EXTENSIONS                 0x821D

#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_COLOR_BUFFER_BIT               0x00004000

#define GL_TEXTURE_2D                     0x0DE1
#define GL_RGBA8UI                        0x8D7C
#define GL_RGBA8                          0x8058
#define GL_RGBA                           0x1908

#define GL_UNSIGNED_BYTE                  0x1401

#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803

#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703
#define GL_CLAMP_TO_EDGE                  0x812F

// TODO(Charly): Add needed opengl functions

// Core 3.3 stuff
// TODO(Charly): Check if this is ok on linux
typedef const char* def_glGetStringi(GLenum, GLuint);
FUNC_VAR(glGetStringi);

typedef void def_glUseProgram(GLuint);
FUNC_VAR(glUseProgram);

typedef void def_glGetIntegerv(GLenum, GLint*);
FUNC_VAR(glGetIntegerv);

typedef void def_glViewport(GLint, GLint, GLsizei, GLsizei);
FUNC_VAR(glViewport);

typedef void def_glClearColor(GLclampf, GLclampf, GLclampf, GLclampf);
FUNC_VAR(glClearColor);

typedef void def_glClear(GLbitfield);
FUNC_VAR(glClear);

typedef void def_glFlush();
FUNC_VAR(glFlush);

typedef GLboolean def_glIsTexture(GLuint);
FUNC_VAR(glIsTexture);

typedef void def_glEnable(GLenum);
FUNC_VAR(glEnable);

typedef void def_glGenTextures(GLsizei, GLuint*);
FUNC_VAR(glGenTextures);

typedef void def_glDeleteTextures(GLsizei, const GLuint*);
FUNC_VAR(glDeleteTextures);

typedef void def_glBindTexture(GLenum, GLuint);
FUNC_VAR(glBindTexture);

typedef void def_glTexImage2D(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
FUNC_VAR(glTexImage2D);

typedef void def_glTexParameteri(GLenum, GLenum, GLint);
FUNC_VAR(glTexParameteri);

typedef void def_glGenerateMipmap(GLenum);
FUNC_VAR(glGenerateMipmap);

// GL_ARB_debug_output
#define GL_DEBUG_TYPE_ERROR_ARB                              0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB                0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB                 0x824E
#define GL_DEBUG_TYPE_PORTABILITY_ARB                        0x824F
#define GL_DEBUG_TYPE_PERFORMANCE_ARB                        0x8250
#define GL_DEBUG_TYPE_OTHER_ARB                              0x8251

#define GL_DEBUG_SEVERITY_HIGH_ARB                           0x9146
#define GL_DEBUG_SEVERITY_MEDIUM_ARB                         0x9147
#define GL_DEBUG_SEVERITY_LOW_ARB                            0x9148

typedef void def_DebugProc(GLenum, GLenum, GLuint, GLenum, GLsizei, const char*, const void*);

typedef void def_glDebugMessageCallbackARB(def_DebugProc, void*);
FUNC_VAR(glDebugMessageCallbackARB);

// NOTE(Charly): ---------- OpenGL1 begin ----------
// TODO(Charly): This is opengl1 stuff only, remove this
#define GL_TRIANGLES    0x0004
#define GL_QUADS        0x0007

typedef void def_glBegin(GLenum);
FUNC_VAR(glBegin);

typedef void def_glColor3f(GLfloat, GLfloat, GLfloat);
FUNC_VAR(glColor3f);

typedef void def_glVertex2f(GLfloat, GLfloat);
FUNC_VAR(glVertex2f);

typedef void def_glTexCoord2f(GLfloat, GLfloat);
FUNC_VAR(glTexCoord2f);

typedef void def_glEnd();
FUNC_VAR(glEnd);
// NOTE(Charly): ----------  OpenGL1 end  ----------
#else
#include <GL/gl.h>

typedef void def_glGenerateMipmap(GLenum);
extern def_glGenerateMipmap* glGenerateMipmap_;
#define glGenerateMipmap glGenerateMipmap_

typedef void def_glGenVertexArrays(GLsizei, GLuint*);
extern def_glGenVertexArrays* glGenVertexArrays_;
#define glGenVertexArrays glGenVertexArrays_

typedef void def_glGenBuffers(GLsizei, GLuint*);
extern def_glGenBuffers* glGenBuffers_;
#define glGenBuffers glGenBuffers_

typedef void def_glBindBuffer(GLenum, GLuint);
extern def_glBindBuffer* glBindBuffer_;
#define glBindBuffer glBindBuffer_

typedef void def_glBufferData(GLenum, GLsizeiptr, const void*, GLenum);
extern def_glBufferData* glBufferData_;
#define glBufferData glBufferData_

#endif

struct Bitmap;
struct GameState;

struct Transform
{
    Vec2 position;
    Vec2 offset;
    Vec2 scale;

    // TODO(Charly). Orientation ?
};

void InitializeRenderer(GameState* gameState);
void ResizeRenderer(GameState* gameState);

// TODO(Charly): x and y are given in opengl coordinates for now,
//               maybe this should change
void RenderBitmap(Bitmap* bitmap, Transform* transform);

// NOTE(Charly): Cleanup GPU memory
void ReleaseBitmap(Bitmap* bitmap);

#endif // RELWARB_OPENGL_H
