#ifndef RELWARB_ENTITY_H
#define RELWARB_ENTITY_H

#include "relwarb_world_sim.h"

typedef uint32 EntityID;
typedef uint32 ComponentID;

struct RigidBody;
struct Shape;
struct RenderingPattern;
struct Controller;

enum ComponentFlag
{
	// TODO(charly): Maybe find a better name for this one
	ComponentFlag_Movable       = 1 << 0,
	ComponentFlag_Collidable    = 1 << 1,
	ComponentFlag_Renderable    = 1 << 2,
};

enum EntityType
{
	EntityType_Player = 0,
	EntityType_Enemy,
	EntityType_Wall,
	// ...
};

struct Entity
{
	EntityID id;
	EntityType entityType;
	uint32 flags;

	Vec2 p;     // NOTE(Charly): Linear position
	Vec2 dp;    // NOTE(Charly): Linear velocity
	Vec2 ddp;   // NOTE(Charly): Linear acceleration

	RigidBody* body;
	Shape* shape;
	RenderingPattern* pattern;

	// TODO(Thomas): Handle flags a nicer way. That way :
	//					1) we have to do a constructor for each combination
	//					2) flags are statically defined
	//				 Use something like 'void addComponent(GameState gameState, ComponentType type, void * data, ComponentFlag flag)' ?

    // NOTE(Charly): There are two different things that needs to be done here,
    //                - Create a component and add it to the game state, those are defined by the systems.
    //                - Add a component to an entity, just takes id and flag as input

	// TODO(Charly): Here are gameplay related stuff, tied to some particular
	//				 entity types. Not sure how we want to handle this yet.

	real32 playerSpeed;
	real32 playerJumpHeight;
	real32 playerJumpDist;

	real32 gravity;

	real32 initialJumpVelocity;
	bool32 alreadyJumping;
	bool32 newJump;
	real32 jumpTime;
	int nbJumps;

	bool32 quickFall;
	real32 quickFallTime;
	
	Controller* controller;
};

// NOTE(Charly): Helps compressing a bit of code
//               Not designed for polymorphism though
struct Component 
{
    EntityID entityID;
    ComponentID id;
};

inline void SetEntityFlag(Entity* entity, ComponentFlag flag)
{
    entity->flags |= flag;
}

inline void ToggleEntityFlag(Entity* entity, ComponentFlag flag)
{
    entity->flags ^= flag;
}

inline bool32 EntityHasFlag(Entity* entity, ComponentFlag flag)
{
    bool32 result = entity->flags & flag;
    return result;
}

// TODO(Charly): Add CreateXEntityFromData stuff
Entity* CreatePlayerEntity(GameState* state, Vec2 p, 
						RenderingPattern* pattern, 
						Shape* shape, 
						Controller* controller);

#endif // RELWARB_ENTITY_H