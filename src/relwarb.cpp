#include "relwarb.h"
#include "relwarb_math.h"
#include "relwarb_renderer.h"
#include "relwarb_opengl.h"

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
    gameState->worldSize = Vec2(20.f, 20.f / ratio);

    Mat4 worldMat = {0};
    worldMat[0][0] = (gameState->viewportSize.x / 2.f) / (gameState->worldSize.x / 2.f);
    worldMat[1][1] = (gameState->viewportSize.y / 2.f) / (gameState->worldSize.y / 2.f);
    worldMat[2][2] = 1.f;
    worldMat[3][3] = 1.f;
    worldMat[3][1] = -gameState->viewportSize.y / 2.f;
    gameState->worldMatrix = worldMat;

    gameState->gravity = Vec2(0.f, -9.8f);

	Vec2 halfSize = gameState->worldSize * 0.5f;

    Entity* entity0 = CreateEntity(gameState, Vec2(0, gameState->worldSize.y / 2.f));
    
    Bitmap* bitmap = CreateBitmap(gameState);
    bitmap->entityID = entity0->id;
    LoadBitmapData("assets/smiley.png", bitmap);

	uint8 tiles_indices[] = { 1 };
	RenderingPattern* pattern = CreateRenderingPattern(gameState, Vec2(1.f, 1.f), tiles_indices, &bitmap, 1);

    RigidBody* body = CreateRigidBody(gameState, 0.1f);
    body->entityID = entity0->id;
    body->invMass = .1f;

    AddRenderingPatternToEntity(entity0, pattern);
    AddRigidBodyToEntity(entity0, body);

	Bitmap* textures[10];
    textures[7] = CreateBitmap(gameState);
    LoadBitmapData("assets/corner_topleft.png", textures[7]);
	textures[8] = CreateBitmap(gameState);
	LoadBitmapData("assets/horizontal_up.png", textures[8]);
	textures[9] = CreateBitmap(gameState);
	LoadBitmapData("assets/corner_topright.png", textures[9]);
	textures[4] = CreateBitmap(gameState);
	LoadBitmapData("assets/vertical_left.png", textures[4]);
	textures[6] = CreateBitmap(gameState);
	LoadBitmapData("assets/vertical_right.png", textures[6]);
	textures[1] = CreateBitmap(gameState);
	LoadBitmapData("assets/corner_bottomleft.png", textures[1]);
	textures[2] = CreateBitmap(gameState);
	LoadBitmapData("assets/horizontal_down.png", textures[2]);
	textures[3] = CreateBitmap(gameState);
	LoadBitmapData("assets/corner_bottomright.png", textures[3]);

	uint8 horiz_indices[] = {	7, 8, 9,
								1, 2, 3 };
	Bitmap* horiz_bitmaps[] = { textures[horiz_indices[0]], textures[horiz_indices[1]], textures[horiz_indices[2]],
								textures[horiz_indices[3]], textures[horiz_indices[4]], textures[horiz_indices[5]] };
	uint8 vert_indices[] = {	7, 9,
								4, 6,
								1, 3 };
	Bitmap* vert_bitmaps[] = {  textures[vert_indices[0]], textures[vert_indices[1]],
								textures[vert_indices[2]], textures[vert_indices[3]],
								textures[vert_indices[4]], textures[vert_indices[5]] };

	RenderingPattern* horizPattern = CreateRenderingPattern(gameState, Vec2(3, 2), horiz_indices, horiz_bitmaps, 6);
	RenderingPattern* vertPattern = CreateRenderingPattern(gameState, Vec2(2, 3), vert_indices, vert_bitmaps, 6);

    RectangularShape* horizontalBoundary = CreateShape(gameState, Vec2(gameState->worldSize.x, 2.f));
    RectangularShape* verticalBoundary = CreateShape(gameState, Vec2(2.f, gameState->worldSize.y));

    Entity* floor = CreateEntity(gameState, Vec2(0.f, 1.f));
    AddRectangularShapeToEntity(floor, horizontalBoundary);
    AddRenderingPatternToEntity(floor, horizPattern);

    Entity* ceiling = CreateEntity(gameState, Vec2(0.f, gameState->worldSize.y - 1.f));
	AddRectangularShapeToEntity(ceiling, horizontalBoundary);
	AddRenderingPatternToEntity(ceiling, horizPattern);

    Entity* leftWall = CreateEntity(gameState, Vec2(-halfSize.x + 1.f, halfSize.y));
	AddRectangularShapeToEntity(leftWall, verticalBoundary);
	AddRenderingPatternToEntity(leftWall, vertPattern);

    Entity* rightWall = CreateEntity(gameState, Vec2(halfSize.x - 1.f, halfSize.y));
	AddRectangularShapeToEntity(rightWall, verticalBoundary);
	AddRenderingPatternToEntity(rightWall, vertPattern);

    RectangularShape* platformBoundary = CreateShape(gameState, Vec2(gameState->worldSize.x * 0.125f, 2.f));

    Entity* leftPlatform = CreateEntity(gameState, Vec2(-halfSize.x * 0.5f, halfSize.y * 0.5f));
	AddRectangularShapeToEntity(leftPlatform, horizontalBoundary);
	AddRenderingPatternToEntity(leftPlatform, horizPattern);

    Entity* rightPlatform = CreateEntity(gameState, Vec2(halfSize.x * 0.5f, halfSize.y * 0.5f));
	AddRectangularShapeToEntity(rightPlatform, horizontalBoundary);
	AddRenderingPatternToEntity(rightPlatform, horizPattern);

    Entity* centerPlatform = CreateEntity(gameState, Vec2(0.f, halfSize.y));
	AddRectangularShapeToEntity(centerPlatform, horizontalBoundary);
	AddRenderingPatternToEntity(centerPlatform, horizPattern);
}

void UpdateGame(GameState* gameState)
{
	// TODO(Charly): How do we retrieve actual characters ?
	// NOTE(Thomas): Actual like in "the one really moving/played" or actual like in a french mistranslation of 'current' ?
	gameState->onEdge = false;
	// if (gameState->controller.moveLeft)   gameState->character.posX -= 0.02f;
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
	UpdateWorld(gameState, 1.f / 60.f);
}

// TODO(Charly): Move this in renderer ? 
void RenderGame(GameState* gameState)
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

Entity* CreateEntity(GameState* gameState, Vec2 p, Vec2 dp, Vec2 ddp)
{
	EntityID id = gameState->nbEntities++;
	Assert(id < WORLD_SIZE);
	Entity* result = &gameState->entities[id];
	*result = {0};
	result->id = id;
	result->p = p;
	result->dp = dp;
	result->ddp = ddp;

	return result;
}

RectangularShape* CreateShape(GameState* gameState, Vec2 size_, Vec2 offset_)
{
	ComponentID id = gameState->nbShapes++;
	Assert(id < WORLD_SIZE);
	RectangularShape* result = &gameState->shapes[id];
	result->id = id;

	result->size = size_;
	result->offset = offset_;

	return result;
}

Bitmap* CreateBitmap(GameState* gameState)
{
	ComponentID id = gameState->nbBitmaps++;
	Assert(id < WORLD_SIZE);
	Bitmap* result = &gameState->bitmaps[id];
	result->id = id;

	return result;
}
