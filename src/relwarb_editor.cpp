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
    int x = Clamp(Floor(worldPos.x) + width / 2, 0, width);
	int y = Clamp(Floor(worldPos.y) + height / 2, 0, height);

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
		//RenderingPattern* pattern = CreateRenderingPattern(state, Vec2(1), patternArray, 1, &bitmap);
		//CreateWallEntity(state, Vec2(Floor(worldPos.x) + 0.5, Floor(worldPos.y) + 0.5), pattern, &state->shapes[0]);

		*tile = selectedBitmap;
	}
}

internal void RemoveBitmap(GameState* state, Vec2 worldPos)
{
    int* tile = tiles + GetTileIndex(worldPos);
    if (*tile != -1)
    {
        uint8 patternArray[] = { 1 };
        Bitmap* bitmap = state->bitmaps + selectedBitmap;
        RenderingPattern* pattern = CreateRenderingPattern(state, Vec2(1), patternArray, 1, &bitmap);
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

    Transform t;
    t.offset = Vec2(0, 0);
    t.scale = Vec2(1);
    t.proj = gameState->projMatrix;
    t.world = gameState->worldMatrix;

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            int tile = tiles[j * width + i];
            if (tile != -1)
            {
                t.position = Vec2((i - width / 2) + 0.5, (j - height / 2) + 0.5);
                RenderBitmap(&gameState->bitmaps[tile], &t);
            }
            ++tile;
        }
    }

    // NOTE(Charly): Render attached bitmap
	{
		Vec2 cursor = ViewportToWorld(gameState, gameState->cursor);
        t.position = Vec2(Floor(cursor.x) + 0.5, Floor(cursor.y) + 0.5);
        RenderBitmap(&gameState->bitmaps[selectedBitmap], &t);
	}

    {
        Vec2 cursor = ViewportToWorld(gameState, gameState->cursor);
        t.position = Vec2(cursor.x, cursor.y);
        RenderBitmap(&gameState->bitmaps[selectedBitmap], &t);
    }
}