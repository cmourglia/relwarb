#include "relwarb.h"
#include "relwarb_math.h"
#include "relwarb_renderer.h"
#include "relwarb_opengl.h"
#include "relwarb_debug.h"
#include "relwarb_parser.h"
#include "relwarb_editor.h"

// TODO(Charly): This should go somewhere else.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void InitGame(GameState* gameState)
{
    InitializeRenderer();
    gameState->projMatrix = Ortho(-gameState->viewportSize.x / 2, gameState->viewportSize.x / 2, 
                                  -gameState->viewportSize.y / 2, gameState->viewportSize.y / 2);
    real32 ratio = gameState->viewportSize.x / gameState->viewportSize.y;

	// NOTE(Thomas): Seems like worldSize should be the one we define, and windows size/viewport size are computed accordingly.
    gameState->worldSize = Vec2(32, 18);

    Mat4 worldMat = {0};
    worldMat[0][0] = (gameState->viewportSize.x / 2.f) / (gameState->worldSize.x / 2.f);
    worldMat[1][1] = (gameState->viewportSize.y / 2.f) / (gameState->worldSize.y / 2.f);
    worldMat[2][2] = 1.f;
    worldMat[3][3] = 1.f;
    //worldMat[3][1] = -gameState->viewportSize.y / 2.f;
    gameState->worldMatrix = worldMat;

    gameState->gravity = Vec2(0.f, -1.f);

	Vec2 halfSize = gameState->worldSize * 0.5f;

	//LoadMapFile(gameState, "config/base_map.ini");
    
    Bitmap* bitmap = CreateBitmap(gameState);
    LoadBitmapData("assets/smiley.png", bitmap);

	uint8 tiles_indices[] = { 1 };
	
	Shape* heroShape = CreateShape(gameState, Vec2(1, 1));
	RenderingPattern* heroPattern = CreateRenderingPattern(gameState, Vec2(1.f, 1.f), tiles_indices, 1, &bitmap);

	CreatePlayerEntity(gameState, Vec2(0, 0), heroPattern, heroShape, &gameState->controllers[0]);

    LoadBitmapData("assets/corner_topleft.png", CreateBitmap(gameState));
    LoadBitmapData("assets/horizontal_up.png", CreateBitmap(gameState));
    LoadBitmapData("assets/corner_topright.png", CreateBitmap(gameState));
    LoadBitmapData("assets/vertical_left.png", CreateBitmap(gameState));
    LoadBitmapData("assets/vertical_right.png", CreateBitmap(gameState));
    LoadBitmapData("assets/corner_bottomleft.png", CreateBitmap(gameState));
    LoadBitmapData("assets/horizontal_down.png", CreateBitmap(gameState));
    LoadBitmapData("assets/corner_bottomright.png", CreateBitmap(gameState));
    LoadBitmapData("assets/horizontal_up.png", CreateBitmap(gameState));
}

void UpdateGame(GameState* gameState, real32 dt)
{
	// NOTE(Charly): Toggle game mode on presses
	if (gameState->keyStates[Key_F1].stateChange && gameState->keyStates[Key_F1].clicked)
	{
		gameState->mode = gameState->mode == GameMode_Game ? GameMode_Editor : GameMode_Game;
	}

	switch (gameState->mode)
	{
		case GameMode_Game:
		{
			UpdateWorld(gameState, dt);
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
			glViewport(0, 0, gameState->viewportSize.x, gameState->viewportSize.y);

			glClearColor(0.3f, 0.8f, 0.7f, 0.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			for (uint32 elementIdx = 0; elementIdx < gameState->nbEntities; ++elementIdx)
			{
				Entity* entity = &gameState->entities[elementIdx];
				if (EntityHasFlag(entity, ComponentFlag_Renderable))
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

void LoadBitmapData(const char* filename, Bitmap* bitmap)
{
	// NOTE(Charly): Get images bottom-up
	stbi_set_flip_vertically_on_load(true);
	int n;
	bitmap->data = stbi_load(filename, &bitmap->width, &bitmap->height, &n, 4);
	Assert(bitmap->data);
}

void ReleaseBitmapData(Bitmap* bitmap)
{
	ReleaseBitmap(bitmap);
	stbi_image_free(bitmap->data);
}

Entity* CreateEntity(GameState* gameState, EntityType type, Vec2 p, Vec2 dp, Vec2 ddp)
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

Vec2 ViewportToWorld(GameState* state, Vec2 in)
{
	// [0, viewport] -> [0, 1], origin top left
	Vec2 result = in / state->viewportSize;
	// [0, 1] -> [0, 1] origin bot left
	result.y = 1 - result.y;
	// [0, 1] -> [-0.5, 0.5]
	result = result - Vec2(0.5);
	// [-0.5, 0.5] -> [-world / 2, world / 2]
	result = result * state->worldSize;

	return result;
}
