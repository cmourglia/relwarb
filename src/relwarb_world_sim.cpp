#include "relwarb_world_sim.h"

#include <utility>
#include <vector>
#include <algorithm>

#include "relwarb_defines.h"
#include "relwarb_utils.h"
#include "relwarb_entity.h"
#include "relwarb_debug.h"
#include "relwarb.h"

void UpdateWorld(GameState* gameState, real32 dt)
{
	// Update particle systems
	for (uint32 systemIdx = 0; systemIdx < MAX_PARTICLE_SYSTEMS; ++systemIdx)
	{
		ParticleSystem* system = gameState->particleSystems + systemIdx;

		if (system->alive)
		{
			// Spawn new particles for the current system
			int newParticlesCount = system->particlesPerSecond * dt;
			// Log(Log_Debug, "system %i should spawn %i particles", systemIdx, newParticlesCount);
			for (int particleIdx = 0; particleIdx < newParticlesCount; ++particleIdx)
			{
				real angle = z::GenerateRandBetween(system->minAngle, system->maxAngle);
				real vel   = z::GenerateRandBetween(system->minVelocity, system->maxVelocity);

				Particle particle;
				particle.p         = system->pos;
				particle.dp        = z::Vec2(vel * z::Cos(angle), vel * z::Sin(angle));
				particle.color     = system->startColor;
				particle.life      = z::GenerateRandNormal(system->particleLife,
                                                      system->particleLifeDelta);
				particle.totalLife = particle.life;

				system->particles.push_back(particle);
			}
			// Update system lifetime
			system->systemLife -= dt;
			if (system->systemLife <= 0.f)
			{
				system->alive = false;
			}
		}

void UpdateWorld(GameState* gameState, real32 dt)
{
	UpdateParticleSystems(gameState, dt);




		}
	}

	// 2. Collision detection
	// Depending on the types of shapes we want collision for (I think I won't
	// be wrong if I say that we want other stuff than AABBs), we might need
	// a pruning phase (only test collisions for stuff that can collide).
	// Then, for each potentially colliding pair of entities, perform the test
	// (Depending on the shapes, GJK might be the best tool)

	// TODO(Thomas): Do something smart.
	std::vector<std::pair<Entity*, Entity*>> collisions;

	for (uint32 firstIdx = 0; firstIdx < (gameState->nbEntities - 1); ++firstIdx)
	{
		Entity* firstEntity = &gameState->entities[firstIdx];
		if (EntityHasComponent(firstEntity, ComponentFlag_Collidable))
		{
			for (uint32 secondIdx = firstIdx + 1; secondIdx < gameState->nbEntities; ++secondIdx)
			{
				Entity* secondEntity = &gameState->entities[secondIdx];
				if (EntityHasComponent(secondEntity, ComponentFlag_Collidable))
				{
					if (Intersect(firstEntity, secondEntity))
					{
						collisions.push_back(
						    std::pair<Entity*, Entity*>(firstEntity, secondEntity));
					}
				}
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

	for (auto it : collisions)
	{
		z::vec2 overlap = Overlap(it.first, it.second);
		if (CollisionCallback(it.first, it.second, &overlap))
		{
			Assert(EntityHasComponent(it.first, ComponentFlag_Movable) ||
			       EntityHasComponent(it.second, ComponentFlag_Movable));

			if (EntityHasComponent(it.first, ComponentFlag_Movable) &&
			    EntityHasComponent(it.second, ComponentFlag_Movable))
			{
				// TODO(Thomas): Handle collision w.r.t respective weights
			}
			else
			{
				z::vec2 clampDp;
				if (z::Abs(overlap.x) < z::Abs(overlap.y))
				{
					overlap = overlap * z::Vec2(1.f, 0.f);
					clampDp = z::Vec2(0.f, 1.f);
				}
				else
				{
					overlap = overlap * z::Vec2(0.f, 1.f);
					clampDp = z::Vec2(1.f, 0.f);
				}
				if (EntityHasComponent(it.first, ComponentFlag_Movable))
				{
					it.first->p -= overlap;
					it.first->dp = it.first->dp * clampDp;
				}
				else // (EntityHasFlag(it.second, ComponentFlag_Movable))
				{
					it.second->p += overlap;
					it.second->dp = it.second->dp * clampDp;
				}
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

z::vec2 MoveEntity(GameState* state, Entity* entity, z::vec2 motion)
{
	entity->p += motion;
	return motion;
}
