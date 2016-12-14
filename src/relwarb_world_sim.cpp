#include <stdio.h>
#include <utility>
#include <vector>

#include "relwarb_defines.h"
#include "relwarb_utils.h"
#include "relwarb_entity.h"
#include "relwarb_debug.h"
#include "relwarb.h"

void UpdateWorld(GameState* gameState, real32 dt)
{
	// NOTE(Charly): I have removed generic integration stuff for now.
	//				 This function might actually be scripted, or call 
	//				 scripted entity update functions.
    for (uint32 entityIdx = 0; entityIdx < gameState->nbEntities; ++entityIdx)
    {
        Entity* entity = &gameState->entities[entityIdx];

		switch (entity->entityType)
		{
			case EntityType_Player:
			{
				// NOTE(Charly): We are updating a player, so we need to :
				//	- Change x velocity based on left / right inputs
				//	- If jump is pressed:
				//		- Is it the start of a new jump ? 
				//			- Y: start jumping, compute gravity and velocity
				//			based on current state and wished jump height,
				//			keep track of the number of total jumps (gd related)
				//			- N: update jumping elapsed time
				//	- Else:
				//		- Did we begin a jump and stopped early ? 
				//			- Change the gravity momentarily and track time

				Controller* controller = entity->controller;

				entity->dp.x = 0.0;
				if (controller->moveLeft)
				{
					entity->dp.x += -10.0;
				}
				
				if (controller->moveRight)
				{
					entity->dp.x += 10.0;
				}

				Vec2 acc = Vec2(0, entity->gravity);

#define MAX_JUMP_TIME   0.5f
#define MAX_STOP_TIME   0.1f
#define MAX_NB_JUMPS	2

				if (controller->jump)
				{
					if (!entity->alreadyJumping || entity->newJump && entity->nbJumps < MAX_NB_JUMPS)
					{
						// Start jumping
						entity->dp.y = entity->initialJumpVelocity;
						entity->alreadyJumping = true;
						entity->newJump = false;
						++entity->nbJumps;
					}
					else
					{
						entity->jumpTime += dt;
					}
				}
				else
				{
					entity->newJump = true;

					if (entity->alreadyJumping)
					{
						if (!entity->quickFall && entity->jumpTime < MAX_JUMP_TIME)
						{
							entity->quickFall = true;
							entity->quickFallTime = 0;
						}

						if (entity->quickFall && entity->quickFallTime < MAX_STOP_TIME)
						{
							entity->quickFallTime += dt;
							acc.y *= 5;
						}
					}
				}

				entity->p += dt * entity->dp + (0.5 * dt * dt * acc);
				entity->dp += dt * acc;

				// HACK(Charly): Stupid hack to keep the entities inside the world
				Shape* shape = entity->shape;

				// World center in (0,0)
				Vec2 minBound = entity->p - (shape->size / 2.f);
				Vec2 maxBound = entity->p + (shape->size / 2.f);

				Vec2 hs = gameState->worldSize / 2.f;

				bool32 resetJumps = false;

				if (minBound.x <= -hs.x)
				{
					entity->p.x = -hs.x + (shape->size.x / 2.f);
					resetJumps = true;
				}
				else if (maxBound.x >= hs.x)
				{
					entity->p.x = hs.x - (shape->size.x / 2.f);
					resetJumps = true;
				}

				if (minBound.y <= -hs.y)
				{
					entity->p.y = -hs.y + (shape->size.y / 2.f);
					entity->dp.y = 0;

					resetJumps = true;
				}
				else if (maxBound.y >= hs.y)
				{
					entity->p.y = hs.y - (shape->size.y / 2.f);
					entity->dp.y = 0;

					resetJumps = false;
				}

				if (resetJumps)
				{
					entity->alreadyJumping = false;
					entity->quickFall = false;
					entity->jumpTime = 0.f;
					entity->quickFallTime = 0.f;
					entity->nbJumps = 0;
				}
			} break;

			default:
			{
			}
		}
    }
	
    // 2. Collision detection
    // Depending on the types of shapes we want collision for (I think I won't
    // be wrong if I say that we want other stuff than AABBs), we might need
    // a pruning phase (only test collisions for stuff that can collide).
    // Then, for each potentially colliding pair of entities, perform the test
    // (Depending on the shapes, GJK might be the best tool)

	std::vector<std::pair <Entity *, Entity *>> collisions;

    // QUESTION(THOMAS): Double loop ? with test to handle each pair only once ?
    // ANSWER(Charly):   Yup, but you do not want to iterate over the entities, 
    //                   you want to iterate over the shapes and retrieve their 
    //                   position from their entityID.
	for (uint32 firstIdx = 0; firstIdx < gameState->nbRigidBodies; ++firstIdx)
	{
		/* Changement vers Rigid bodies

		RectangularShape* firstShape = &gameState->shapes[firstIdx];
		Entity* firstEntity = &gameState->entities[firstShape->entityID];
        if (EntityHasFlag(firstEntity, ComponentFlag_Collidable))
		{
			for (uint32 secondIdx = firstIdx + 1; secondIdx < gameState->nbShapes; ++secondIdx)
			{
				RectangularShape* secondShape = &gameState->shapes[secondIdx];
				Entity* secondEntity = &gameState->entities[secondShape->entityID];
				if (EntityHasFlag(secondEntity, ComponentFlag_Collidable))
				{
					if (Intersect(	gameState->rigidBodies[firstEntity->components[ComponentType_RigidBody]].p, firstShape,
									gameState->rigidBodies[secondEntity->components[ComponentType_RigidBody]].p, secondShape))
					{
						collisions.push_back(std::pair<Entity *, Entity *>(&gameState->entities[firstShape->entityID] , &gameState->entities[secondShape->entityID]));
					}
				}
			}
		}*/
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

	// for (auto it : collisions)
	// {
	// 	if (CollisionCallback(it.first, it.second, nullptr))
	// 	{
	// 		Assert(EntityHasFlag(it.first, ComponentFlag_Movable) || EntityHasFlag(it.second, ComponentFlag_Movable));

	// 		if (EntityHasFlag(it.first, ComponentFlag_Movable) && EntityHasFlag(it.second, ComponentFlag_Movable))
	// 		{

	// 		}
	// 		else
	// 		{
	// 			Vec2 overlap = Overlap( gameState->rigidBodies[it.first->components[ComponentType_RigidBody]].p, &gameState->shapes[it.first->components[ComponentType_CollisionShape]],
	// 									gameState->rigidBodies[it.second->components[ComponentType_RigidBody]].p, &gameState->shapes[it.second->components[ComponentType_CollisionShape]]);
	// 			if (EntityHasFlag(it.first, ComponentFlag_Movable))
	// 			{
	// 				gameState->rigidBodies[it.first->components[ComponentType_RigidBody]].p -= overlap;
	// 			}
	// 			else // (EntityHasFlag(it.second, ComponentFlag_Movable))
	// 			{
	// 				gameState->rigidBodies[it.second->components[ComponentType_RigidBody]].p += overlap;
	// 			}
	// 		}
	// 	}
	// }
}

bool32 CollisionCallback(Entity* e1, Entity* e2, void* userParam)
{
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

Shape* CreateShape(GameState* gameState, Vec2 size_, Vec2 offset_)
{
	ComponentID id = gameState->nbShapes++;
	Assert(id < WORLD_SIZE);
	Shape* result = &gameState->shapes[id];

	result->size = size_;
	result->offset = offset_;

	return result;
}

void AddRigidBodyToEntity(Entity* entity, RigidBody* body)
{
	entity->body = body;
	SetEntityFlag(entity, ComponentFlag_Movable);
}

void AddShapeToEntity(Entity* entity, Shape* shape)
{
    entity->shape = shape;
    SetEntityFlag(entity, ComponentFlag_Collidable);
}
