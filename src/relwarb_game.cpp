
#include "relwarb_game.h"
#include "relwarb_entity.h"
#include "relwarb.h"

#include "relwarb_debug.h"

void CreateDashSkill(Skill* skill, Entity* entity)
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
}

void CreateManaRecharge(Skill* skill, Entity* executive)
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
}

void DashTrigger(Skill* skill, Entity* entity)
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
		}
		SetEntityStatus(entity, EntityStatus_Rooted);
	}
}

void ManaTrigger(Skill* skill, Entity* entity)
{
	if (!skill->isActive && (entity->status & EntityStatus_Landed))
	{
		skill->isActive = true;
		skill->elapsed = 0.f;
		skill->remainingSteps = skill->nbSteps;
		SetEntityStatus(entity, EntityStatus_Rooted);
	}
}

void DashApply(Skill* skill, Entity* executive, real32 dt)
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
		executive->dp = Vec2(0);
	}
}

void ManaApply(Skill* skill, Entity* executive, real32 dt)
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
	}
}


void UpdateGameLogic(GameState* gameState, real32 dt)
{
	for (uint32 playerIdx = 0; playerIdx < gameState->nbPlayers; ++playerIdx)
	{
		Entity* player = gameState->players[playerIdx];
		Controller* controller = player->controller;

		// Refresh cooldowns
		for (uint32 i = 0; i < NB_SKILLS; ++i) {
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
					player->skills[0].triggerHandle(&player->skills[0], player);
				}
			}

			if (controller->mana && controller->newMana)
			{
				if (player->skills[1].remainingCooldown <= 0.f)
				{
					player->skills[1].triggerHandle(&player->skills[1], player);
				}
			}
		}

		// Resolve skills
		for (uint32 i = 0; i < NB_SKILLS; ++i) {
			if (player->skills[i].applyHandle != nullptr)
			{
				player->skills[i].applyHandle(&player->skills[i], player, dt);
			}
		}
	}
}