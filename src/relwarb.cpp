#include "relwarb.h"
#include "relwarb_math.h"
#include "relwarb_renderer.h"
#include "relwarb_opengl.h"
#include "relwarb_debug.h"
#include "relwarb_parser.h"

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

	LoadMapFile(gameState, "config/base_map.ini");
    
    Bitmap* bitmap = CreateBitmap(gameState);
    LoadBitmapData("assets/smiley.png", bitmap);

	uint8 tiles_indices[] = { 1 };
	
	Shape* heroShape = CreateShape(gameState, Vec2(1, 1));
	RenderingPattern* heroPattern = CreateRenderingPattern(gameState, Vec2(1.f, 1.f), tiles_indices, 1, &bitmap);

	CreatePlayerEntity(gameState, Vec2(0, 0), heroPattern, heroShape, &gameState->controllers[0]);

}

void UpdateGame(GameState* gameState, real32 dt)
{
	// TODO(Charly): How do we retrieve actual characters ?
	// NOTE(Thomas): Actual like in "the one really moving/played" or actual like in a french mistranslation of 'current' ?
	// NOTE(Charly): The first one I guess
	
	// if (gameState->controller.moveLeft)   gameState->character.posX -= 0.02f
	// if (gameState->controller.moveRight)  gameState->character.posX += 0.02f;
	// if (gameState->controller.moveDown)   gameState->character.posY -= 0.02f;
	// if (gameState->controller.moveUp)     gameState->character.posY += 0.02f;
	// if (gameState->entities[0].shape.posX < -1.f)
	// {
	//    gameState->entities[0].shape.posX = -1.f;
	//    gameState->onEdge = true;
	// }
	// if (gameState->entities[0].shape.posX > 1.f - gameState->entities[0].shape.sizeX)
	// {
	// gameState->entities[0].shape.posX = 1.f - gameState->entities[0].shape.sizeX;
	// gameState->onEdge = true;
	// }
	// if (gameState->entities[0].shape.posY < -1.f)
	// {
	// gameState->entities[0].shape.posY = -1.f;
	// gameState->onEdge = true;
	// }
	// if (gameState->entities[0].shape.posY > 1.f - gameState->entities[0].shape.sizeY)
	// {
	// gameState->entities[0].shape.posY = 1.f - gameState->entities[0].shape.sizeY;
	// gameState->onEdge = true;
	// }
	UpdateWorld(gameState, dt);
}

// TODO(Charly): Move this in renderer ? 
void RenderGame(GameState* gameState, real32 dt)
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
