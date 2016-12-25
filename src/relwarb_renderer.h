#ifndef RELWARB_RENDERER_H
#define RELWARB_RENDERER_H

#include "zmath.hpp"
#include "relwarb_entity.h"

struct Bitmap;
struct GameState;
struct Entity;

enum SpriteType
{
    SpriteType_Still = 0,
    SpriteType_Timed,
};

struct Sprite
{
    SpriteType spriteType;
    union {
        Bitmap* stillSprite;
        // Time based sprite
        struct {
            uint32 nbSteps;
            Bitmap** steps;
            uint32 currentStep;
            real32 stepTime;

            bool32 active;
            real32 elapsed;
        };
    };
};

enum RenderingPatternType
{
    RenderingPattern_Unique = 0,
    RenderingPattern_Multi,
    RenderingPattern_Fill,
};

struct RenderingPattern
{
    RenderingPatternType patternType;

    union {
        // Unique pattern
        struct {
            Sprite* unique;
        };

        // Multiple state pattern
        struct {
            uint32 current;
            Sprite** bitmaps;
        };

        // Fill pattern
        // TODO(Thomas): Handle fill pattern evolving through time or something
        struct {
            z::vec2 size;
            uint8* pattern;
            Bitmap** tiles;
        };
    };

    RenderingPattern() {}
};

struct Transform
{
    z::vec2 position;
    z::vec2 offset;
    z::vec2 scale;

    z::mat4 proj;
    z::mat4 world;

    // TODO(Charly). Orientation ?
};

void InitializeRenderer();
void ResizeRenderer(GameState* gameState);

Sprite* CreateStillSprite(GameState* gameState, Bitmap* bitmap);

Sprite* CreateTimeSprite(GameState* gameState, uint32 nbBitmaps, Bitmap** bitmaps, real32 stepTime, bool32 active = true);

Bitmap* GetSpriteBitmap(const Sprite* sprite);

// NOTE(Thomas): Maybe just merge into render function or something ?
void UpdateSpriteTime(Sprite* sprite, real32 dt);

RenderingPattern* CreateUniqueRenderingPattern( GameState* gameState,
                                                Sprite* sprite);

RenderingPattern* CreateHeroRenderingPattern(   GameState* gameState,
                                                Sprite** sprites);

RenderingPattern* CreateFillRenderingPattern(   GameState* gameState,
                                                z::vec2 size,
                                                uint8* pattern,
                                                uint8 nbBitmaps,
                                                Bitmap** bitmaps);

void AddRenderingPatternToEntity(Entity* entity, RenderingPattern* pattern);

void RenderPattern(RenderingPattern* pattern, Transform* transform, z::vec2 size = z::vec2(0));

// Render the pattern at the position given in transform, and repeated to fit the given size
void RenderFillPattern(RenderingPattern* pattern, Transform* transform, z::vec2 size);

// TODO(Charly): x and y are given in opengl coordinates for now,
//               maybe this should change
void RenderBitmap(Bitmap* bitmap, const Transform* transform);

// NOTE(Charly): Cleanup GPU memory
void ReleaseBitmap(Bitmap* bitmap);

void RenderGrid(z::vec2 resolution = z::vec2(10, 10), z::vec2 center = z::vec2(0));

#endif // RELWARB_RENDERER_H
