#ifndef RELWARB_ENTITY_H
#define RELWARB_ENTITY_H

#include "relwarb_game.h"
#include "relwarb_world_sim.h"

#include "relwarb_debug.h"

typedef uint32 EntityID;
typedef uint32 ComponentID;

struct RigidBody;
struct Shape;
struct RenderingPattern;
struct Controller;
struct Bitmap;
struct Skill;

enum ComponentFlag
{
	// TODO(charly): Maybe find a better name for this one
	ComponentFlag_Movable    = 1 << 0,
	ComponentFlag_Collidable = 1 << 1,
	ComponentFlag_Renderable = 1 << 2,
	ComponentFlag_Orientable = 1 << 3,
};

enum EntityType
{
	EntityType_Player = 0,
	EntityType_Enemy,
	EntityType_Wall,
	// ...
};

enum EntityStatus
{
	EntityStatus_Landed  = 1 << 0, // In contact with ground <=> horizontal wall
	EntityStatus_Airbone = 1 << 1, // Not in contact with ground, incompatible with Landed
	EntityStatus_Rooted  = 1 << 2, // Unable to move
	EntityStatus_Muted   = 1 << 3, // Unable to cast spells (skills involving mana)
	EntityStatus_Stunned = 1 << 4, // Unable to move and use skills
};

struct Entity
{
	EntityID   id;
	EntityType entityType;
	uint32     flags;

	z::vec2 p;   // NOTE(Charly): Linear position
	z::vec2 dp;  // NOTE(Charly): Linear velocity
	z::vec2 ddp; // NOTE(Charly): Linear acceleration

	int32 orientation;

	// NOTE(Thomas): Gonna go with integer values here, don't think there are real advantages with
	// real numbers (*badam tss*)
	uint32 health;
	uint32 max_health;
	uint32 mana;
	uint32 max_mana;
	uint32 status;

	RigidBody*        body;
	Shape*            shape;
	RenderingPattern* pattern;

	// TODO(Thomas): Handle flags a nicer way. That way :
	//                  1) we have to do a constructor for each combination
	//                  2) flags are statically defined
	//               Use something like 'void addComponent(GameState gameState, ComponentType type,
	//               void * data, ComponentFlag flag)' ?

	// NOTE(Charly): There are two different things that needs to be done here,
	//                - Create a component and add it to the game state, those are defined by the
	//                systems.
	//                - Add a component to an entity, just takes id and flag as input

	// TODO(Charly): Here are gameplay related stuff, tied to some particular
	//               entity types. Not sure how we want to handle this yet.

	real32 playerSpeed;
	real32 playerJumpHeight;
	real32 playerJumpDist;

	real32 gravity;

	real32 initialJumpVelocity;
	bool32 alreadyJumping;
	bool32 newJump;
	real32 jumpTime;
	int    nbJumps;

	bool32 quickFall;
	real32 quickFallTime;

	int32 controllerId;

#define NB_SKILLS 4
	Skill skills[NB_SKILLS];

	// HUD data
	Bitmap* avatar;
};

// NOTE(Charly): Helps compressing a bit of code
//               Not designed for polymorphism though
struct Component
{
	EntityID    entityID;
	ComponentID id;
};

inline void SetEntityComponent(Entity* entity, ComponentFlag flag)
{
	entity->flags |= flag;
}

inline void ToggleEntityComponent(Entity* entity, ComponentFlag flag)
{
	entity->flags ^= flag;
}

inline bool32 EntityHasComponent(Entity* entity, ComponentFlag flag)
{
	bool32 result = entity->flags & flag;
	return result;
}

inline void SetEntityStatus(Entity* entity, EntityStatus flag)
{
	entity->status |= flag;
}

inline void UnsetEntityStatus(Entity* entity, EntityStatus flag)
{
	entity->status &= ~flag;
}

inline void ToggleEntityStatus(Entity* entity, EntityStatus flag)
{
	entity->status ^= flag;
}

inline void ResetJump(Entity* player)
{
	player->alreadyJumping = false;
	player->quickFall      = false;
	player->jumpTime       = 0.f;
	player->quickFallTime  = 0.f;
	player->nbJumps        = 0;
}

inline void WentAirborne(Entity* entity)
{
	UnsetEntityStatus(entity, EntityStatus_Landed);
	SetEntityStatus(entity, EntityStatus_Airbone);
}

inline void Landed(Entity* entity)
{
	UnsetEntityStatus(entity, EntityStatus_Airbone);
	SetEntityStatus(entity, EntityStatus_Landed);
	ResetJump(entity);
}

// TODO(Charly): Add CreateXEntityFromData stuff
Entity* CreatePlayerEntity(GameState*        state,
                           z::vec2           p,
                           RenderingPattern* pattern,
                           Shape*            shape,
                           int32             controllerId);

Entity* CreateWallEntity(GameState* state, z::vec2 p, RenderingPattern* pattern, Shape* shape);

bool32 Intersect(const Entity* entity1, const Entity* entity2);

// Returns the overlap w.r.t entity1 so that applying the opposite vector to entity1 would lead to
// shapes being just in contact.
z::vec2 Overlap(const Entity* entity1, const Entity* entity2);

#endif // RELWARB_ENTITY_H
