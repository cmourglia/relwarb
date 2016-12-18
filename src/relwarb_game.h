#ifndef RELWARB_GAME_H
#define RELWARB_GAME_H

#include "relwarb_defines.h"
#include "relwarb_math.h"

struct Entity;

struct Skill
{
	// Parameters
	void (*triggerHandle)(Skill*, Entity*);
	void (*applyHandle)(Skill*, Entity*, real32);
	void (*collisionHandle)(Skill*, Entity*, Entity*, void*);
	
	// Active status
	bool32 isActive;
	real32 elapsed;
	uint32 manaCost;

	union
	{
		// Dash Data
		struct
		{
			real32	duration;
			real32	horizDistance;

			real32	direction; // Toward left (-1) or toward right (+1)
		};
		// ...
	};
};

void CreateDashSkill(Skill* skill, Entity* executive);

void DashTrigger(Skill* skill, Entity* entity);
void DashApply(Skill* skill, Entity* executive, real32 dt);
//void DashCollide(Entity* executive, Entity* victim, void* parameters);

#endif  RELWARB_GAME_H