
#include "relwarb_game.h"
#include "relwarb_entity.h"
#include "relwarb.h"

#include "relwarb_debug.h"

bool CreateDashSkill(Skill* skill, Entity* entity)
{
    skill->isActive = false;
    skill->triggerHandle = &DashTrigger;
    skill->postTriggerHandle = &DashPostTrigger;
    skill->applyHandle = &DashApply;
    skill->postApplyHandle = nullptr;
    skill->collideHandle = nullptr;
    skill->postCollideHandle = nullptr;

    skill->manaCost = 1;
    // NOTE(Thomas): Magic numbers to tailor
    skill->duration = 0.2f;
    skill->horizDistance = entity->shape->size.x() * 5.f;
    skill->cooldownDuration = 0.1f;

    return true;
}

bool CreateManaRecharge(Skill* skill, Entity* executive)
{
    skill->isActive = false;
    skill->triggerHandle = &ManaTrigger;
    skill->postTriggerHandle = &ManaPostTrigger;
    skill->applyHandle = &ManaApply;
    skill->postApplyHandle = nullptr;
    skill->collideHandle = nullptr;
    skill->postCollideHandle = nullptr;

    // NOTE(Thomas): Magic numbers to tailor
    skill->nbSteps = 5;
    skill->manaRefundPerStep = 1.f;
    skill->stepDuration = 0.8f;
    skill->cooldownDuration = 2.f;

    return true;
}

bool DashTrigger(Skill* skill, Entity* entity)
{
    if (!skill->isActive && entity->mana >= skill->manaCost && !(entity->status & EntityStatus_Rooted))
    {
        if (entity->controller->moveLeft || entity->controller->moveRight)
        {
            entity->mana -= skill->manaCost;
            skill->isActive = true;
            skill->elapsed = 0.f;

            if (entity->controller->moveLeft)
            {
                skill->direction = -1.f;
            }
            else // entity->controller->moveRight
            {
                skill->direction = 1.f;
            }
            SetEntityStatus(entity, EntityStatus_Rooted);
            return true;
        }
    }
    return false;
}

bool ManaTrigger(Skill* skill, Entity* entity)
{
    if (!skill->isActive && (entity->status & EntityStatus_Landed))
    {
        skill->isActive = true;
        skill->elapsed = 0.f;
        skill->remainingSteps = skill->nbSteps;
        SetEntityStatus(entity, EntityStatus_Rooted);
        return true;
    }
    return false;
}

bool DashPostTrigger(Skill* skill, Entity* entity, GameState* gameState)
{
    CreateTextualGameUI(gameState, "Dash !", z::vec4(1.0, 0.0, 0.0, 1.0), entity->p + z::vec2(0.5, 0.5), 2.0);
    return true;
}

bool ManaPostTrigger(Skill* skill, Entity* entity, GameState* gameState)
{
    CreateTextualGameUI(gameState, "Mana !", z::vec4(0.0, 0.0, 1.0, 1.0), entity->p + z::vec2(0.5, 0.5), 5.0);
    return true;
}

bool DashApply(Skill* skill, Entity* executive, real32 dt)
{
    if (skill->isActive)
    {
        skill->elapsed += dt;
        if (skill->elapsed >= skill->duration)
        {
            skill->remainingCooldown = skill->cooldownDuration;
            skill->isActive = false;
            UnsetEntityStatus(executive, EntityStatus_Rooted);

            dt -= skill->elapsed - skill->duration;
        }

        real32 ratio = dt / skill->duration;
        executive->p.x() += skill->direction * ratio * skill->horizDistance;
        executive->dp = z::vec2(0);
        return true;
    }
    return false;
}

bool ManaApply(Skill* skill, Entity* executive, real32 dt)
{
    if (skill->isActive)
    {
        skill->elapsed += dt;
        if (skill->elapsed >= skill->stepDuration)
        {
            executive->mana += skill->manaRefundPerStep;
            if (executive->mana > executive->max_mana)
            {
                executive->mana = executive->max_mana;
            }
            skill->elapsed -= skill->stepDuration;
            skill->remainingSteps -= 1;
            if (skill->remainingSteps == 0)
            {
                UnsetEntityStatus(executive, EntityStatus_Rooted);
                skill->remainingCooldown = skill->cooldownDuration;
                skill->isActive = false;
            }
        }
        return true;
    }
    return false;
}

void UpdateGameLogic(GameState* gameState, real32 dt)
{
    for (uint32 playerIdx = 0; playerIdx < gameState->nbPlayers; ++playerIdx)
    {
        Entity* player = gameState->players[playerIdx];
        Controller* controller = player->controller;

        // Refresh cooldowns
        for (uint32 i = 0; i < NB_SKILLS; ++i) {
            // TODO(Thomas): Encapsulate in "UpdateSkill" or something
            player->skills[i].remainingCooldown -= dt;
            if (player->skills[i].remainingCooldown <= 0.f)
            {
                player->skills[i].remainingCooldown = 0.f;
            }
        }

        if (!(player->status & EntityStatus_Muted) && !(player->status & EntityStatus_Stunned))
        {
            // Check for triggers
            if (controller->dash && controller->newDash)
            {
                if (player->skills[0].remainingCooldown <= 0.f)
                {
                    if (player->skills[0].triggerHandle(&player->skills[0], player) && player->skills[0].postTriggerHandle != nullptr)
                    {
                        player->skills[0].postTriggerHandle(&player->skills[1], player, gameState);
                    }
                }
            }

            // TODO(Charly): Allow player to stop charging on demand
            if (controller->mana && controller->newMana)
            {
                if (player->skills[1].remainingCooldown <= 0.f)
                {
                    if (player->skills[1].triggerHandle(&player->skills[1], player) && player->skills[1].postTriggerHandle != nullptr)
                    {
                        player->skills[1].postTriggerHandle(&player->skills[1], player, gameState);
                    }
                }
            }
        }

        // Resolve skills
        for (uint32 i = 0; i < NB_SKILLS; ++i) {
            if (player->skills[i].applyHandle != nullptr)
            {
                if (player->skills[i].applyHandle(&player->skills[i], player, dt) && player->skills[i].postApplyHandle != nullptr)
                {
                    player->skills[i].postApplyHandle(&player->skills[i], player, gameState);
                }
            }
        }
    }

    for (uint32 spriteIdx = 0; spriteIdx < gameState->nbSprites; ++spriteIdx)
    {
        UpdateSpriteTime(&gameState->sprites[spriteIdx], dt);
    }

    for (uint32 gameUIIdx = 0; gameUIIdx < gameState->nbGameUIs; ++gameUIIdx)
    {
        UpdateGameUITime(&gameState->gameUIs[gameUIIdx], dt);
    }
}
