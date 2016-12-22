#ifndef RELWARB_H
#define RELWARB_H

#include "relwarb_defines.h"
#include "relwarb_math.h"
#include "relwarb_world_sim.h"
#include "relwarb_renderer.h"
#include "relwarb_input.h"

#define WORLD_SIZE		1024

// NOTE(Charly): This is garbage code
//               controller0 -> keyboard
//               controller{1 - 4} -> xbox controllers 
#define MAX_CONTROLLERS	5
#define MAX_PLAYERS		4

struct Controller
{
	// TODO(Thomas): Use
	uint32 controlledEntityID;

	// TODO(Thomas): Use
	bool32 enabled;

    bool32 moveLeft;
    bool32 moveRight;

    bool32 jump;
	bool32 newJump;
	bool32 dash;
	bool32 newDash;
	bool32 mana;
	bool32 newMana;
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

    Vec2 viewportSize;
    Vec2 worldSize;

    Mat4 projMatrix;
    Mat4 worldMatrix;

    bool32 onEdge;

	Entity*				players[MAX_PLAYERS];
	uint32				nbPlayers;

	Entity				entities[WORLD_SIZE];
	uint32				nbEntities;

	RigidBody			rigidBodies[WORLD_SIZE];
    uint32              nbRigidBodies;

	Shape	            shapes[WORLD_SIZE];
    uint32              nbShapes;

	// TODO(Thomas): Use different size for following, as number of Bitmap/Sprite/Pattern are different from number of entities
	Bitmap				bitmaps[WORLD_SIZE];
    uint32              nbBitmaps;

	Sprite				sprites[WORLD_SIZE];
	uint32				nbSprites;

	RenderingPattern	patterns[WORLD_SIZE];
	uint32				nbPatterns;

    Controller          controllers[MAX_CONTROLLERS];
    uint32              nbControllers;

	Bitmap				hudHealth[3];
	Bitmap				hudMana[2];

    Vec2 gravity;

	GameMode mode;

	// NOTE(Charly): Windows coordinates
	Vec2 cursor;
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

Entity* CreateEntity(GameState* gameState, EntityType type, Vec2 p, Vec2 dp = Vec2(0), Vec2 ddp = Vec2(0));

Bitmap* CreateBitmap(GameState* gameState);
// XXXComponent* CreateXXXComponent(GameState* gameState);

Vec2 ViewportToWorld(GameState* state, Vec2 in);

#endif // RELWARB_H
