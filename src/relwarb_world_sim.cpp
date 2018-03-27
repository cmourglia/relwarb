#include "relwarb_world_sim.h"

#include <utility>
#include <vector>
#include <algorithm>

#include "relwarb_defines.h"
#include "relwarb_utils.h"
#include "relwarb_entity.h"
#include "relwarb_debug.h"
#include "relwarb_particles.h"
#include "relwarb.h"

global_variable real32 rigidBodySpawnTimer = 0.f;

inline std::vector<Entity*> GetColliders(GameState* state, int32 entityFilter = -1);
inline CollisionResult      TestCollision(Entity* e1, Entity* e2);

void UpdateWorld(GameState* gameState, real32 dt)
{
	UpdateParticleSystems(gameState, dt);

	if (rigidBodySpawnTimer <= 0)
	{
		z::vec2 p      = z::Vec2(z::GenerateRandBetween(-15, 15), 10);
		Bitmap* bitmap = CreateBitmap(gameState);
		LoadBitmapData("assets/sprites/crate.png", bitmap);

		Sprite*           crateSprite = CreateStillSprite(gameState, bitmap);
		RenderingPattern* pattern     = CreateUniqueRenderingPattern(gameState, crateSprite);
		Shape*            shape       = CreateShape(gameState, z::Vec2(1, 1), z::Vec2(0, 0));
		RigidBody*        body        = CreateRigidBody(gameState, 1.0f);

		CreateBoxEntity(gameState, p, pattern, shape, body);
		rigidBodySpawnTimer = 2.0;
	}
	rigidBodySpawnTimer -= dt;

	// 1. Physics update
	for (uint32 id = 0; id < gameState->nbEntities; ++id)
	{
		Entity* entity = gameState->entities + id;
		if (EntityHasComponent(entity, ComponentFlag_Movable))
		{
			const z::vec2 gravity = z::Vec2(0, -50);
			entity->dp += gravity * dt;
			entity->p += entity->dp * dt;
		}
	}

	// 2. Collision detection
	// Depending on the types of shapes we want collision for (I think I won't
	// be wrong if I say that we want other stuff than AABBs), we might need
	// a pruning phase (only test collisions for stuff that can collide).
	// Then, for each potentially colliding pair of entities, perform the test
	// (Depending on the shapes, GJK might be the best tool)

	std::vector<CollisionResult> collisions;

	std::vector<Entity*> colliders = GetColliders(gameState);

	const uint32 nbColliders = (uint32)colliders.size();

	for (uint32 firstIdx = 0; firstIdx < (nbColliders - 1); ++firstIdx)
	{
		Entity* firstEntity = colliders[firstIdx];

		for (uint32 secondIdx = firstIdx + 1; secondIdx < nbColliders; ++secondIdx)
		{
			Entity*         secondEntity = colliders[secondIdx];
			CollisionResult collision    = TestCollision(firstEntity, secondEntity);
			if (collision.collided)
			{
				collisions.push_back(collision);
			}
		}
	}

	//
	// 3. Collision solving
	// for each collision
	//      - Call a callback function which goal is to enable gameplay programming.
	//        This could look something like
	//        bool32 CollisionCallback(Entity* e1, Entity* e2, void* userParam)
	//        The idea is that, depending on the objects colliding, you might want to
	//        be able to do some particular work. For example, if a character is
	//        colliding against a wall where there are many spikes, you want him to
	//        loose HP. You might want some destruction, or particles to spawn
	//        (you hit a fire hydrant), or you might want to trigger some event on
	//        some not visible colliding objects (you ran through a checkpoint,
	//        or on an invisible trap).
	//        Depending on the case, you might not want to enable the collision solving
	//        (an AABB used for a checkpoint), this is the whole point of returning
	//        a boolean as a result of that function. You want, or not, the collision
	//        to be solved.
	//
	//      - if (CollisionCallback(e1, e2)) {
	//          Now you want to solve the collision.
	//          The easiest way to do it, is by directly modifying the position of
	//          the objects overlapping.
	//          Objects move along the normal of the contact, and their position is
	//          modified so that they just do not overlap anymore.
	//          The mass of each object can be used to weight the amount of correction
	//          for each object (The heavier, the harder to move. Immovable objects
	//          have an infinite mass / null inverse mass)
	//      }

	for (auto collision : collisions)
	{
		Entity*      e1          = collision.entity1;
		Entity*      e2          = collision.entity2;
		const bool32 e1IsMovable = EntityHasComponent(e1, ComponentFlag_Movable);
		const bool32 e2IsMovable = EntityHasComponent(e2, ComponentFlag_Movable);

		if (!e1IsMovable && !e2IsMovable)
		{
			// Kinematic vs static
			return;
		}
		if (e1IsMovable && e2IsMovable)
		{
			// TODO(Thomas): Handle collision w.r.t respective weights
		}
		else
		{
			z::vec2 clampDp = collision.normal.x == 0 ? z::Vec2(1, 0) : z::Vec2(0, 1);

			if (e1IsMovable)
			{
				e1->p -= collision.normal * collision.distance * 2;
				e1->dp = e1->dp * clampDp;
			}
			else // (EntityHasFlag(it.second, ComponentFlag_Movable))
			{
				e2->p += collision.normal * collision.distance * 2;
				e2->dp = e2->dp * clampDp;
			}
		}
	}
}

bool32 CollisionCallback(Entity* e1, Entity* e2, void* userParam)
{
	// NOTE(Thomas): Only if Player against Wall or something, or always ?
	// NOTE(Thomas): else if or just if ?
	if (e1->entityType == EntityType_Player && e2->entityType == EntityType_Wall)
	{
		z::vec2* overlap = static_cast<z::vec2*>(userParam);
		if (z::Abs(overlap->y) < z::Abs(overlap->x) && overlap->y > 0)
		{
			Landed(e1);
		}
	}
	else if (e2->entityType == EntityType_Player && e1->entityType == EntityType_Wall)
	{
		z::vec2* overlap = static_cast<z::vec2*>(userParam);
		if (z::Abs(overlap->y) < z::Abs(overlap->x) && overlap->y > 0)
		{
			Landed(e2);
		}
	}

	return true;
}

RigidBody* CreateRigidBody(GameState* gameState, real32 mass)
{
	ComponentID id = gameState->nbRigidBodies++;
	Assert(id < WORLD_SIZE);
	RigidBody* result = &gameState->rigidBodies[id];

	result->invMass = (mass == 0.f ? 0.f : 1.f / mass);

	return result;
}

Shape* CreateShape(GameState* gameState, z::vec2 size_, z::vec2 offset_)
{
	ComponentID id = gameState->nbShapes++;
	Assert(id < WORLD_SIZE);
	Shape* result = &gameState->shapes[id];

	result->size   = size_;
	result->offset = offset_;

	return result;
}

void AddRigidBodyToEntity(Entity* entity, RigidBody* body)
{
	entity->body = body;
	SetEntityComponent(entity, ComponentFlag_Movable);
}

void AddShapeToEntity(Entity* entity, Shape* shape)
{
	entity->shape = shape;
	SetEntityComponent(entity, ComponentFlag_Collidable);
}

z::vec2 MoveEntity(GameState* state, Entity* entity, z::vec2 motion, CollisionResult* collisionData)
{
	entity->p += motion;
	return motion;
}

CollisionResult FillCollisionResult(Entity* e1, Entity* e2)
{
	CollisionResult result = {};

	z::vec2 overlap = Overlap(e1, e2);

	if (z::Abs(overlap.x) < z::Abs(overlap.y))
	{
		result.distance = z::Abs(overlap.x / 2.0);
		result.normal   = z::Vec2((overlap.x > 0 ? 1 : -1), 0);
	}
	else
	{
		result.distance = overlap.y / 2.0;
		result.normal   = z::Vec2(0, (overlap.y > 0 ? 1 : -1));
	}

	result.entity1  = e1;
	result.entity2  = e2;
	result.collided = true;

	return result;
}

inline std::vector<Entity*> GetColliders(GameState* state, int32 entityFilter)
{
	std::vector<Entity*> colliders;
	colliders.reserve(state->nbEntities);
	for (uint32 id = 0; id < state->nbEntities; ++id)
	{
		Entity* entity = state->entities + id;

		const int32  eid        = (int32)entity->id;
		const bool32 collidable = EntityHasComponent(entity, ComponentFlag_Collidable);

		if (eid != entityFilter && collidable)
		{
			colliders.push_back(entity);
		}
	}

	return colliders;
}

CollisionResult TestCollision(Entity* e1, Entity* e2)
{
	CollisionResult collision = {};
	if (Intersect(e1, e2))
	{
		collision = FillCollisionResult(e1, e2);
	}

	return collision;
}