#include "relwarb_world_sim.h"

#include <utility>
#include <vector>
#include <algorithm>

#include <Box2D/Box2D.h>

#include "relwarb_defines.h"
#include "relwarb_utils.h"
#include "relwarb_entity.h"
#include "relwarb_debug.h"
#include "relwarb_particles.h"
#include "relwarb.h"

global_variable real32 rigidBodySpawnTimer = 0.f;


void UpdateWorld(real32 dt)
{
	UpdateParticleSystems(dt);

	// if (rigidBodySpawnTimer <= 0)
	// {
	// 	z::vec2 p      = z::Vec2(z::GenerateRandBetween(-5, 5), 10);
	// 	Bitmap* bitmap = CreateBitmap(gameState);
	// 	LoadBitmapData("assets/sprites/crate.png", bitmap);

	// 	Sprite*           crateSprite = CreateStillSprite(gameState, bitmap);
	// 	RenderingPattern* pattern     = CreateUniqueRenderingPattern(gameState, crateSprite);
	// 	Shape*            shape       = CreateShape(gameState, z::Vec2(1, 1), z::Vec2(0, 0));
	// 	RigidBody*        body        = CreateRigidBody(gameState, 1.0f);

	// 	CreateBoxEntity(gameState, p, pattern, shape, body);
	// 	rigidBodySpawnTimer = 3.0;
	// }
	rigidBodySpawnTimer -= dt;

	const int32 velocityIterations = 6;
	const int32 positionIterations = 2;
	state->world->Step(dt, 6, 2);

}

{

void SetupDynamicEntity(Entity* entity, PhysicsEntityData data)
{
	uint32 bodyIndex = state->nbRigidBodies++;
	entity->body     = bodyIndex;

	b2BodyDef def;
	def.position.Set(data.position.x, data.position.y);

	switch (data.type)
	{
		case RigidBodyType_Static:
			def.type = b2_staticBody;
			break;
		case RigidBodyType_Kinematic:
			def.type = b2_kinematicBody;
			break;
		case RigidBodyType_Dynamic:
			def.type = b2_dynamicBody;
			break;
	}

	b2Body* body = state->world->CreateBody(&def);

	b2PolygonShape shape;
	shape.SetAsBox(data.extents.x, data.extents.y);

	body->CreateFixture(&shape, data.mass);

	RigidBody* rigidBody = state->bodies + bodyIndex;
	rigidBody->type      = data.type;
	rigidBody->body      = body;
}

ComponentID CreateShape(z::vec2 size, z::vec2 offset)
{
	ComponentID id = state->nbShapes++;
	Assert(id < WORLD_SIZE);
	Shape* result = &state->shapes[id];

	result->size   = size;
	result->offset = offset;

	return id;
}

void AddShapeToEntity(Entity* entity, ComponentID shape)
{
	entity->shape = shape;
	SetEntityComponent(entity, ComponentFlag_Collidable);
}

Shape* GetShape(Entity* entity)
{
	Shape* result = nullptr;
	if (entity->shape >= 0) result = state->shapes + entity->shape;

	return result;
}

RigidBody* GetRigidBody(Entity* entity)
{
	RigidBody* result = nullptr;
	if (entity->body >= 0) result = state->bodies + entity->body;

	return result;
}