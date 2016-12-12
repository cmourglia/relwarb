#ifndef RELWARB_ENTITY_H
#define RELWARB_ENTITY_H

#include "relwarb_world_sim.h"

typedef uint32 EntityID;
typedef uint32 ComponentID;

struct RigidBody;
struct Shape;
struct RenderingPattern;

enum ComponentFlag
{
	// TODO(charly): Maybe find a better name for this one
	ComponentFlag_Movable       = 1 << 0,
	ComponentFlag_Collidable    = 1 << 1,
	ComponentFlag_Renderable    = 1 << 2,
};

struct Entity
{
	Vec2 p;     // NOTE(Charly): Linear position
	Vec2 dp;    // NOTE(Charly): Linear velocity
	Vec2 ddp;   // NOTE(Charly): Linear acceleration

	RigidBody* body;
	Shape* shape;
	RenderingPattern* pattern;

	EntityID id;
	uint32 flags;

	// TODO(Thomas): Handle flags a nicer way. That way :
	//					1) we have to do a constructor for each combination
	//					2) flags are statically defined
	//				 Use something like 'void addComponent(GameState gameState, ComponentType type, void * data, ComponentFlag flag)' ?

    // NOTE(Charly): There are two different things that needs to be done here,
    //                - Create a component and add it to the game state, those are defined by the systems.
    //                - Add a component to an entity, just takes id and flag as input
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

#endif // RELWARB_ENTITY_H