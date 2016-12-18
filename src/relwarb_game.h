#ifndef RELWARB_GAME_H
#define RELWARB_GAME_H

#include "relwarb_defines.h"
#include "relwarb_math.h"

struct Entity;
struct GameState;

struct Skill
{
	// Parameters
	// NOTE(Thomas): Trigger may not be null. Apply and collide can.
	void (*triggerHandle)(Skill*, Entity*);
	void (*applyHandle)(Skill*, Entity*, real32);
	void (*collideHandle)(Skill*, Entity*, Entity*, void*);
	
	// Active status
	bool32 isActive;
	real32 elapsed;

	// Cooldown status
	real32 cooldownDuration;
	real32 remainingCooldown;

	union
	{
		// Dash Data
		struct
		{
			real32	duration;
			uint32	manaCost;
			real32	horizDistance;

			real32	direction; // Toward left (-1) or toward right (+1)
		};
		// ManaRecharge Data
		struct
		{
			uint32	nbSteps;
			uint32	manaRefundPerStep;
			real32	stepDuration;

			uint32	remainingSteps;
		};
		// ...
	};
};

void CreateDashSkill(Skill* skill, Entity* executive);
void DashTrigger(Skill* skill, Entity* entity);
void DashApply(Skill* skill, Entity* executive, real32 dt);
//void DashCollide(Entity* executive, Entity* victim, void* parameters);

void CreateManaRecharge(Skill* skill, Entity* executive);
void ManaTrigger(Skill* skill, Entity* entity);
void ManaApply(Skill* skill, Entity* executive, real32 dt);

void UpdateGameLogic(GameState* gameState, real32 dt);

#endif  RELWARB_GAME_H