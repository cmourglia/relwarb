
#include "relwarb_game.h"
#include "relwarb_entity.h"
#include "relwarb_controller.h"
#include "relwarb.h"

#include "relwarb_debug.h"

bool CreateDashSkill(Skill* skill, Entity* entity)
{
	skill->isActive      = false;
	skill->triggerHandle = &DashTrigger;
	skill->applyHandle   = &DashApply;
	skill->collideHandle = nullptr;

	skill->dash.manaCost = 1;
	// NOTE(Thomas): Magic numbers to tailor
	skill->dash.duration         = 0.25f;
	skill->dash.horizDistance    = entity->shape->size.x * 5.f;
	skill->dash.cooldownDuration = 0.1f;

	return true;
}

bool CreateManaRecharge(Skill* skill, Entity* executive)
{
	skill->isActive      = false;
	skill->triggerHandle = &ManaTrigger;
	skill->applyHandle   = &ManaApply;
	skill->collideHandle = nullptr;

	// NOTE(Thomas): Magic numbers to tailor
	skill->mana.nbSteps           = 5;
	skill->mana.manaRefundPerStep = 1.f;
	skill->mana.stepDuration      = 0.8f;
	skill->mana.cooldownDuration  = 2.f;

	return true;
}

bool CreatePassiveRegeneration(Skill* skill, Entity* executive)
{
	skill->isActive      = false;
	skill->triggerHandle = &PassiveRegenerationTrigger;
	skill->applyHandle   = &PassiveRegenerationApply;
	skill->collideHandle = nullptr;

	skill->regen.manaRefundPerStep = 1;
	skill->regen.manaStepDuration  = 2.f;

	skill->regen.healthRefundPerStep = 1;
	skill->regen.healthStepDuration  = 1.f;

	return true;
}

bool DashTrigger(GameState* gameState, Skill* skill, Entity* entity)
{
	if (skill->dash.remainingCooldown <= 0.f && !skill->isActive &&
	    entity->mana >= skill->dash.manaCost && !(entity->status & EntityStatus_Rooted))
	{
		if (IsActionPressed(gameState, entity->controllerId, Action_Left) ||
		    IsActionPressed(gameState, entity->controllerId, Action_Right))
		{
			entity->mana -= skill->dash.manaCost;
			skill->isActive     = true;
			skill->dash.elapsed = 0.f;
			// z::vec2 initPos =
			skill->dash.initialPos = entity->p + entity->shape->size * z::vec2{0.0, 1.0};

			if (IsActionPressed(gameState, entity->controllerId, Action_Left))
			{
				skill->dash.direction = -1.f;
			}
			else // entity->controller->moveRight
			{
				skill->dash.direction = 1.f;
			}
			SetEntityStatus(entity, EntityStatus_Rooted);
			return true;
		}
	}
	return false;
}

bool ManaTrigger(GameState* gameState, Skill* skill, Entity* entity)
{
	if (skill->mana.remainingCooldown <= 0.f && !skill->isActive &&
	    (entity->status & EntityStatus_Landed))
	{
		skill->isActive            = true;
		skill->mana.elapsed        = 0.f;
		skill->mana.remainingSteps = skill->mana.nbSteps;
		SetEntityStatus(entity, EntityStatus_Rooted);
		return true;
	}
	return false;
}

bool PassiveRegenerationTrigger(GameState* gameState, Skill* skill, Entity* entity)
{
	if (!skill->isActive)
	{
		skill->isActive                = true;
		skill->regen.healthStepElasped = 0.f;
		skill->regen.manaStepElasped   = 0.f;

		return true;
	}
	else
	{
		return false;
	}
}

bool DashApply(GameState* gameState, Skill* skill, Entity* executive, real32 dt)
{
	skill->dash.remainingCooldown -= dt;
	if (skill->dash.remainingCooldown <= 0.f)
	{
		skill->dash.remainingCooldown = 0.f;
	}

	if (skill->isActive)
	{
		skill->dash.elapsed += dt;
		if (skill->dash.elapsed >= skill->dash.duration)
		{
			skill->dash.remainingCooldown = skill->dash.cooldownDuration;
			skill->isActive               = false;
			UnsetEntityStatus(executive, EntityStatus_Rooted);

			dt -= skill->dash.elapsed - skill->dash.duration;
		}

		real32 ratio = dt / skill->dash.duration;
		executive->p.x += skill->dash.direction * ratio * skill->dash.horizDistance;
		executive->dp = z::vec2{0.0, 0.0};

		// Post effects
		real32    interpolate = skill->dash.elapsed * 5.0;
		z::vec4   currentColor{1.f - interpolate, interpolate, 0.f, 1.f};
		Transform transform   = {};
		transform.origin      = z::vec2{0.5, 0.0};
		auto worldToNormalize = GetProjectionMatrix(RenderMode_World, gameState) *
		                        GetTransformMatrix(RenderMode_World, &transform);
		auto normalizePos = worldToNormalize * skill->dash.initialPos;
		RenderText("Dash !",
		           normalizePos * z::vec2{0.5, 0.5} + z::vec2{0.5, 0.5},
		           currentColor,
		           gameState,
		           ObjectType::ObjectType_UI);

		return true;
	}
	return false;
}

bool ManaApply(GameState* gameState, Skill* skill, Entity* executive, real32 dt)
{
	skill->mana.remainingCooldown -= dt;
	if (skill->mana.remainingCooldown <= 0.f)
	{
		skill->mana.remainingCooldown = 0.f;
	}

	if (skill->isActive)
	{
		skill->mana.elapsed += dt;
		if (skill->mana.elapsed >= skill->mana.stepDuration)
		{
			executive->mana += skill->mana.manaRefundPerStep;
			if (executive->mana > executive->max_mana)
			{
				executive->mana = executive->max_mana;
			}
			skill->mana.elapsed -= skill->mana.stepDuration;
			skill->mana.remainingSteps -= 1;
			if (skill->mana.remainingSteps == 0)
			{
				// TODO(Thomas): Should be handle with a stack of some kind so that we do not
				// "unroot" the player if an external effect is supposed to root him
				UnsetEntityStatus(executive, EntityStatus_Rooted);
				skill->mana.remainingCooldown = skill->mana.cooldownDuration;
				skill->isActive               = false;
			}
		}

		// Post effects
		z::vec4   indigo{0.3f, 0.0f, 0.51f, 1.0f};
		z::vec4   turquoise{0.0f, 0.8f, 0.81f, 1.0f};
		real32    elapsed2     = skill->mana.elapsed * 2.f;
		real32    interpolate  = (elapsed2 < 1.f) ? (elapsed2) : (2.f - elapsed2);
		z::vec4   currentColor = indigo * interpolate + turquoise * (1.f - interpolate);
		Transform transform    = {};
		transform.origin       = z::vec2{0.5, 0.0};
		auto worldToNormalize  = GetProjectionMatrix(RenderMode_World, gameState) *
		                        GetTransformMatrix(RenderMode_World, &transform);
		auto normalizePos = worldToNormalize *
		                    (executive->p + executive->shape->size * z::vec2{0.0, 1.0});
		RenderText("Mana !",
		           normalizePos * z::vec2{0.5, 0.5} + z::vec2{0.5, 0.5},
		           currentColor,
		           gameState,
		           ObjectType::ObjectType_UI);

		return true;
	}
	return false;
}

bool PassiveRegenerationApply(GameState* gameState, Skill* skill, Entity* executive, real32 dt)
{
	// Skill is always active
	if (!(executive->status & EntityStatus::EntityStatus_Stunned))
	{
		skill->regen.healthStepElasped += dt;
		if (skill->regen.healthStepElasped >= skill->regen.healthStepDuration)
		{
			executive->health += skill->regen.healthRefundPerStep;
			if (executive->health > executive->max_health)
			{
				executive->health = executive->max_health;
			}
			skill->regen.healthStepElasped -= skill->regen.healthStepDuration;
		}

		skill->regen.manaStepElasped += dt;
		if (skill->regen.manaStepElasped >= skill->regen.manaStepDuration)
		{
			executive->mana += skill->regen.manaRefundPerStep;
			if (executive->mana > executive->max_mana)
			{
				executive->mana = executive->max_mana;
			}
			skill->regen.manaStepElasped -= skill->regen.manaStepDuration;
		}
		return true;
	}
	return false;
}

void UpdateGameLogic(GameState* gameState, real32 dt)
{
	for (uint32 playerIdx = 0; playerIdx < gameState->nbPlayers; ++playerIdx)
	{
		Entity*     player     = gameState->players[playerIdx];
		Controller* controller = &(gameState->controllers[player->controllerId]);

		if (!(player->status & EntityStatus_Muted) && !(player->status & EntityStatus_Stunned))
		{
			// Check for triggers
			if (IsActionRisingEdge(gameState, playerIdx, Action_Skill1))
			{
				player->skills[0].triggerHandle(gameState, &player->skills[0], player);
			}

			// TODO(Charly): Allow player to stop charging on demand
			if (IsActionRisingEdge(gameState, playerIdx, Action_Skill2))
			{
				player->skills[1].triggerHandle(gameState, &player->skills[1], player);
			}

			player->skills[2].triggerHandle(gameState, &player->skills[2], player);
		}

		// Resolve skills
		for (uint32 i = 0; i < NB_SKILLS; ++i)
		{
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
