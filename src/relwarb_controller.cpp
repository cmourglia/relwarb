#include "relwarb_controller.h"

#include "relwarb.h"
#include "relwarb_input.h"

#include <assert.h>
#include <string.h>

void SetControllerType(GameState* state, int32 id, ControllerType type)
{
	assert(id >= 0 && id < MAX_CONTROLLERS);
	state->controllers[id].controllerType = type;
}

ControllerType GetControllerType(GameState* state, int32 id)
{
	assert(id >= 0 && id < MAX_CONTROLLERS);
	ControllerType result = state->controllers[id].controllerType;
	return result;
}

void SetControllerGamepadId(GameState* state, int32 id, int32 gid)
{
	assert(id >= 0 && id < MAX_CONTROLLERS);
	assert(gid >= 0 && gid < MAX_GAMEPADS);

	state->controllers[id].gamepadId = gid;
}

void ConfigureController(GameState* state, int32 id, ControllerType type, int32 gid)
{
	SetControllerType(state, id, type);
	if (type == ControllerType_Gamepad)
	{
		SetControllerGamepadId(state, id, gid);
	}

	memset(state->controllers[id].actionToInput, -1, Action_Count * sizeof(int32));
}

void MapActionToInput(GameState* state, int32 id, int32 action, int32 input)
{
	assert(id >= 0 && id < MAX_CONTROLLERS);
	assert(action >= 0 && action < Action_Count);

	Controller* controller            = state->controllers + id;
	controller->actionToInput[action] = input;
}

bool32 IsActionPressed(GameState* state, int32 id, int32 action)
{
	assert(id >= 0 && id < MAX_CONTROLLERS);
	assert(action >= 0 && action < Action_Count);

	Controller* controller = state->controllers + id;
	int32       input      = controller->actionToInput[action];

	bool32 result;

	switch (controller->controllerType)
	{
		case ControllerType_Keyboard:
		{
			result = IsKeyPressed(state, input);
		}
		break;

		case ControllerType_Gamepad:
		{
			int32 gid = controller->gamepadId;
			result    = IsGamepadButtonPressed(state, gid, input);
		}
		break;

		default:
			result = false;
	}

	return result;
}

bool32 IsActionRisingEdge(GameState* state, int32 id, int32 action)
{
	assert(id >= 0 && id < MAX_CONTROLLERS);
	assert(action >= 0 && action < Action_Count);

	Controller* controller = state->controllers + id;
	int32       input      = controller->actionToInput[action];

	bool32 result;

	switch (controller->controllerType)
	{
		case ControllerType_Keyboard:
		{
			result = IsKeyRisingEdge(state, input);
		}
		break;

		case ControllerType_Gamepad:
		{
			int32 gid = controller->gamepadId;
			result    = IsGamepadButtonRisingEdge(state, gid, input);
		}
		break;

		default:
			result = false;
	}

	return result;
}

bool32 IsActionFallingEdge(GameState* state, int32 id, int32 action)
{
	assert(id >= 0 && id < MAX_CONTROLLERS);
	assert(action >= 0 && action < Action_Count);

	Controller* controller = state->controllers + id;
	int32       input      = controller->actionToInput[action];

	bool32 result;

	switch (controller->controllerType)
	{
		case ControllerType_Keyboard:
		{
			result = IsKeyFallingEdge(state, input);
		}
		break;

		case ControllerType_Gamepad:
		{
			int32 gid = controller->gamepadId;
			result    = IsGamepadButtonFallingEdge(state, gid, input);
		}
		break;

		default:
			result = false;
	}

	return result;
}