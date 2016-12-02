#ifndef RELWRAB_OPENGL_H
#define RELWRAB_OPENGL_H

#include <GL/gl.h>

#ifndef GLchar
#define GLchar char
#endif

#if defined(_GL_ALLOW_ERROR_CHECKING)
// TODO(Charly): Maybe we do not want to assert on opengl errors
//               and just log them ?
#define GL_ASSERT(x)                                        \
{                                                           \
    (x);                                                    \
    GLenum glErr;                                           \
    while ((glErr = glGetError()) != GL_NO_ERROR)           \
    {                                                       \
        Assert(!"OpenGL Error");                            \
    }                                                       \
}

#define GL_CHECK_ERRORS()                                   \
{                                                           \
    GLenum glErr;                                           \
    while ((glErr = glGetError()) != GL_NO_ERROR)           \
    {                                                       \
        Assert(!"OpenGL Error");                            \
    }                                                       \
}                                                           \
(void)0

#else
#define GL_ASSERT(x) x
#define GL_CHECK_ERRORS()
#endif

#define GL_NUM_EXTENSIONS                 0x821D

// TODO(Charly): Add needed opengl functions

// Core 3.3 stuff
typedef const char* def_glGetStringi(GLenum, GLuint);
def_glGetStringi* glGetStringi;

typedef void def_glUseProgram(GLuint);
def_glUseProgram* glUseProgram;

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
def_glDebugMessageCallbackARB* glDebugMessageCallbackARB;

#endif // RELWRAB_OPENGL_H
