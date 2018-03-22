#ifndef RELWARB_GAME_H
#define RELWARB_GAME_H

#include "zmath.hpp"
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

            z::vec2 initialPos;
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

    Skill() {};
};

bool CreateDashSkill(Skill* skill, Entity* executive);
bool DashTrigger(GameState* gameState, Skill* skill, Entity* entity);
bool DashApply(GameState* gameState, Skill* skill, Entity* executive, real32 dt);
//bool DashCollide(GameState* gameState, Entity* executive, Entity* victim, void* parameters);

bool CreateManaRecharge(Skill* skill, Entity* executive);
bool ManaTrigger(GameState* gameState, Skill* skill, Entity* entity);
bool ManaApply(GameState* gameState, Skill* skill, Entity* executive, real32 dt);

void UpdateGameLogic(GameState* gameState, real32 dt);

#endif // RELWARB_GAME_H
