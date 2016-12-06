#ifndef RELWARB_H
#define RELWARB_H

#include "relwarb_defines.h"

#define WORLD_SIZE	1024

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

struct RectangularShape
{
	// NOTE(Thomas): Center of the shape
	real32 posX;
	real32 posY;

	real32 sizeX;
	real32 sizeY;

    RectangularShape() = default;
	RectangularShape(real32 pX, real32 pY, real32 width, real32 height)
		:posX(pX), posY(pY), sizeX(width), sizeY(height) {}
};

// TODO(Charly): More "generic" stuff ? Notion of entity, etc
struct Character
{
    // TODO(Charly): vec2
    // TODO(Charly): Decide where is the (0, 0) of the character
    real32 posX;
    real32 posY;

    real32 sizeX;
    real32 sizeY;

    // TODO(Charly): velocity
    // TODO(Charly): acceleration

    Bitmap bitmap;
};

struct WorldElement
{
	RectangularShape shape;

	// TODO(Thomas): velocity
	// TODO(Thomas): acceleration

	Bitmap bitmap;

    WorldElement() = default;
	WorldElement(RectangularShape shape_, Bitmap bitmap_)
		:shape(shape_), bitmap(bitmap_) {}
};

// NOTE(Charly): Store the current state of the game
struct GameState
{
    Controller controller;
    Character character;

	uint32 nbElements;
	WorldElement elements[WORLD_SIZE];

    uint32 renderWidth;
    uint32 renderHeight;

    bool32 onEdge;

    GameState() = default;
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
