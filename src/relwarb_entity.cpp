#include "relwarb_entity.h"

#include "relwarb.h"
#include "relwarb_world_sim.h"
#include "relwarb_renderer.h"
#include "relwarb_debug.h"

Entity* CreatePlayerEntity(GameState* state, Vec2 p, 
						   RenderingPattern* pattern, 
						   Shape* shape, 
						   Controller* controller)
{
	Entity* result = CreateEntity(state, EntityType_Player, p);
	state->players[state->nbPlayers++] = result;

	AddRenderingPatternToEntity(result, pattern);
	AddShapeToEntity(result, shape);
	SetEntityComponent(result, ComponentFlag_Movable);

	// FIXME(Charly): Load this from files
	result->avatar = CreateBitmap(state);
	switch (state->nbPlayers)
	{
		case 1:
		{
			LoadBitmapData("assets/p1_avatar.png", result->avatar);
		} break;
		case 2:
		{
			LoadBitmapData("assets/p2_avatar.png", result->avatar);
		}break;
		case 3:
		{
			//LoadBitmapData("assets/p3_avatar.png", result->avatar);
		} break;
		case 4:
		{
			//LoadBitmapData("assets/p4_avatar.png", result->avatar);
		}break;
		default:
			Log(Log_Error, "Invalid number of players");
	}
	result->max_health = 10;
	result->health = 7;
	result->max_mana = 5;
	result->mana = 5;
	result->playerSpeed = 40.f; 
	result->playerJumpHeight = 5.f;
	result->playerJumpDist = 16.f;
	result->initialJumpVelocity = (2 * result->playerJumpHeight * result->playerSpeed) / result->playerJumpDist;
	result->gravity = (-2 * result->playerJumpHeight * result->playerSpeed * result->playerSpeed) / (result->playerJumpDist * result->playerJumpDist);
	result->status = 0;

	CreateDashSkill(&result->skills[0], result);
	CreateManaRecharge(&result->skills[1], result);
	CreateManaRecharge(&result->skills[2], result);

	result->controller = controller;

	return result;
}

Entity* CreateWallEntity(GameState* state, Vec2 p, RenderingPattern* pattern, Shape* shape)
{
	Entity* result = CreateEntity(state, EntityType_Wall, p);

	AddRenderingPatternToEntity(result, pattern);
	AddShapeToEntity(result, shape);

	return result;
}

// cf. http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?page=3
bool32 Intersect(const Entity* entity1, const Entity* entity2)
{
	// NOTE(Thomas): Inliner ?
	const Vec2& pos1 = entity1->p;
	const Vec2& pos2 = entity2->p;
	const Shape* shape1 = entity1->shape;
	const Shape* shape2 = entity2->shape;

	real32 diffX = Abs(pos1.x + shape1->offset.x - pos2.x - shape2->offset.x);
	real32 diffY = Abs(pos1.y + shape1->offset.y - pos2.y - shape2->offset.y);
	return (diffX < (shape1->size.x + shape2->size.x) * 0.5f && diffY < (shape1->size.y + shape2->size.y) * 0.5f);
}

Vec2 Overlap(const Entity* entity1, const Entity* entity2)
{
	// NOTE(Thomas): Inliner ?
	const Vec2& pos1 = entity1->p;
	const Vec2& pos2 = entity2->p;
	const Shape* shape1 = entity1->shape;
	const Shape* shape2 = entity2->shape;

	Vec2 over(pos1.x + shape1->offset.x - pos2.x - shape2->offset.x,
		pos1.y + shape1->offset.y - pos2.y - shape2->offset.y);
	Vec2 size(shape1->size + shape2->size);
	Vec2 sign(over.x > 0.f ? 1.f : -1.f, over.y > 0.f ? 1.f : -1.f);
	return over - 0.5f * Times(sign, size);
}