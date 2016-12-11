#ifndef RELWARB_RENDERER_H
#define RELWARB_RENDERER_H

#include "relwarb_math.h"

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

#endif // RELWARB_RENDERER_H