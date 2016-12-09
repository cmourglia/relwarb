#ifndef RELWARB_OPENGL_H
#define RELWARB_OPENGL_H

#include "relwarb_defines.h"
#include "relwarb_math.h"

#define GL_GLEXT_PROTOTYPES
#include "glcorearb.h"

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
extern def_glDebugMessageCallbackARB* glDebugMessageCallbackARB_;
#define glDebugMessageCallbackARB glDebugMessageCallbackARB_

struct Bitmap;
struct GameState;

struct Transform
{
    Vec2 position;
    Vec2 offset;
    Vec2 scale;

    // TODO(Charly). Orientation ?
};

void InitializeRenderer();
void ResizeRenderer(GameState* gameState);

// TODO(Charly): x and y are given in opengl coordinates for now,
//               maybe this should change
void RenderBitmap(Bitmap* bitmap, Transform* transform);

// NOTE(Charly): Cleanup GPU memory
void ReleaseBitmap(Bitmap* bitmap);

#endif // RELWARB_OPENGL_H
