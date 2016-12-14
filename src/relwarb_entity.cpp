#include "relwarb_entity.h"

#include "relwarb.h"
#include "relwarb_world_sim.h"
#include "relwarb_renderer.h"

Entity* CreatePlayerEntity(GameState* state, Vec2 p, 
						   RenderingPattern* pattern, 
						   Shape* shape, 
						   Controller* controller)
{
	Entity* result = CreateEntity(state, EntityType_Player, p);

	AddRenderingPatternToEntity(result, pattern);
	AddShapeToEntity(result, shape);

	// FIXME(Charly): Load this from files 
	result->playerSpeed = 40.f; 
	result->playerJumpHeight = 5.f;
	result->playerJumpDist = 16.f;

	result->initialJumpVelocity = (2 * result->playerJumpHeight * result->playerSpeed) / result->playerJumpDist;
	result->gravity = (-2 * result->playerJumpHeight * result->playerSpeed * result->playerSpeed) / (result->playerJumpDist * result->playerJumpDist);

	result->controller = controller;

	return result;
}