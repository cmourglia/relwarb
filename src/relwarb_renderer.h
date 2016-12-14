#ifndef RELWARB_RENDERER_H
#define RELWARB_RENDERER_H

#include "relwarb_entity.h"
#include "relwarb_math.h"

struct Bitmap;
struct GameState;
struct Entity;

enum RenderingPatternType
{
	RenderingPattern_Unique = 0,
};

struct RenderingPattern
{
	Vec2 size;
	// NOTE(Thomas): Pattern to be rendered.
	//					For a start, I am thinking of storing something like 3x3, so that corners are fixed and middle values are repeated.
	//					It's not generic but it's the basic use, to be extended later on.
	//	e.g. :	7 8 9		7 8 8 8 9
	//			4 5 6	=>	4 5 5 5 6
	//			1 2 3		1 2 2 2 3
	uint8* pattern;
	// NOTE(Thomas): Arrays of tiles needed for the pattern
	Bitmap** bitmaps;

	RenderingPatternType patternType;
};

struct Transform
{
    Vec2 position;
    Vec2 offset;
    Vec2 scale;

    Mat4 proj;
    Mat4 world;

    // TODO(Charly). Orientation ?
};

void InitializeRenderer();
void ResizeRenderer(GameState* gameState);

RenderingPattern* CreateRenderingPattern(GameState* gameState, 
										 Vec2 size,
										 uint8* pattern,
										 uint8 nbBitmaps,
										 Bitmap** bitmaps, 
										 RenderingPatternType type = RenderingPattern_Unique);

void AddRenderingPatternToEntity(Entity* entity, RenderingPattern* pattern);

// Render the pattern at the position given in transform, and repeated to fit the given size
void RenderPattern(RenderingPattern* pattern, Transform* transform, Vec2 size = Vec2(0));

// TODO(Charly): x and y are given in opengl coordinates for now,
//               maybe this should change
void RenderBitmap(Bitmap* bitmap, Transform* transform);

// NOTE(Charly): Cleanup GPU memory
void ReleaseBitmap(Bitmap* bitmap);

void AddBitmapToEntity(Entity* entity, Bitmap* bitmap);

void RenderGrid(Vec2 resolution = Vec2(10, 10), Vec2 center = Vec2(0));

#endif // RELWARB_RENDERER_H