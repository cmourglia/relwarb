#include "relwarb_entity.h"

#include "relwarb.h"
#include "relwarb_world_sim.h"
#include "relwarb_renderer.h"
#include "relwarb_debug.h"

#include <assert.h>

Entity* CreatePlayerEntity(GameState*        state,
                           z::vec2           p,
                           RenderingPattern* pattern,
                           Shape*            shape,
                           int32             controllerId)
{
	Entity* result                     = CreateEntity(state, EntityType_Player, p);
	state->players[state->nbPlayers++] = result->id;

	AddRenderingPatternToEntity(result, pattern);
	AddShapeToEntity(result, shape);
	SetEntityComponent(result, ComponentFlag_Movable);
	SetEntityComponent(result, ComponentFlag_Orientable);

	// FIXME(Charly): Load this from files
	result->avatar = CreateBitmap(state);
	switch (state->nbPlayers)
	{
		case 1:
		{
			LoadBitmapData("assets/sprites/p1_avatar.png", result->avatar);
		}
		break;
		case 2:
		{
			LoadBitmapData("assets/sprites/p2_avatar.png", result->avatar);
		}
		break;
		case 3:
		{
			// LoadBitmapData("assets/sprites/p3_avatar.png", result->avatar);
		}
		break;
		case 4:
		{
			// LoadBitmapData("assets/sprites/p4_avatar.png", result->avatar);
		}
		break;
		default:
			Log(Log_Error, "Invalid number of players");
	}
	result->max_health          = 10;
	result->health              = 1;
	result->max_mana            = 5;
	result->mana                = 5;
	result->playerSpeed         = 40.f;
	result->playerJumpHeight    = 5.f;
	result->playerJumpDist      = 16.f;
	result->initialJumpVelocity = (2 * result->playerJumpHeight * result->playerSpeed) /
	                              result->playerJumpDist;
	result->gravity = (-2 * result->playerJumpHeight * result->playerSpeed * result->playerSpeed) /
	                  (result->playerJumpDist * result->playerJumpDist);
	result->status      = 0;
	result->orientation = 1.f;

	CreateDashSkill(&result->skills[0], result);
	CreateManaRecharge(&result->skills[1], result);
	CreatePassiveRegeneration(&result->skills[2], result);

	result->controllerId = controllerId;
	result->updateFunc   = &UpdateEntityPlayer;

	return result;
}

Entity* CreateWallEntity(GameState* state, z::vec2 p, RenderingPattern* pattern, Shape* shape)
{
	Entity* result = CreateEntity(state, EntityType_Wall, p);

	AddRenderingPatternToEntity(result, pattern);
	AddShapeToEntity(result, shape);

	result->updateFunc = &UpdateEntityNoop;

	return result;
}

// cf. http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?page=3
bool32 Intersect(const Entity* entity1, const Entity* entity2)
{
	// NOTE(Thomas): Inliner ?
	const z::vec2& pos1   = entity1->p;
	const z::vec2& pos2   = entity2->p;
	const Shape*   shape1 = entity1->shape;
	const Shape*   shape2 = entity2->shape;

	real32 diffX = z::Abs(pos1.x + shape1->offset.x - pos2.x - shape2->offset.x);
	real32 diffY = z::Abs(pos1.y + shape1->offset.y - pos2.y - shape2->offset.y);
	return (diffX < (shape1->size.x + shape2->size.x) * 0.5f &&
	        diffY < (shape1->size.y + shape2->size.y) * 0.5f);
}

z::vec2 Overlap(const Entity* entity1, const Entity* entity2)
{
	// NOTE(Thomas): Inliner ?
	const z::vec2& pos1   = entity1->p;
	const z::vec2& pos2   = entity2->p;
	const Shape*   shape1 = entity1->shape;
	const Shape*   shape2 = entity2->shape;

	const real32 overX = (pos1.x + shape1->offset.x) - (pos2.x - shape2->offset.x);
	const real32 overY = (pos1.y + shape1->offset.y) - (pos2.y - shape2->offset.y);
	const real32 signX = overX >= 0.f ? 1.f : -1.f;
	const real32 signY = overY >= 0.f ? 1.f : -1.f;

	z::vec2 over = z::Vec2(overX, overY);
	z::vec2 size = shape1->size + shape2->size;
	z::vec2 sign = z::Vec2(signX, signY);
	return over - 0.5f * sign * size;
}

Entity* GetPlayerEntity(GameState* state, int32 player)
{
	assert(player >= 0 && player < MAX_PLAYERS);
	int32 entityId = state->players[player];
	assert(entityId >= 0 && entityId < WORLD_SIZE);
	Entity* result = state->entities + entityId;

	return result;
}

void UpdateEntityNoop(GameState* state, Entity* entity, real32 dt)
{
	(void)state;
	(void)entity;
	(void)dt;
}

void UpdateEntityPlayer(GameState* state, Entity* entity, real32 dt)
{
	const int32 controllerId = entity->controllerId;

	if (!(entity->status & EntityStatus_Muted) && !(entity->status & EntityStatus_Stunned))
	{
		// Check for triggers
		if (IsActionRisingEdge(state, controllerId, Action_Skill1))
		{
			entity->skills[0].triggerHandle(state, &entity->skills[0], entity);
		}

		// TODO(Charly): Allow entity to stop charging on demand
		if (IsActionRisingEdge(state, controllerId, Action_Skill2))
		{
			entity->skills[1].triggerHandle(state, &entity->skills[1], entity);
		}

		entity->skills[2].triggerHandle(state, &entity->skills[2], entity);
	}

#define MAX_JUMP_TIME 0.25f
#define MAX_STOP_TIME 0.05f
#define MAX_NB_JUMPS 2

	// NOTE(Charly): We are updating a player, so we need to :
	//  - Change x velocity based on left / right inputs
	//  - If jump is pressed:
	//      - Is it the start of a new jump ?
	//          - Y: start jumping, compute gravity and velocity
	//          based on current state and wished jump height,
	//          keep track of the number of total jumps (gd related)
	//          - N: update jumping elapsed time
	//  - Else:
	//      - Did we begin a jump and stopped early ?
	//          - Change the gravity momentarily and track time
	real32 oldX = entity->p.x;

	z::vec2 acc    = z::Vec2(0, entity->gravity);
	z::vec2 motion = entity->dp;

	if (!(entity->status & (EntityStatus_Rooted | EntityStatus_Stunned)))
	{
		if (IsActionPressed(state, controllerId, Action_Left))
		{
			motion.x += -10.0;
		}

		if (IsActionPressed(state, controllerId, Action_Right))
		{
			motion.x += 10.0;
		}

		if (IsActionPressed(state, controllerId, Action_Jump))
		{
			if (IsActionRisingEdge(state, controllerId, Action_Jump) &&
			    (!entity->alreadyJumping || (entity->newJump && entity->nbJumps < MAX_NB_JUMPS)))
			{
				// Start jumping
				motion.y               = entity->initialJumpVelocity;
				entity->alreadyJumping = true;
				entity->newJump        = false;
				++entity->nbJumps;
				WentAirborne(entity);
			}
			else
			{
				entity->jumpTime += dt;
			}
		}
		else
		{
			entity->newJump = true;

			if (entity->alreadyJumping)
			{
				if (!entity->quickFall && entity->jumpTime < MAX_JUMP_TIME)
				{
					entity->quickFall     = true;
					entity->quickFallTime = 0;
				}

				if (entity->quickFall && entity->quickFallTime < MAX_STOP_TIME)
				{
					entity->quickFallTime += dt;
					acc.y *= 5;
				}
			}
		}
	}

	motion += dt * acc;
	entity->dp = MoveEntity(state, entity, motion);

	if (z::OppositeSign(entity->p.x - oldX, entity->orientation))
	{
		entity->orientation *= -1.f;
	}

	// Resolve skills
	for (uint32 i = 0; i < NB_SKILLS; ++i)
	{
		if (entity->skills[i].applyHandle != nullptr)
		{
			entity->skills[i].applyHandle(state, &entity->skills[i], entity, dt);
		}
	}
}
