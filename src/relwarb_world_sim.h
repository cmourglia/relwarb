#ifndef RELWARB_WORLD_SIM_H
#define RELWARB_WORLD_SIM_H

#include "relwarb_defines.h"
#include "relwarb_entity.h"

struct GameState;

struct RigidBody : public Component
{
    Vec2 forces;
    // TODO(Charly): Angular stuff ?

    real32 invMass;

	inline RigidBody() {}
};

// NOTE(Charly): Create a rigid body
//               A null mass will lead to a static object
RigidBody* CreateRigidBody(GameState* gameState, 
                           real32 mass = 0.f, 
                           Vec2 initialPos = Vec2(0.f),
                           Vec2 initialVel = Vec2(0.f));

void AddRigidBodyToEntity(Entity* entity, RigidBody* body, ComponentFlag flag = ComponentFlag_Movable);

void UpdateWorld(GameState* gameState, real32 dt);

inline void ApplyForce(RigidBody* body, Vec2 force)
{
    body->forces += force;
}

inline void ApplyImpulse(RigidBody* body, Vec2 impulse)
{
    body->dp += impulse;
}

// TODO(Charly): ApplyImpulseToPoint ?

bool32 CollisionCallback(Entity* e1, Entity* e2, void* userParam);

#endif // RELWARB_WORLD_SIM_H