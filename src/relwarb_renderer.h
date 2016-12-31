#ifndef RELWARB_RENDERER_H
#define RELWARB_RENDERER_H

#include "zmath.hpp"
#include "relwarb_entity.h"
#include "relwarb_opengl.h"

// TODO(Charly): Remove this
#include <vector>

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
    z::vec2 size = z::vec2(1);
    z::vec2 origin = z::vec2(0);

    real32 rotation = 0;
    int orientation = 1;    // NOTE(Charly): Negative value if looking left,
                            //               Positive value otherwise.
};

enum RenderMode
{
    RenderMode_World,
    RenderMode_ScreenAbsolute,
    RenderMode_ScreenRelative,
};

struct Vertex
{
    z::vec2 position;
    z::vec2 texcoord;
};

struct Mesh
{
    RenderMode renderMode;
    GLuint program;
    GLuint texture;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    z::mat3 worldTransform;
    z::vec3 color;
    bool hasColor = false;
};

void InitializeRenderer(GameState* gameState);
void ResizeRenderer(GameState* gameState);
void FlushRenderQueue(GameState* gameState);

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

void RenderBitmap(Bitmap* bitmap, RenderMode mode, Transform* transform);

void LoadTexture(Bitmap* bitmap);
// NOTE(Charly): Cleanup GPU memory
void ReleaseTexture(Bitmap* bitmap);

void RenderText(char* text, z::vec2 pos, z::vec3 color, GameState* state);
void RenderMesh(const Mesh* mesh, z::mat3 projectionMatrix);

z::mat3 GetTransformMatrix(RenderMode renderMode, Transform* transform);
z::mat3 GetProjectionMatrix(RenderMode renderMode, GameState* gameState);
#endif // RELWARB_RENDERER_H
