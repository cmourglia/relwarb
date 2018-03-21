#include "relwarb.h"

#include <ctime>

#include "relwarb_renderer.h"
#include "relwarb_opengl.h"
#include "relwarb_debug.h"
#include "relwarb_parser.h"
#include "relwarb_editor.h"

#include "zmath.hpp"

// TODO(Charly): This should go somewhere else.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void InitGame(GameState* gameState)
{
    z::SeedRNG((unsigned)time(nullptr));
    LoadBitmapData("assets/sprites/health_full.png", &gameState->hudHealth[0]);
    LoadBitmapData("assets/sprites/health_mid.png", &gameState->hudHealth[1]);
    LoadBitmapData("assets/sprites/health_none.png", &gameState->hudHealth[2]);
    LoadBitmapData("assets/sprites/mana_full.png", &gameState->hudMana[0]);
    LoadBitmapData("assets/sprites/mana_none.png", &gameState->hudMana[1]);
    //LoadBitmapData("assets/sprites/smiley.png", &gameState->particleBitmap);
    LoadBitmapData("assets/sprites/particle.png", &gameState->particleBitmap);

    InitializeRenderer(gameState);
    gameState->projMatrix = z::Ortho(-gameState->viewportSize.x() / 2, gameState->viewportSize.x() / 2,
                                     -gameState->viewportSize.y() / 2, gameState->viewportSize.y() / 2);
    real32 ratio = gameState->viewportSize.x() / gameState->viewportSize.y();

    // NOTE(Thomas): Seems like worldSize should be the one we define, and windows size/viewport size are computed accordingly.
    gameState->worldSize = z::vec2(48, 24);

    z::mat4 worldMat(1);
    worldMat[0][0]         = (gameState->viewportSize.x() / 2.f) / (gameState->worldSize.x() / 2.f);
    worldMat[1][1]         = (gameState->viewportSize.y() / 2.f) / (gameState->worldSize.y() / 2.f);
    gameState->worldMatrix = worldMat;

    gameState->gravity = z::vec2(0.f, -1.f);

    z::vec2 halfSize = gameState->worldSize * 0.5f;

    // NOTE(Thomas): Must be before any other data is created, as indices are hardcoded in the file
    LoadMapFile(gameState, "config/base_map.ini");

    Bitmap* bitmap_p1[2];
    bitmap_p1[0] = CreateBitmap(gameState);
    LoadBitmapData("assets/sprites/p1_stand.png", bitmap_p1[0]);
    bitmap_p1[1] = CreateBitmap(gameState);
    LoadBitmapData("assets/sprites/p1_stand2.png", bitmap_p1[1]);

    Sprite* sprite_p1 = CreateTimeSprite(gameState, 2, bitmap_p1, 0.5f);

    Bitmap* bitmap_p2[2];
    bitmap_p2[0] = CreateBitmap(gameState);
    LoadBitmapData("assets/sprites/p2_stand.png", bitmap_p2[0]);
    bitmap_p2[1] = CreateBitmap(gameState);
    LoadBitmapData("assets/sprites/p2_stand2.png", bitmap_p2[1]);

    Sprite* sprite_p2 = CreateTimeSprite(gameState, 2, bitmap_p2, 0.5f);

    uint8 tiles_indices[] = { 1 };

    Shape* heroShape = CreateShape(gameState, z::vec2(1.f, 1.5f));

    RenderingPattern* heroPattern1 = CreateUniqueRenderingPattern(gameState, sprite_p1);
    CreatePlayerEntity(gameState, z::vec2(-2, -2), heroPattern1, heroShape, &gameState->controllers[0]);

    RenderingPattern* heroPattern2 = CreateUniqueRenderingPattern(gameState, sprite_p2);
    CreatePlayerEntity(gameState, z::vec2(2, -2), heroPattern2, heroShape, &gameState->controllers[1]);

    LoadBitmapData("assets/sprites/corner_topleft.png", CreateBitmap(gameState));
    LoadBitmapData("assets/sprites/horizontal_up.png", CreateBitmap(gameState));
    LoadBitmapData("assets/sprites/corner_topright.png", CreateBitmap(gameState));
    LoadBitmapData("assets/sprites/vertical_left.png", CreateBitmap(gameState));
    LoadBitmapData("assets/sprites/vertical_right.png", CreateBitmap(gameState));
    LoadBitmapData("assets/sprites/corner_bottomleft.png", CreateBitmap(gameState));
    LoadBitmapData("assets/sprites/horizontal_down.png", CreateBitmap(gameState));
    LoadBitmapData("assets/sprites/corner_bottomright.png", CreateBitmap(gameState));
    LoadBitmapData("assets/sprites/horizontal_up.png", CreateBitmap(gameState));
}

void UpdateGame(GameState* gameState, real32 dt)
{
    // NOTE(Charly): Toggle game mode on presses
    if (gameState->keyStates[Key_F1].stateChange && gameState->keyStates[Key_F1].clicked)
    {
        gameState->mode = gameState->mode == GameMode_Game ? GameMode_Editor : GameMode_Game;
    }

    local_persist bool32 slowDownTime = false;
    if (InputUpFront(&gameState->keyStates[Key_T]))
    {
        slowDownTime ^= true;
    }

    if (slowDownTime) dt *= 0.1f;

    switch (gameState->mode)
    {
        case GameMode_Game:
        {
            if (InputUpFront(&gameState->buttonStates[Button_Left]))
            {
                SpawnParticleSystem(gameState, ViewportToWorld(gameState, gameState->cursor));
            }

            UpdateGameLogic(gameState, dt);
            UpdateWorld(gameState, dt);

            // Update particle system
#if 0
            for (uint32 particleSpawnIndex = 0; particleSpawnIndex < 1; ++particleSpawnIndex)
            {
                Particle* particle = gameState->particles + gameState->nextParticle++;
                if (gameState->nextParticle >= MAX_PARTICLES)
                {
                    gameState->nextParticle = 0;
                }

                particle->p = z::vec2(z::GenerateRandBetween(-0.5, 0.5), 3);
                particle->dp = z::vec2(z::GenerateRandBetween(-2.5, 2.5), z::GenerateRandBetween(9, 11));
                particle->color = z::vec4(1, 1, 1, 1);
                particle->dcolor = z::vec4(0, 0, 0, -0.5);
            }

            for (uint32 particleIdx = 0; particleIdx < MAX_PARTICLES; ++particleIdx)
            {
                const z::vec2 gravity(0, -10);
                Particle* particle = gameState->particles + particleIdx;
                particle->dp += gravity * dt;
                particle->p += particle->dp * dt;
                particle->color += particle->dcolor * dt;
            }
#endif
        } break;

        case GameMode_Editor:
        {
            UpdateEditor(gameState);
        } break;

        default:
        {
            Assert(!"Wrong code path");
        }
    }
}

// TODO(Charly): Move this in renderer ?
void RenderGame(GameState* gameState, real32 dt)
{
    switch (gameState->mode)
    {
        case GameMode_Game:
        {
            for (uint32 elementIdx = 0; elementIdx < gameState->nbEntities; ++elementIdx)
            {
                Entity* entity = &gameState->entities[elementIdx];
                if (EntityHasComponent(entity, ComponentFlag_Renderable))
                {
                    RenderingPattern* pattern = entity->pattern;
                    z::vec2 pos(entity->p);

                    Transform transform = GetWorldTransform(entity->p);

                    // TODO(Thomas): Handle drawing size with a drawing size
                    if (EntityHasComponent(entity, ComponentFlag_Collidable))
                    {
                        transform.size = entity->shape->size;
                        transform.origin += entity->shape->offset;
                    }

                    if (entity->entityType == EntityType_Player)
                    {
                        transform.orientation = entity->orientation < 0.f ? -1 : 1;
                    }

                    RenderPattern(pattern, &transform, entity->shape->size);
                }
            }

            RenderHUD(gameState);
            RenderText("Hello, World", z::vec2(0.0, 0.0), z::vec4(1, 0, 0, 1), gameState, ObjectType_Debug);
            RenderText("I am another test text !", z::vec2(0.0, 0.1), z::vec4(0, 1, 0, 1), gameState, ObjectType_Debug);
            RenderText("abcdefghijklmnopqrstuvwxyz 0123456789", z::vec2(0.0, 0.2), z::vec4(0, 0, 1, 1), gameState, ObjectType_Debug);

            char fps[128];
            snprintf(fps, 128, "dt: %.3f, fps: %.3f", dt, 1/dt);
            RenderText(fps, z::vec2(0.8, 0), z::vec4(0, 0, 0, 1), gameState, ObjectType_Debug);

            FlushRenderQueue(gameState);
        } break;

        case GameMode_Editor:
        {
            RenderEditor(gameState);
        } break;

        default:
        {
            Assert(!"Wrong code path");
        }
    }
}

void RenderHUD(GameState* gameState)
{
    real32 ratio = gameState->viewportSize.x() / gameState->viewportSize.y();

    Transform transform;

    z::vec2 onScreenPos = z::vec2(0.04, 0.04);
    for (uint32 i = 0; i < gameState->nbPlayers; ++i)
    {
        Entity* player = gameState->players[i];

        transform.size = z::vec2(0.0625, 0.0625 * ratio);

        // Avatar
        transform.position = onScreenPos;
        RenderBitmap(player->avatar, RenderMode_ScreenRelative, &transform);

        // Health
        transform.size = z::vec2(0.025f, 0.025f*ratio);
        z::vec2 healthPos = onScreenPos + z::vec2(0.075f, 0.f);
        for (uint32 hp = 0; hp < player->max_health; hp+=2)
        {
            transform.position = healthPos;
            if (hp < player->health)
            {
                if (hp + 1 < player->health)
                {
                    RenderBitmap(&gameState->hudHealth[0], RenderMode_ScreenRelative, &transform);
                }
                else
                {
                    RenderBitmap(&gameState->hudHealth[1], RenderMode_ScreenRelative, &transform);
                }
            }
            else
            {
                RenderBitmap(&gameState->hudHealth[2], RenderMode_ScreenRelative, &transform);
            }

            healthPos.x() += 0.0255f;
        }

        // Mana
        z::vec2 manaPos = onScreenPos + z::vec2(0.075f, 0.0375f*ratio);
        for (uint32 mp = 0; mp < player->max_mana; ++mp)
        {
            transform.position = manaPos;
            if (mp < player->mana)
            {
                RenderBitmap(&gameState->hudMana[0], RenderMode_ScreenRelative, &transform);
            }
            else
            {
                RenderBitmap(&gameState->hudMana[1], RenderMode_ScreenRelative, &transform);
            }
            manaPos.x() += 0.0255f;
        }

        onScreenPos.x() += 0.24;
    }
}

void LoadBitmapData(const char* filename, Bitmap* bitmap)
{
    // NOTE(Charly): Get images bottom-up
    stbi_set_flip_vertically_on_load(true);
    int n;
    bitmap->data = stbi_load(filename, &bitmap->width, &bitmap->height, &n, 4);
    Assert(bitmap->data);

    LoadTexture(bitmap);
}

void ReleaseBitmapData(Bitmap* bitmap)
{
    ReleaseTexture(bitmap);
    stbi_image_free(bitmap->data);
}

Entity* CreateEntity(GameState* gameState, EntityType type, z::vec2 p, z::vec2 dp, z::vec2 ddp)
{
    EntityID id = gameState->nbEntities++;
    Assert(id < WORLD_SIZE);

    Entity* result = &gameState->entities[id];
    *result = {0};

    result->id = id;

    result->p = p;
    result->dp = dp;
    result->ddp = ddp;

    result->entityType = type;

    return result;
}

Bitmap* CreateBitmap(GameState* gameState)
{
    ComponentID id = gameState->nbBitmaps++;
    Assert(id < WORLD_SIZE);
    Bitmap* result = &gameState->bitmaps[id];

    return result;
}

z::vec2 ViewportToWorld(GameState* state, z::vec2 in)
{
    // [0, viewport] -> [0, 1], origin top left
    z::vec2 result = in / state->viewportSize;
    // [0, 1] -> [0, 1] origin bot left
    result.y() = 1 - result.y();
    // [0, 1] -> [-0.5, 0.5]
    result = result - z::vec2(0.5);
    // [-0.5, 0.5] -> [-world / 2, world / 2]
    result = result * state->worldSize;

    return result;
}

Transform GetWorldTransform(z::vec2 pos)
{
    Transform result;
    result.position = pos;
    result.origin = z::vec2(0.5);

    return result;
}
