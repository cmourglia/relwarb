#include <stdio.h>
#include <utility>
#include <vector>

#include "relwarb_defines.h"
#include "relwarb_utils.h"
#include "relwarb_entity.h"
#include "relwarb.h"

void UpdateWorld(GameState* gameState, real32 dt)
{
    // Here is where the magic happens
    // Multiple stuff has to be done here, for each dynamic entities
    // (objects, actors or however you wanna call them).

    // 1. Integration step (Symplectic-Euler):
    // for each entity
    //      - Add gravity if needed :
    //          This needs more thinking / testing : how do we want to handle
    //          dashes, spells that may require a big air-time, etc ?
    //      - Compute acceleration (Newton FTW, a(t) = F(t) * 1 / m)
    //      - Compute new velocity : v(t) = v(t-1) + a(t) * dt
    //      - Compute new position : p(t) = p(t-1) + v(t) * dt
    //
    //      - NOTE(Charly): Forward Euler would have been
    //          p(t) = p(t-1) + v(t-1) * dt
    //          v(t) = v(t-1) + a(t) * dt
    for (uint32 entityIdx = 0; entityIdx < gameState->nbRigidBodies; ++entityIdx)
    {
        Entity* entity = &gameState->entities[entityIdx];

        if (EntityHasFlag(entity, ComponentFlag_Movable))
        {
            RigidBody* body = entity->body;
            body->forces += gameState->gravity;
            entity->ddp = body->forces * body->invMass;
            body->forces = Vec2(0.f); 
        }


        entity->dp += dt * entity->ddp;
        entity->p += dt * entity->dp; 
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
