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
        Bitmap *stillSprite;

        // Time based sprite
        struct
        {
            uint32 nbSteps;
            Bitmap **steps;
            uint32 currentStep;
            real32 stepTime;

            bool32 active;
            real32 elapsed;
        };
    };
};

enum GameUIType
{
    GameUIType_Text = 0,
};

// TODO(Thomas) Rename ?
struct GameUI
{

    GameUIType uiType;
    z::vec2 initialPos;
    real32 duration; // Infinite if negative

    real32 elapsed;

    struct
    {
        char *text;
        z::vec4 initialColor;
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
        struct
        {
            Sprite *unique;
        };

        // Multiple state pattern
        struct
        {
            uint32 current;
            Sprite **bitmaps;
        };

        // Fill pattern
        // TODO(Thomas): Handle fill pattern evolving through time or something
        struct
        {
            z::vec2 size;
            uint8 *pattern;
            Bitmap **tiles;
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
    int orientation = 1; // NOTE(Charly): Negative value if looking left,
                         //               Positive value otherwise.
};

// FIXME(Charly): Not sure about the name ...
enum RenderMode
{
    RenderMode_World,
    RenderMode_ScreenAbsolute,
    RenderMode_ScreenRelative,
};

// FIXME(Charly): Not sure about the name ...
enum ObjectType
{
    ObjectType_Default,
    ObjectType_Debug,
    ObjectType_UI,
};

struct Vertex
{
    z::vec2 position;
    z::vec2 texcoord;
};

struct Mesh
{
    RenderMode renderMode;
    GLuint vertexProgram;
    GLuint fragmentProgram;
    GLuint pipeline;
    GLuint texture;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    z::mat3 worldTransform;
    z::vec4 color;
};

typedef std::vector<Mesh> RenderQueue;

enum
{
    RendererPipeline_Bitmap = 0,
    RendererPipeline_Color,
    RendererPipeline_Text,
    RendererPipeline_Particles,
    RendererPipeline_Count,
};

enum
{
    VertexArray_Bitmap = 0,
    VertexArray_Particles,
    VertexArray_Count,
};

enum
{
    BitmapBuffer_Indices = 0,
    BitmapBuffer_Vertices,
    BitmapBuffer_Texcoords,
    BitmapBuffer_Count,
};

enum
{
    ParticleBuffer_Billboard = 0,
    ParticleBuffer_PosSize,
    ParticleBuffer_Color,
    ParticleBuffer_Count,
};

struct Renderer
{
    RenderQueue defaultRenderQueue;
    RenderQueue debugRenderQueue;
    RenderQueue uiRenderQueue;

    GLuint defaultVertexProg;
    GLuint instancedVertexProg;

    GLuint bitmapFragmentProg;
    GLuint colorFragmentProg;
    GLuint textFragmentProg;
    GLuint particlesFragmentProg;

    GLuint pipelines[RendererPipeline_Count];

    GLuint particlesVAO;
    GLuint particlesVBOs[ParticleBuffer_Count];
};

void InitializeRenderer(GameState *gameState);
void ResizeRenderer(GameState *gameState);
void FlushRenderQueue(GameState *gameState);

Sprite *CreateStillSprite(GameState *gameState, Bitmap *bitmap);

Sprite *CreateTimeSprite(GameState *gameState, uint32 nbBitmaps, Bitmap **bitmaps, real32 stepTime, bool32 active = true);

Bitmap *GetSpriteBitmap(const Sprite *sprite);

GameUI *CreateTextualGameUI(GameState *gameState, char *text, z::vec4 color = z::vec4(1), z::vec2 initialPos = z::vec2(0), real32 duration = -1.0);

// NOTE(Thomas): Maybe just merge into render function or something ?
void UpdateSpriteTime(Sprite *sprite, real32 dt);

void UpdateGameUITime(GameUI *gameUI, real32 dt);

RenderingPattern *CreateUniqueRenderingPattern(GameState *gameState,
                                               Sprite *sprite);

RenderingPattern *CreateHeroRenderingPattern(GameState *gameState,
                                             Sprite **sprites);

RenderingPattern *CreateFillRenderingPattern(GameState *gameState,
                                             z::vec2 size,
                                             uint8 *pattern,
                                             uint8 nbBitmaps,
                                             Bitmap **bitmaps);

void AddRenderingPatternToEntity(Entity *entity, RenderingPattern *pattern);

void RenderPattern(GameState *gameState, RenderingPattern *pattern, Transform *transform, z::vec2 size = z::vec2(0));

// Render the pattern at the position given in transform, and repeated to fit the given size
void RenderFillPattern(GameState *gameState, RenderingPattern *pattern, Transform *transform, z::vec2 size);

void RenderGameUI(GameState *gameState, const GameUI *gameUI);
void RenderBitmap(GameState *gameState, Bitmap *bitmap, RenderMode mode, Transform *transform, z::vec4 color = z::vec4(1));
void RenderParticles(GameState *gameState);

void LoadTexture(Bitmap *bitmap);
// NOTE(Charly): Cleanup GPU memory
void ReleaseTexture(Bitmap *bitmap);

void RenderText(char *text, z::vec2 pos, z::vec4 color, GameState *state, ObjectType type);
void RenderMesh(const Mesh *mesh, z::mat3 projectionMatrix);

z::mat3 GetTransformMatrix(RenderMode renderMode, Transform *transform);
z::mat3 GetProjectionMatrix(RenderMode renderMode, GameState *gameState);
#endif // RELWARB_RENDERER_H
