#ifndef RELWARB_WORLD_SIM_H
#define RELWARB_WORLD_SIM_H

#include "relwarb_math.h"
#include "relwarb_defines.h"

struct GameState;
struct Entity;
struct Bitmap;

enum RigidBodyType
{
	RigidBodyType_Static,
	RigidBodyType_Kinematic,
	RigidBodyType_Dynamic,
};

struct RigidBody
{
	// TODO(Charly): Angular stuff ?
	RigidBodyType type;
	z::vec2       forces;
	real32        invMass;
};

// TODO(Charly): Generalize shapes
struct Shape
{
	z::vec2 size;
	z::vec2 offset;
};

struct CollisionResult
{
	Entity* entity1;
	Entity* entity2;

	z::vec2 normal;
	real32  distance;

	bool32 collided;
};

// NOTE(Charly): Create a rigid body
//               A null mass will lead to a static object
RigidBody* CreateRigidBody(GameState* gameState, real32 mass = 0.f);
Shape*     CreateShape(GameState* gameState, z::vec2 size, z::vec2 offset = z::Vec2(0));

void AddRigidBodyToEntity(Entity* entity, RigidBody* body);
void AddShapeToEntity(Entity* entity, Shape* shape);

void UpdateWorld(GameState* gameState, real32 dt);

// Move a kinematic body. Collisions are solved for this entity.
// Returns the remainder of the motion.
z::vec2 MoveEntity(GameState*       gameState,
                   Entity*          entity,
                   z::vec2          motion,
                   CollisionResult* collisionData);

CollisionResult FillCollisionResult(Entity* e1, Entity* e2);

// TODO(Charly): ApplyForce
// TODO(Charly): ApplyImpulse
// TODO(Charly): ApplyImpulseToPoint

#endif // RELWARB_WORLD_SIM_H
