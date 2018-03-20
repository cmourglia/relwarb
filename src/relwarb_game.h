#ifndef RELWARB_GAME_H
#define RELWARB_GAME_H

#include "zmath.hpp"
#include "relwarb_defines.h"

struct Entity;
struct GameState;

struct Skill
{
    // Parameters
    // NOTE(Thomas): Trigger may not be null. Apply, collide and post effects can.
    bool (*triggerHandle)(Skill*, Entity*);
    bool (*postTriggerHandle)(Skill*, Entity*, GameState*);
    bool (*applyHandle)(Skill*, Entity*, real32);
    bool (*postApplyHandle)(Skill*, Entity*, GameState*);
    bool (*collideHandle)(Skill*, Entity*, Entity*, void*);
    bool (*postCollideHandle)(Skill*, Entity*, GameState*);

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
            real32  duration;
            uint32  manaCost;
            real32  horizDistance;

            real32  direction; // Toward left (-1) or toward right (+1)
        };
        // ManaRecharge Data
        struct
        {
            uint32  nbSteps;
            uint32  manaRefundPerStep;
            real32  stepDuration;

            uint32  remainingSteps;
        };
        // ...
    };
};

bool CreateDashSkill(Skill* skill, Entity* executive);
bool DashTrigger(Skill* skill, Entity* entity);
bool DashPostTrigger(Skill* skill, Entity* entity, GameState* gameState);
bool DashApply(Skill* skill, Entity* executive, real32 dt);
//bool DashCollide(Entity* executive, Entity* victim, void* parameters);

bool CreateManaRecharge(Skill* skill, Entity* executive);
bool ManaTrigger(Skill* skill, Entity* entity);
bool ManaPostTrigger(Skill* skill, Entity* entity, GameState* gameState);
bool ManaApply(Skill* skill, Entity* executive, real32 dt);

void UpdateGameLogic(GameState* gameState, real32 dt);

#endif  RELWARB_GAME_H
