#ifndef RELWARB_H
#define RELWARB_H

#include "relwarb_defines.h"
#include "relwarb_math.h"

#define WORLD_SIZE	1024

// NOTE(THOMAS): I'll go for adjective forms here, as it shows properties of entities
enum ComponentFlag
{
	ComponentFlag_Physical = 00000001,
	ComponentFlag_Collisional = 00000010,
	ComponentFlag_Graphical = 00000100,
};

// NOTE(THOMAS): Adjective forms also as it shows type of data stored, or a name (as now) relative to data stored ?
enum ComponentType
{
	ComponentType_PhysicsData		= 0 ,
	ComponentType_CollisionShape		,
	ComponentType_GraphicsData			,

	ComponentType_NbTypes
};

// NOTE(Charly): This is garbage code
struct Controller
{
    bool32 moveUp;
    bool32 moveDown;
    bool32 moveLeft;
    bool32 moveRight;

    Controller() = default;
};

// TODO(Charly): This should go somewhere else
struct Bitmap
{
    uint32 texture = 0;
    uint8* data;

    int width;
    int height;

	Bitmap() = default;
};

struct PhysicsData
{
	Vec2 p;     // NOTE(Charly): Linear position
	Vec2 dp;    // NOTE(Charly): Linear velocity
	Vec2 ddp;   // NOTE(Charly): Linear acceleration

    // TODO(Charly): Angular stuff ?

	PhysicsData() = default;
	PhysicsData(Vec2 p_, Vec2 dp_ = Vec2(0), Vec2 ddp_ = Vec2(0))
		:p(p_), dp(dp_), ddp(ddp_) {}
};

struct RectangularShape
{
	Vec2 size;
	Vec2 offset;

    RectangularShape() = default;
	RectangularShape(Vec2 size_, Vec2 offset_ = Vec2(0))
		:size(size_), offset(offset_) {}
};

struct GameState;
struct Entity
{
	uint32 id;
	uint32 flags;

	uint32 indices[ComponentType_NbTypes];

	// TODO(Thomas): Handle flags a nicer way. That way :
	//					1) we have to do a constructor for each combination
	//					2) flags are statically defined
	//				 Use something like 'void addComponent(GameState gameState, ComponentType type, void * data, ComponentFlag flag)' ?
	Entity(GameState * gameState, PhysicsData physics, RectangularShape shape, Bitmap bitmap);

	Entity() = default;
};

// NOTE(Charly): Store the current state of the game
struct GameState
{
    // TODO(Charly): View / Proj matrices
    // TODO(Charly): Do we want orthographic or perspective projection ?
    Controller controller;

    uint32 renderWidth;
    uint32 renderHeight;

    bool32 onEdge;

	uint32 nbEntities;
	Entity entities[WORLD_SIZE];

	// For each components, number of entities already stored
	// NOTE(THOMAS): Don't like the name ...
	uint32 nbComponentData[ComponentType_NbTypes];

	PhysicsData			physics[WORLD_SIZE];
	RectangularShape	shapes[WORLD_SIZE];
	Bitmap				bitmaps[WORLD_SIZE];

	GameState()
		:nbEntities(0), onEdge(false)
	{
		for (uint32 i = 0; i < ComponentType_NbTypes; ++i)
			nbComponentData[i] = 0;
	}
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
void LoadImage(const char* filename, Bitmap* bitmap);
void ReleaseImage(Bitmap* bitmap);

#endif // RELWARB_H
