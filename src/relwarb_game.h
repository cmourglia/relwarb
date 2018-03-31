#ifndef RELWARB_GAME_H
#define RELWARB_GAME_H

#include "relwarb_math.h"
#include "relwarb_defines.h"

struct Entity;
struct GameState;

struct Skill
{
    // Parameters
    // NOTE(Thomas): Trigger may not be null. Apply and collide can.
    bool(*triggerHandle)(GameState*, Skill*, Entity*);
    bool(*applyHandle)(GameState*, Skill*, Entity*, real32);
    bool(*collideHandle)(GameState*, Skill*, Entity*, Entity*, void*);

    // Active status
    bool32 isActive;

    union
    {
        // Dash Data
        struct
        {
			// Generic data
            real32	cooldownDuration;
			real32  duration;
			uint32  manaCost;
			real32  horizDistance;

			// Activation data
			real32	elapsed;
			real32	remainingCooldown;
            z::vec2 initialPos;
            real32  direction; // Toward left (-1) or toward right (+1)
        } dash;

        // ManaRecharge Data
        struct
        {
			// Generic data
			real32	cooldownDuration;
			uint32  nbSteps;
			uint32  manaRefundPerStep;
			real32  stepDuration;

			// Activation data
			real32	elapsed;
            real32	remainingCooldown;
            uint32  remainingSteps;
        } mana;
        // Passive regeneration
        struct
        {
            uint32  healthRefundPerStep;
            real32  healthStepDuration;

            uint32  manaRefundPerStep;
            real32  manaStepDuration;

            real32  healthStepElasped;
			real32  manaStepElasped;
        } regen;
        
		struct {
			// Generic data
			real32	cooldownDuration;
			real32  sliceDuration;
			real32	hitLowerBound;
			real32	hitHigherBound;
			uint32  manaCost;
			uint32	maxCharges;

			// Activation data
			real32	elapsed;
			real32	remainingCooldown;
			uint32	remainingCharges;
			real32  direction; // Toward left (-1) or toward right (+1)
			// TODO(Thomas): Handle hit targets in a more generic way to have more than 4 targets without exploding union size
			void*	alreadyHit[4];
		} slice;
		// ...
    };
};

bool CreateDashSkill(Skill* skill, Entity* executive);
bool DashTrigger(GameState* gameState, Skill* skill, Entity* entity);
bool DashApply(GameState* gameState, Skill* skill, Entity* executive, real32 dt);
//bool DashCollide(GameState* gameState, Entity* executive, Entity* victim, void* parameters);

bool CreateManaRecharge(Skill* skill, Entity* executive);
bool ManaTrigger(GameState* gameState, Skill* skill, Entity* entity);
bool ManaApply(GameState* gameState, Skill* skill, Entity* executive, real32 dt);

bool CreatePassiveRegeneration(Skill* skill, Entity* executive);
bool PassiveRegenerationTrigger(GameState* gameState, Skill* skill, Entity* entity);
bool PassiveRegenerationApply(GameState* gameState, Skill* skill, Entity* executive, real32 dt);

bool CreateSliceAndDice(Skill* skill, Entity* executive);
bool SliceAndDiceTrigger(GameState* gameState, Skill* skill, Entity* entity);
bool SliceAndDiceApply(GameState* gameState, Skill* skill, Entity* executive, real32 dt);

void UpdateGameLogic(GameState* gameState, real32 dt);

#endif // RELWARB_GAME_H
