#ifndef RELWARB_H
#define RELWARB_H

#include "relwarb_defines.h"
#include "relwarb_world_sim.h"
#include "relwarb_renderer.h"
#include "relwarb_input.h"

#define WORLD_SIZE      1024

// NOTE(Charly): This is garbage code
//               controller0 -> keyboard
//               controller{1 - 4} -> xbox controllers
#define MAX_CONTROLLERS 5
#define MAX_PLAYERS     4

#define MAX_PARTICLE_SYSTEMS 1024

struct Controller
{
    // TODO(Thomas): Use
    uint32 controlledEntityID;

    // TODO(Thomas): Use
    bool32 enabled = false;

    bool32 moveLeft = false;
    bool32 moveRight = false;

    bool32 jump = false;
    bool32 newJump = false;
    bool32 dash = false;
    bool32 newDash = false;
    bool32 mana = false;
    bool32 newMana = false;
};

// TODO(Charly): This should go somewhere else
struct Bitmap
{
    uint32 texture = 0;
    uint8* data;

    int width;
    int height;
};

enum GameMode
{
    GameMode_Game = 0,
    GameMode_Editor,
};

// NOTE(Charly): Store the current state of the game
struct GameState
{
    // TODO(Charly): View / Proj matrices
    // TODO(Charly): Do we want orthographic or perspective projection ?

    z::vec2 viewportSize;
    z::vec2 worldSize;

    z::mat4 projMatrix;
    z::mat4 worldMatrix;

    bool32 onEdge = false;

    Entity*             players[MAX_PLAYERS];
    uint32              nbPlayers = 0;

    Entity              entities[WORLD_SIZE];
    uint32              nbEntities = 0;

    RigidBody           rigidBodies[WORLD_SIZE];
    uint32              nbRigidBodies = 0;

    Shape               shapes[WORLD_SIZE];
    uint32              nbShapes = 0;

    // TODO(Thomas): Use different size for following, as number of Bitmap/Sprite/Pattern are different from number of entities
    Bitmap              bitmaps[WORLD_SIZE];
    uint32              nbBitmaps = 0;

    Sprite              sprites[WORLD_SIZE];
    uint32              nbSprites = 0;

    RenderingPattern    patterns[WORLD_SIZE];
    uint32              nbPatterns = 0;

    Controller          controllers[MAX_CONTROLLERS];
    uint32              nbControllers = 0;

    ParticleSystem      particleSystems[MAX_PARTICLE_SYSTEMS];
    Bitmap              particleBitmap;

    Bitmap              hudHealth[3];
    Bitmap              hudMana[2];

    z::vec2 gravity;

    GameMode mode = GameMode_Game;

    // NOTE(Charly): Windows coordinates
    z::vec2 cursor;
    InputState keyStates[Key_Count];
    InputState buttonStates[Button_Count];
};


// NOTE(Charly): Initialize all the game logic related stuff here
// TODO(Charly): This should probably be exposed to the scripting
void InitGame(GameState* gameState);

// NOTE(Charly): Do all game logic and computations here
// TODO(Charly): This should probably be exposed to the scripting
void UpdateGame(GameState* gameState, real32 dt);

// NOTE(Charly): Render the current state of the game
// TODO(Charly): Maybe we need to pass the delta time for some
//               time dependent effects ?
void RenderGame(GameState* gameState, real32 dt);

// NOTE(Thomas): Render HUD (atm only in GameMode_Game)
void RenderHUD(GameState* gameState);

// TODO(Charly): This should go somewhere else
// NOTE(Charly): bitmap must not be null, otherwise UB
void LoadBitmapData(const char* filename, Bitmap* bitmap);
void ReleaseBitmapData(Bitmap* bitmap);

Entity* CreateEntity(GameState* gameState,
                     EntityType type,
                     z::vec2 p,
                     z::vec2 dp = z::vec2(0),
                     z::vec2 ddp = z::vec2(0));

Bitmap* CreateBitmap(GameState* gameState);
// XXXComponent* CreateXXXComponent(GameState* gameState);

z::vec2 ViewportToWorld(GameState* state, z::vec2 in);

// NOTE(Charly): Initialize a transform with origin at (0.5, 0.5) instead of (0, 0)
Transform GetWorldTransform(z::vec2 position);

#endif // RELWARB_H
