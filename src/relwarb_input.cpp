#include "relwarb_input.h"

#include "relwarb.h"

bool32 IsKeyPressed(GameState* state, int32 key)
{
	if (key == Key_Unknown)
	{
		return false;
	}

	const bool32 result = state->inputState.keyboard.keys[key];
	return result;
}

bool32 IsKeyRisingEdge(GameState* state, int32 key)
{
	if (key == Key_Unknown)
	{
		return false;
	}

	const bool32 last = state->lastInputState.keyboard.keys[key];
	const bool32 curr = state->inputState.keyboard.keys[key];

	const bool32 result = curr && !last;
	return result;
}

bool32 IsKeyFallingEdge(GameState* state, int32 key)
{
	if (key == Key_Unknown)
	{
		return false;
	}

	const bool32 last = state->lastInputState.keyboard.keys[key];
	const bool32 curr = state->inputState.keyboard.keys[key];

	const bool32 result = last && !curr;
	return result;
}

bool32 IsMouseButtonPressed(GameState* state, int32 button)
{
	const bool32 result = state->inputState.mouse.buttons[button];
	return result;
}

bool32 IsMouseButtonRisingEdge(GameState* state, int32 button)
{
	const bool32 last = state->lastInputState.mouse.buttons[button];
	const bool32 curr = state->inputState.mouse.buttons[button];

	const bool32 result = curr && !last;
	return result;
}

bool32 IsMouseButtonFallingEdge(GameState* state, int32 button)
{
	const bool32 last = state->lastInputState.mouse.buttons[button];
	const bool32 curr = state->inputState.mouse.buttons[button];

	const bool32 result = last && !curr;
	return result;
}

bool32 IsGamepadButtonPressed(GameState* state, int32 pad, int32 button)
{
	const bool32 result = state->inputState.gamepads[pad].buttons[button];
	return result;
}

bool32 IsGamepadButtonRisingEdge(GameState* state, int32 pad, int32 button)
{
	const bool32 last = state->lastInputState.gamepads[pad].buttons[button];
	const bool32 curr = state->inputState.gamepads[pad].buttons[button];

	const bool32 result = curr && !last;
	return result;
}

bool32 IsGamepadButtonFallingEdge(GameState* state, int32 pad, int32 button)
{
	const bool32 last = state->lastInputState.gamepads[pad].buttons[button];
	const bool32 curr = state->inputState.gamepads[pad].buttons[button];

	const bool32 result = last && !curr;
	return result;
}

z::vec2 GetCursorPosition(GameState* state)
{
	const z::vec2 result = state->inputState.mouse.cursor;
	return result;
}

z::vec2 GetCursorWorldPosition(GameState* state)
{
	const z::vec2 result = ViewportToWorld(state, GetCursorPosition(state));
	return result;
}