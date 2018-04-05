#ifndef RELWARB_WORLD_SIM_H
#define RELWARB_WORLD_SIM_H

#include "relwarb_math.h"
#include "relwarb_defines.h"

#include <vector>

struct GameState;
struct Entity;
struct Bitmap;

class b2Body;

enum RigidBodyType
{
	RigidBodyType_Static,
	RigidBodyType_Kinematic,
	RigidBodyType_Dynamic,
};

struct PhysicsEntityData
{
	real32        mass     = 0.0f;
	z::vec2       extents  = z::Vec2(1, 1);
	z::vec2       position = z::Vec2(0, 0);
	RigidBodyType type     = RigidBodyType_Static;
};

struct RigidBody
{
	RigidBodyType type;
	b2Body*       body;
};

struct Shape
{
	z::vec2 size;
	z::vec2 offset;
};

ComponentID CreateShape(z::vec2 size, z::vec2 offset = z::Vec2(0));
Shape*      GetShape(Entity* entity);
void        AddShapeToEntity(Entity* entity, ComponentID shape);

RigidBody* GetRigidBody(Entity* entity);

void SetupDynamicEntity(Entity* entity, PhysicsEntityData data);
void UpdateWorld(real32 dt);

#endif // RELWARB_WORLD_SIM_H
