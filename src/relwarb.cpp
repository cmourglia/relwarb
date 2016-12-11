#include "relwarb.h"
#include "relwarb_opengl.h"
#include "relwarb_math.h"

// TODO(Charly): This should go somewhere else.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void InitGame(GameState* gameState)
{
    gameState->gravity = Vec2(0.f, -9.8f);
	real32 halfWidth = gameState->renderWidth * 0.5f;
	real32 halfHeight = gameState->renderHeight * 0.5f;

    Entity* entity0 = CreateEntity(gameState);
    
    Bitmap* bitmap = CreateBitmap(gameState);
    bitmap->entityID = entity0->id;
    LoadImage("assets/smiley.png", bitmap);

    RigidBody* body = CreateRigidBody(gameState, 0.1f);
    body->entityID = entity0->id;
    body->invMass = .1f;

    AddComponentToEntity(entity0, bitmap->id, ComponentType_Bitmap, ComponentFlag_Renderable);
    AddComponentToEntity(entity0, body->id, ComponentType_RigidBody, ComponentFlag_Movable);


	Bitmap* wallTexture = CreateBitmap(gameState);
	LoadImage("assets/wall.png", wallTexture);

	RectangularShape* horizontalBoundary = CreateShape(gameState, Vec2(gameState->renderWidth, 2.f));
	RectangularShape* verticalBoundary = CreateShape(gameState, Vec2(2.f, gameState->renderHeight));

	Entity* floor = CreateEntity(gameState);
	RigidBody* floor_body = CreateRigidBody(gameState, 0.f, Vec2(0.f, -1.f));
	AddComponentToEntity(floor, floor_body->id, ComponentType_RigidBody, ComponentFlag_Collidable);
	AddComponentToEntity(floor, horizontalBoundary->id, ComponentType_CollisionShape, ComponentFlag_Collidable);
	AddComponentToEntity(floor, wallTexture->id, ComponentType_Bitmap, ComponentFlag_Renderable);

	Entity* ceiling = CreateEntity(gameState);
	RigidBody* ceiling_body = CreateRigidBody(gameState, 0.f, Vec2(0.f, gameState->renderHeight + 1.f));
	AddComponentToEntity(ceiling, ceiling_body->id, ComponentType_RigidBody, ComponentFlag_Collidable);
	AddComponentToEntity(ceiling, horizontalBoundary->id, ComponentType_CollisionShape, ComponentFlag_Collidable);
	AddComponentToEntity(ceiling, wallTexture->id, ComponentType_Bitmap, ComponentFlag_Renderable);

	Entity* leftWall = CreateEntity(gameState);
	RigidBody* leftWall_body = CreateRigidBody(gameState, 0.f, Vec2(- halfWidth - 1.f, halfHeight));
	AddComponentToEntity(leftWall, leftWall_body->id, ComponentType_RigidBody, ComponentFlag_Collidable);
	AddComponentToEntity(leftWall, verticalBoundary->id, ComponentType_CollisionShape, ComponentFlag_Collidable);
	AddComponentToEntity(leftWall, wallTexture->id, ComponentType_Bitmap, ComponentFlag_Renderable);

	Entity* rightWall = CreateEntity(gameState);
	RigidBody* rightWall_body = CreateRigidBody(gameState, 0.f, Vec2(halfWidth + 1.f, halfHeight));
	AddComponentToEntity(rightWall, rightWall_body->id, ComponentType_RigidBody, ComponentFlag_Collidable);
	AddComponentToEntity(rightWall, verticalBoundary->id, ComponentType_CollisionShape, ComponentFlag_Collidable);
	AddComponentToEntity(rightWall, wallTexture->id, ComponentType_Bitmap, ComponentFlag_Renderable);


	RectangularShape* platformBoundary = CreateShape(gameState, Vec2(gameState->renderWidth * 0.125f, 32.f));

	Entity* leftPlatform = CreateEntity(gameState);
	RigidBody* leftPlatform_body = CreateRigidBody(gameState, 0.f, Vec2(-halfWidth * 0.5f, halfHeight * 0.5f));
	AddComponentToEntity(leftPlatform, leftPlatform_body->id, ComponentType_RigidBody, ComponentFlag_Collidable);
	AddComponentToEntity(leftPlatform, platformBoundary->id, ComponentType_CollisionShape, ComponentFlag_Collidable);
	AddComponentToEntity(leftPlatform, wallTexture->id, ComponentType_Bitmap, ComponentFlag_Renderable);

	Entity* rightPlatform = CreateEntity(gameState);
	RigidBody* rightPlatform_body = CreateRigidBody(gameState, 0.f, Vec2(halfWidth * 0.5f, halfHeight * 0.5f));
	AddComponentToEntity(rightPlatform, rightPlatform_body->id, ComponentType_RigidBody, ComponentFlag_Collidable);
	AddComponentToEntity(rightPlatform, platformBoundary->id, ComponentType_CollisionShape, ComponentFlag_Collidable);
	AddComponentToEntity(rightPlatform, wallTexture->id, ComponentType_Bitmap, ComponentFlag_Renderable);

	Entity* centerPlatform = CreateEntity(gameState);
	RigidBody* centerPlatform_body = CreateRigidBody(gameState, 0.f, Vec2(0.f, halfHeight));
	AddComponentToEntity(centerPlatform, centerPlatform_body->id, ComponentType_RigidBody, ComponentFlag_Collidable);
	AddComponentToEntity(centerPlatform, platformBoundary->id, ComponentType_CollisionShape, ComponentFlag_Collidable);
	AddComponentToEntity(centerPlatform, wallTexture->id, ComponentType_Bitmap, ComponentFlag_Renderable);
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

void RenderGame(GameState* gameState)
{
    glViewport(0, 0, gameState->renderWidth, gameState->renderHeight);

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

			RenderBitmap(bitmap, pos.x, pos.y);
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