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

internal void CopyPositionsToPhysics();
internal void CopyPositionsFromPhysics();

void UpdateWorld(real32 dt)
{
	UpdateParticleSystems(dt);

	if (rigidBodySpawnTimer <= 0)
	{
		z::vec2                  p           = z::Vec2(z::GenerateRandBetween(-10, 10), 10);
		static bool              firstTime   = true;
		static Bitmap*           bitmap      = CreateBitmap();
		static ComponentID       crateSprite = -1;
		static ComponentID       pattern     = -1;
		static ComponentID       shape       = -1;
		static PhysicsEntityData data        = {};

		if (firstTime)
		{
			LoadBitmapData("assets/sprites/crate.png", bitmap);
			crateSprite = CreateStillSprite(bitmap);
			pattern     = CreateUniqueRenderingPattern(crateSprite);
			shape       = CreateShape(z::Vec2(1, 1), z::Vec2(0, 0));

			data.type     = RigidBodyType_Dynamic;
			data.extents  = z::Vec2(0.5);
			data.mass     = 1;
			data.position = p;
		}

		Entity* e = CreateBoxEntity(p, pattern, shape);
		SetupDynamicEntity(e, data);

		rigidBodySpawnTimer = 0.25;
	}
	rigidBodySpawnTimer -= dt;

	CopyPositionsToPhysics();

	const int32 velocityIterations = 6;
	const int32 positionIterations = 2;
	state->world->Step(dt, 6, 2);

	CopyPositionsFromPhysics();
}

void CopyPositionsToPhysics()
{
	for (uint32 i = 0; i < state->nbEntities; ++i)
	{
		Entity*    entity = state->entities + i;
		RigidBody* body   = GetRigidBody(entity);

		if (body && body->type == RigidBodyType_Kinematic)
		{
			body->body->SetTransform(b2Vec2(entity->p.x, entity->p.y), 0);
		}
	}
}

void CopyPositionsFromPhysics()
{
	for (uint32 i = 0; i < state->nbEntities; ++i)
	{
		Entity*    entity = state->entities + i;
		RigidBody* body   = GetRigidBody(entity);

		if (body && body->type == RigidBodyType_Dynamic)
		{
			auto p              = body->body->GetPosition();
			auto a              = body->body->GetAngle();
			entity->p           = z::Vec2(p.x, p.y);
			entity->orientation = a;
		}
	}
}

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