#include "relwarb_editor.h"

#include "relwarb_defines.h"
#include "relwarb_opengl.h"
#include "relwarb_debug.h"
#include "relwarb_math.h"
#include "relwarb.h"

#include <malloc.h>

enum SnapMode
{
	SnapMode_Corner = 0,
	SnapMode_Center,
	SnapMode_Count
};

global_variable SnapMode snapMode;
global_variable int selectedBitmap;

global_variable int width;
global_variable int height;
global_variable int* tiles;

internal int GetTileIndex(Vec2 worldPos)
{
	int x = (int)worldPos.x + width / 2;
	int y = (int)worldPos.y + height / 2;

	int result = y * width + x;
	return result;
}

internal int GetTileValue(Vec2 worldPos)
{
	int result = tiles[GetTileIndex(worldPos)];
	return result;
}

internal void AddBitmap(GameState* state, Vec2 worldPos)
{
	int* tile = tiles + GetTileIndex(worldPos);
	Log(Log_Debug, "%d", *tile);
	if (*tile == -1)
	{
		uint8 patternArray[] = { 1 };
		Bitmap* bitmap = state->bitmaps + selectedBitmap;
		RenderingPattern* pattern = CreateRenderingPattern(state, Vec2(1), patternArray, &bitmap, 1);
		CreateWallEntity(state, Vec2(Floor(worldPos.x) + 0.5, Floor(worldPos.y) + 0.5), pattern, &state->shapes[0]);

		*tile = state->nbEntities - 1;
	}
}

void UpdateEditor(GameState* state)
{
	if (tiles == nullptr)
	{
		width = (int)state->worldSize.x;
		height = (int)state->worldSize.y;
		tiles = (int*)malloc(width * height * sizeof(int));
		Assert(tiles);

		for (int i = 0; i < width * height; ++i)
		{
			tiles[i] = -1;
		}
	}

	if (InputUpFront(&state->keyStates[Key_C]))
	{
		snapMode = (SnapMode)((snapMode + 1) % SnapMode_Count);
	}

	if (InputUpFront(&state->keyStates[Key_Tab]))
	{
		selectedBitmap = (selectedBitmap + 1) % state->nbBitmaps;
	}

	if (state->buttonStates[Button_Left].clicked)
	{
		AddBitmap(state, ViewportToWorld(state, state->cursor));
	}
}

void RenderEditor(GameState* gameState)
{
	glViewport(0, 0, gameState->viewportSize.x, gameState->viewportSize.y);

	glClearColor(0.3f, 0.8f, 0.7f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (uint32 entityIdx = 0; entityIdx < gameState->nbEntities; ++entityIdx)
	{
		Entity* entity = &gameState->entities[entityIdx];
		if (EntityHasFlag(entity, ComponentFlag_Renderable) && entity->entityType == EntityType_Wall)
		{
			RenderingPattern* pattern = entity->pattern;
			Vec2 pos(entity->p);

			if (EntityHasFlag(entity, ComponentFlag_Movable))
			{
				// NOTE(Thomas): Do something maybe.
			}

			Transform transform;
			transform.offset = Vec2(0, 0);
			transform.position = pos;
			transform.scale = Vec2(1);
			transform.proj = gameState->projMatrix;
			transform.world = gameState->worldMatrix;

			RenderPattern(pattern, &transform);
		}
	}

	// NOTE(Charly): Render attached bitmap
	{
		Vec2 cursor = ViewportToWorld(gameState, gameState->cursor);

		Transform transform;
		transform.offset = Vec2(0, 0);
		transform.position = Vec2(Floor(cursor.x) + 0.5, Floor(cursor.y) + 0.5);
		transform.scale = Vec2(1);
		transform.proj = gameState->projMatrix;
		transform.world = gameState->worldMatrix;

		RenderBitmap(&gameState->bitmaps[selectedBitmap], &transform);
	}
}