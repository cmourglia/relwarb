#ifndef RELWARB_WORLD_SIM_H
#define RELWARB_WORLD_SIM_H

#include "relwarb_defines.h"
#include "relwarb_math.h"

struct GameState;
struct Entity;

struct RigidBody
{
    Vec2 forces;
    // TODO(Charly): Angular stuff ?

    real32 invMass;
};

// TODO(Charly): Generalize shapes
struct Shape
{
	Vec2 size;
	Vec2 offset;

    inline Shape() {}
	inline Shape(Vec2 size_, Vec2 offset_ = Vec2(0))
		:size(size_), offset(offset_) {}
};

// NOTE(Charly): Create a rigid body
//               A null mass will lead to a static object
RigidBody* CreateRigidBody(GameState* gameState, real32 mass = 0.f);
Shape* CreateShape(GameState* gameState, Vec2 size_, Vec2 offset_ = Vec2(0));

void AddRigidBodyToEntity(Entity* entity, RigidBody* body);
void AddShapeToEntity(Entity* entity, Shape* shape);

void UpdateWorld(GameState* gameState, real32 dt);

// TODO(Charly): ApplyForce
// TODO(Charly): ApplyImpulse
// TODO(Charly): ApplyImpulseToPoint

bool32 CollisionCallback(Entity* e1, Entity* e2, void* userParam);

#endif // RELWARB_WORLD_SIM_H