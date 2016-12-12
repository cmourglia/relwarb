#ifndef RELWARB_H
#define RELWARB_H

#include "relwarb_defines.h"
#include "relwarb_math.h"
#include "relwarb_world_sim.h"

#define WORLD_SIZE	1024

// NOTE(Charly): This is garbage code
//               controller0 -> keyboard
//               controller{1 - 4} -> xbox controllers 
#define MAX_CONTROLLERS 5
struct Controller : public Component
{
    bool32 moveUp;
    bool32 moveDown;
    bool32 moveLeft;
    bool32 moveRight;

    bool32 jump;
};

// TODO(Charly): This should go somewhere else
struct Bitmap : public Component
{
    uint32 texture = 0;
    uint8* data;

    int width;
    int height;
};

struct RectangularShape : public Component
{
	Vec2 size;
	Vec2 offset;

    inline RectangularShape() {}
	inline RectangularShape(Vec2 size_, Vec2 offset_ = Vec2(0))
		:size(size_), offset(offset_) {}
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

	uint32 nbEntities;
	Entity entities[WORLD_SIZE];

	RigidBody			rigidBodies[WORLD_SIZE];
    uint32              nbRigidBodies;

	RectangularShape	shapes[WORLD_SIZE];
    uint32              nbShapes;

	Bitmap				bitmaps[WORLD_SIZE];
    uint32              nbBitmaps;

    Controller          controller[MAX_CONTROLLERS];
    uint32              nbControllers;

    Vec2 gravity;
};

// NOTE(Charly): Initialize all the game logic related stuff here
// TODO(Charly): This should probably be exposed to the scripting
void InitGame(GameState* gameState);

// NOTE(Charly): Do all game logic and computations here
// TODO(Charly): This should probably be exposed to the scripting
void UpdateGame(GameState* gameState);

// NOTE(Charly): Render the current state of the game
// TODO(Charly): Maybe we need to pass the delta time for some
//               time dependent effects ?
void RenderGame(GameState* gameState);

// TODO(Charly): This should go somewhere else
// NOTE(Charly): bitmap must not be null, otherwise UB
void LoadBitmapData(const char* filename, Bitmap* bitmap);
void ReleaseBitmapData(Bitmap* bitmap);

Entity* CreateEntity(GameState* gameState);
void AddComponentToEntity(Entity* entity, ComponentID component, ComponentType type, ComponentFlag flag);

RectangularShape* CreateShape(GameState* gameState);
Bitmap* CreateBitmap(GameState* gameState);
// XXXComponent* CreateXXXComponent(GameState* gameState);



#endif // RELWARB_H
