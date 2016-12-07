#ifndef RELWARB_ENTITY_H
#define RELWARB_ENTITY_H

typedef uint32 EntityID;
typedef uint32 ComponentID;

// NOTE(THOMAS): I'll go for adjective forms here, as it shows properties of entities
enum ComponentFlag
{
	ComponentFlag_Movable       = 1 << 0,
	ComponentFlag_Collidable    = 1 << 1,
	ComponentFlag_Renderable    = 1 << 2,
};

// NOTE(THOMAS): Adjective forms also as it shows type of data stored, or a name (as now) relative to data stored ?
enum ComponentType
{
	ComponentType_RigidBody	= 0,
	ComponentType_CollisionShape,		
	ComponentType_Bitmap,			

	ComponentType_NbTypes
};

struct Entity
{
	EntityID id;
	uint32 flags;

	ComponentID components[ComponentType_NbTypes];

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