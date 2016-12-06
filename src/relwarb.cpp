#include "relwarb.h"
#include "relwarb_opengl.h"
#include "relwarb_math.h"

// TODO(Charly): This should go somewhere else.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Entity::Entity(GameState * gameState, PhysicsData physics, RectangularShape shape, Bitmap bitmap)
	:id(gameState->nbEntities++), flags(ComponentFlag_Physical | ComponentFlag_Collisional | ComponentFlag_Graphical)
{
	indices[ComponentType_PhysicsData] = gameState->nbComponentData[ComponentType_PhysicsData]++;
	gameState->physics[indices[ComponentType_PhysicsData]] = physics;

	indices[ComponentType_CollisionShape] = gameState->nbComponentData[ComponentType_CollisionShape]++;
	gameState->shapes[indices[ComponentType_CollisionShape]] = shape;

	indices[ComponentType_GraphicsData] = gameState->nbComponentData[ComponentType_GraphicsData]++;
	gameState->bitmaps[indices[ComponentType_GraphicsData]] = bitmap;
}

void InitGame(GameState* gameState)
{
	real32 halfWidth = gameState->renderWidth * 0.5f;
	real32 halfHeight = gameState->renderHeight * 0.5f;

	gameState->entities[0] = Entity(gameState, PhysicsData(Vec2(-halfWidth - 1, halfHeight)), RectangularShape(Vec2(2, gameState->renderHeight + 2)), Bitmap());
	gameState->entities[1] = Entity(gameState, PhysicsData(Vec2(halfWidth + 1, halfHeight)), RectangularShape(Vec2(2, gameState->renderHeight + 2)), Bitmap());
	gameState->entities[2] = Entity(gameState, PhysicsData(Vec2(0, -1)), RectangularShape(Vec2(gameState->renderWidth, 2)), Bitmap());
	gameState->entities[3] = Entity(gameState, PhysicsData(Vec2(0, gameState->renderHeight + 1)), RectangularShape(Vec2(gameState->renderWidth, 2)), Bitmap());

	gameState->entities[4] = Entity(gameState, PhysicsData(Vec2(-halfWidth * 0.5f, halfHeight * 0.5f)), RectangularShape(Vec2(halfWidth * 0.25f, 32)), Bitmap());
	gameState->entities[5] = Entity(gameState, PhysicsData(Vec2(halfWidth * 0.5f, halfHeight * 0.5f)), RectangularShape(Vec2(halfWidth * 0.25f, 32)), Bitmap());
	gameState->entities[6] = Entity(gameState, PhysicsData(Vec2(0, halfHeight)), RectangularShape(Vec2(halfWidth * 0.25f, 32)), Bitmap());

    LoadImage("assets/smiley.png", &gameState->bitmaps[0]);
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
}

void RenderGame(GameState* gameState)
{
    glViewport(0, 0, gameState->renderWidth, gameState->renderHeight);

    glClearColor(0.3f, 0.8f, 0.7f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (uint32 elementIdx = 0; elementIdx < 1 /*gameState->nbEntities*/; ++elementIdx)
    {
		Entity* entity = &gameState->entities[elementIdx];
		if (entity->flags & ComponentFlag_Graphical)
		{
			Bitmap* bitmap = &gameState->bitmaps[entity->indices[ComponentType_GraphicsData]];
			RenderBitmap(bitmap, 0, 0);
		}
    }
}

void LoadImage(const char* filename, Bitmap* bitmap)
{
    // NOTE(Charly): Get images bottom-up
    stbi_set_flip_vertically_on_load(true);
    int n;
    bitmap->data = stbi_load(filename, &bitmap->width, &bitmap->height, &n, 4);
    Assert(bitmap->data);
}

void ReleaseImage(Bitmap* bitmap)
{
    ReleaseBitmap(bitmap);
    stbi_image_free(bitmap->data);
}
