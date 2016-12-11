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
    real32 ratio = gameState->viewportSize.x / gameState->viewportSize.y;

    gameState->worldSize = Vec2(20.f, 20.f / ratio);
    gameState->gravity = Vec2(0.f, -9.8f);
	real32 halfWidth = gameState->viewportSize.x * 0.5f;
	real32 halfHeight = gameState->viewportSize.y * 0.5f;

    Entity* entity0 = CreateEntity(gameState);
    
    Bitmap* bitmap = CreateBitmap(gameState);
    bitmap->entityID = entity0->id;
    LoadBitmapData("assets/smiley.png", bitmap);

    RigidBody* body = CreateRigidBody(gameState, 0.1f);
    body->entityID = entity0->id;
    body->invMass = .1f;

    AddComponentToEntity(entity0, bitmap->id, ComponentType_Bitmap, ComponentFlag_Renderable);
    AddComponentToEntity(entity0, body->id, ComponentType_RigidBody, ComponentFlag_Movable);

	// gameState->entities[0] = Entity(gameState, PhysicsData(Vec2(-halfWidth - 1, halfHeight)), RectangularShape(Vec2(2, gameState->renderHeight + 2)), Bitmap());
	// gameState->entities[1] = Entity(gameState, PhysicsData(Vec2(halfWidth + 1, halfHeight)), RectangularShape(Vec2(2, gameState->renderHeight + 2)), Bitmap());
	// gameState->entities[2] = Entity(gameState, PhysicsData(Vec2(0, -1)), RectangularShape(Vec2(gameState->renderWidth, 2)), Bitmap());
	// gameState->entities[3] = Entity(gameState, PhysicsData(Vec2(0, gameState->renderHeight + 1)), RectangularShape(Vec2(gameState->renderWidth, 2)), Bitmap());

	// gameState->entities[4] = Entity(gameState, PhysicsData(Vec2(-halfWidth * 0.5f, halfHeight * 0.5f)), RectangularShape(Vec2(halfWidth * 0.25f, 32)), Bitmap());
	// gameState->entities[5] = Entity(gameState, PhysicsData(Vec2(halfWidth * 0.5f, halfHeight * 0.5f)), RectangularShape(Vec2(halfWidth * 0.25f, 32)), Bitmap());
	// gameState->entities[6] = Entity(gameState, PhysicsData(Vec2(0, halfHeight)), RectangularShape(Vec2(halfWidth * 0.25f, 32)), Bitmap());
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

    for (uint32 elementIdx = 0; elementIdx < 1 /*gameState->nbEntities*/; ++elementIdx)
    {
		Entity* entity = &gameState->entities[elementIdx];
		if (EntityHasFlag(entity, ComponentFlag_Renderable))
		{
			Bitmap* bitmap = &gameState->bitmaps[entity->components[ComponentType_Bitmap]];
            Vec2 pos(0.f);

            if (EntityHasFlag(entity, ComponentFlag_Movable))
            {
                pos = gameState->rigidBodies[entity->components[ComponentType_RigidBody]].p;
            }

            Transform transform;
            transform.offset = Vec2(0, 0);
            transform.position = pos;
            transform.scale = Vec2(1);
			RenderBitmap(bitmap, &transform);
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

Entity* CreateEntity(GameState* gameState)
{
    EntityID id = gameState->nbEntities++;
    Assert(id < WORLD_SIZE);
    Entity* result = &gameState->entities[id];
    result->id = id;

    return result;
}

void AddComponentToEntity(Entity* entity, 
                          ComponentID componentID, 
                          ComponentType componentType, 
                          ComponentFlag componentFlag)
{
    entity->components[componentType] = componentID;
    SetEntityFlag(entity, componentFlag);
}

Bitmap* CreateBitmap(GameState* gameState)
{
    ComponentID id = gameState->nbBitmaps++;
    Assert(id < WORLD_SIZE);
    Bitmap* result = &gameState->bitmaps[id];
    result->id = id;

    return result;
}