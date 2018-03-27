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

void UpdateWorld(GameState* gameState, real32 dt)
{
	UpdateParticleSystems(gameState, dt);

	if (rigidBodySpawnTimer <= 0)
	{
		z::vec2 p      = z::Vec2(z::GenerateRandBetween(-5, 5), 10);
		Bitmap* bitmap = CreateBitmap(gameState);
		LoadBitmapData("assets/sprites/crate.png", bitmap);

		Sprite*           crateSprite = CreateStillSprite(gameState, bitmap);
		RenderingPattern* pattern     = CreateUniqueRenderingPattern(gameState, crateSprite);
		Shape*            shape       = CreateShape(gameState, z::Vec2(1, 1), z::Vec2(0, 0));
		RigidBody*        body        = CreateRigidBody(gameState, 1.0f);

		CreateBoxEntity(gameState, p, pattern, shape, body);
		rigidBodySpawnTimer = 3.0;
	}
	rigidBodySpawnTimer -= dt;

	const int32 velocityIterations = 6;
	const int32 positionIterations = 2;
	gameState->world.Step(dt, 6, 2);
}

void SetupDynamicEntity(GameState* state, Entity* entity, PhysicsEntityData data)
{
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

	entity->body = state->world.CreateBody(&def);

	b2PolygonShape shape;
	shape.SetAsBox(data.extents.x, data.extents.y);

	entity->body->CreateFixture(&shape, data.mass);
}
