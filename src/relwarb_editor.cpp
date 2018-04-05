#include "relwarb_editor.h"

#include "relwarb_defines.h"
#include "relwarb_opengl.h"
#include "relwarb_debug.h"
#include "relwarb.h"

#include <malloc.h>

enum SnapMode
{
	SnapMode_Corner = 0,
	SnapMode_Center,
	SnapMode_Count
};

global_variable SnapMode snapMode;
global_variable int      selectedBitmap;

global_variable int  width;
global_variable int  height;
global_variable int* tiles;

internal int GetTileIndex(z::vec2 worldPos)
{
	int x = z::Clamp(z::Floor(worldPos.x) + width / 2, 0, width - 1);
	int y = z::Clamp(z::Floor(worldPos.y) + height / 2, 0, height - 1);

	int result = y * width + x;
	return result;
}

internal int GetTileValue(z::vec2 worldPos)
{
	int result = tiles[GetTileIndex(worldPos)];
	return result;
}

internal void AddBitmap(z::vec2 worldPos)
{
	int* tile = tiles + GetTileIndex(worldPos);
	Log(Log_Debug, "%d", *tile);
	if (*tile != selectedBitmap)
	{
		*tile = selectedBitmap;
	}
}

internal void RemoveBitmap(z::vec2 worldPos)
{
	int* tile = tiles + GetTileIndex(worldPos);
	if (*tile != -1)
	{
		*tile = -1;
	}
}

void UpdateEditor()
{
	if (tiles == nullptr)
	{
		width  = (int)state->worldSize.x;
		height = (int)state->worldSize.y;
		tiles  = (int*)malloc(width * height * sizeof(int));
		Assert(tiles);

		for (int i = 0; i < width * height; ++i)
		{
			tiles[i] = -1;
		}
	}

	if (IsKeyRisingEdge(Key_C))
	{
		snapMode = (SnapMode)((snapMode + 1) % SnapMode_Count);
	}

	if (IsKeyRisingEdge(Key_Tab))
	{
		selectedBitmap = (selectedBitmap + 1) % state->nbBitmaps;
	}

	if (IsMouseButtonPressed(MouseButton_Left))
	{
		AddBitmap(GetCursorWorldPosition());
	}

	if (IsMouseButtonPressed(MouseButton_Right))
	{
		RemoveBitmap(GetCursorWorldPosition());
	}
}

void RenderEditor()
{
	glViewport(0, 0, state->viewportSize.x, state->viewportSize.y);

	glClearColor(0.3f, 0.8f, 0.7f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Transform t;
	t.origin = z::Vec2(0.5, 0.5);

	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			int tile = tiles[j * width + i];
			if (tile != -1)
			{
				t.position = z::Vec2((i - width / 2) + 0.5, (j - height / 2) + 0.5);
				RenderBitmap(&state->bitmaps[tile], RenderMode_World, &t);
			}
			++tile;
		}
	}

	// NOTE(Charly): Render attached bitmap
	{
		z::vec2 cursor = GetCursorWorldPosition();
		t.position     = z::Vec2(z::Floor(cursor.x) + 0.5, z::Floor(cursor.y) + 0.5);
		RenderBitmap(&state->bitmaps[selectedBitmap], RenderMode_World, &t);
	}
	/*
	    {
	        z::vec2 cursor = ViewportToWorld(state->cursor);
	        t.position = z::vec2(cursor.x, cursor.y);
	        RenderBitmap(&state->bitmaps[selectedBitmap], &t);
	    }*/
}
