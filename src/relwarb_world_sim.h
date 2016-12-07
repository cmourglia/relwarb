#ifndef RELWARB_WORLD_SIM_H
#define RELWARB_WORLD_SIM_H

#include "relwarb_defines.h"
#include "relwarb_entity.h"

struct GameState;

struct RigidBody : public Component
{
	Vec2 p;     // NOTE(Charly): Linear position
	Vec2 dp;    // NOTE(Charly): Linear velocity
	Vec2 ddp;   // NOTE(Charly): Linear acceleration

    Vec2 forces;
    // TODO(Charly): Angular stuff ?

    real32 invMass;

	inline RigidBody() {}
	inline RigidBody(Vec2 p_, Vec2 dp_ = Vec2(0), Vec2 ddp_ = Vec2(0))
		: p(p_), dp(dp_), ddp(ddp_) {}
};

// NOTE(Charly): Create a rigid body
//               A null mass will lead to a static object
RigidBody* CreateRigidBody(GameState* gameState, 
                           real32 mass = 0.f, 
                           Vec2 initialPos = Vec2(0.f),
                           Vec2 initialVel = Vec2(0.f));
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

#endif // RELWARB_WORLD_SIM_H