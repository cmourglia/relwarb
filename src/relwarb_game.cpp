
#include "relwarb_game.h"
#include "relwarb_entity.h"
#include "relwarb.h"

#include "relwarb_debug.h"

bool CreateDashSkill(Skill* skill, Entity* entity)
{
    skill->isActive = false;
    skill->triggerHandle = &DashTrigger;
    skill->applyHandle = &DashApply;
    skill->collideHandle = nullptr;

    skill->manaCost = 1;
    // NOTE(Thomas): Magic numbers to tailor
    skill->duration = 0.2f;
    skill->horizDistance = entity->shape->size.x * 5.f;
    skill->cooldownDuration = 0.1f;

    return true;
}

bool CreateManaRecharge(Skill* skill, Entity* executive)
{
    skill->isActive = false;
    skill->triggerHandle = &ManaTrigger;
    skill->applyHandle = &ManaApply;
    skill->collideHandle = nullptr;

    // NOTE(Thomas): Magic numbers to tailor
    skill->nbSteps = 5;
    skill->manaRefundPerStep = 1.f;
    skill->stepDuration = 0.8f;
    skill->cooldownDuration = 2.f;

    return true;
}

bool DashTrigger(GameState* gameState, Skill* skill, Entity* entity)
{
    if (!skill->isActive && entity->mana >= skill->manaCost && !(entity->status & EntityStatus_Rooted))
    {
        if (entity->controller->moveLeft || entity->controller->moveRight)
        {
            entity->mana -= skill->manaCost;
            skill->isActive = true;
            skill->elapsed = 0.f;
            skill->initialPos = entity->p + entity->shape->size * z::Vec2(0, 1);

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

bool ManaTrigger(GameState* gameState, Skill* skill, Entity* entity)
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

bool DashApply(GameState* gameState, Skill* skill, Entity* executive, real32 dt)
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
        executive->p.x += skill->direction * ratio * skill->horizDistance;
        executive->dp = z::Vec2(0.f);
        
        // Post effects
        real32 interpolate = skill->elapsed * 5.f;
        z::vec4 currentColor = z::Vec4(1.0f - interpolate, interpolate, 0.f, 1.f);
        Transform transform = {};
        transform.origin = z::Vec2(0.5, 0.f);
        auto worldToNormalize = GetProjectionMatrix(RenderMode_World, gameState) * GetTransformMatrix(RenderMode_World, &transform);
        auto normalizePos = worldToNormalize * skill->initialPos;
        RenderText("Dash !", normalizePos * z::Vec2(0.5) + z::Vec2(0.5), currentColor, gameState, ObjectType::ObjectType_UI);
        
        return true;
    }
    return false;
}

bool ManaApply(GameState* gameState, Skill* skill, Entity* executive, real32 dt)
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

        // Post effects
        z::vec4 indigo = z::Vec4(0.3, 0.f, 0.51, 1.0);
        z::vec4 turquoise = z::Vec4(0.f, 0.8, 0.81, 1.0);
        real32 elapsed2 = skill->elapsed * 2.f;
        real32 interpolate = (elapsed2 < 1.f) ? (elapsed2) : (2.f - elapsed2) ;
        z::vec4 currentColor = indigo * interpolate +turquoise * (1.f - interpolate);
        Transform transform = {};
        transform.origin = z::Vec2(0.5, 0);
        auto worldToNormalize = GetProjectionMatrix(RenderMode_World, gameState) * GetTransformMatrix(RenderMode_World, &transform);
        auto normalizePos = worldToNormalize * (executive->p + executive->shape->size * z::Vec2(0, 1));
        RenderText("Mana !", normalizePos * z::Vec2(0.5) + z::Vec2(0.5), currentColor, gameState, ObjectType::ObjectType_UI);

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
                    player->skills[0].triggerHandle(gameState, &player->skills[0], player);
                }
            }

            // TODO(Charly): Allow player to stop charging on demand
            if (controller->mana && controller->newMana)
            {
                if (player->skills[1].remainingCooldown <= 0.f)
                {
                    player->skills[1].triggerHandle(gameState, &player->skills[1], player);
                }
            }
        }

        // Resolve skills
        for (uint32 i = 0; i < NB_SKILLS; ++i) {
            if (player->skills[i].applyHandle != nullptr)
            {
                player->skills[i].applyHandle(gameState, &player->skills[i], player, dt);
            }
        }
    }

    for (uint32 spriteIdx = 0; spriteIdx < gameState->nbSprites; ++spriteIdx)
    {
        UpdateSpriteTime(&gameState->sprites[spriteIdx], dt);
    }
}
