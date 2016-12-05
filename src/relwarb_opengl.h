#ifndef RELWARB_OPENGL_H
#define RELWARB_OPENGL_H

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

// TODO(Charly): Add needed opengl functions

// Core 3.3 stuff
#if 0

typedef const char* def_glGetStringi(GLenum, GLuint);
global_variable def_glGetStringi* glGetStringi_;
#define glGetStringi glGetStringi

typedef void def_glUseProgram(GLuint);
global_variable def_glUseProgram* glUseProgram_;
#define glUseProgram glUseProgram_

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
global_variable def_glDebugMessageCallbackARB* glDebugMessageCallbackARB_;
#define glDebugMessageCallbackARB glDebugMessageCallbackARB_

#else
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

#endif

// NOTE(Charly): ---------- OpenGL1 begin ----------
// TODO(Charly): This is opengl1 stuff only, remove this
#define GL_TRIANGLES                      0x0004

typedef void def_glBegin(GLenum);
FUNC_VAR(glBegin);

typedef void def_glColor3f(GLfloat, GLfloat, GLfloat);
FUNC_VAR(glColor3f);

typedef void def_glVertex2f(GLfloat, GLfloat);
FUNC_VAR(glVertex2f);

typedef void def_glEnd();
FUNC_VAR(glEnd);
// NOTE(Charly): ----------  OpenGL1 end  ----------

#endif // RELWARB_OPENGL_H
