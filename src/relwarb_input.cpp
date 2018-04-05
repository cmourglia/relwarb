#include "relwarb_input.h"

#include "relwarb.h"

bool32 IsKeyPressed(int32 key)
{
	if (key == Key_Unknown)
	{
		return false;
	}

	const bool32 result = state->inputState.keyboard.keys[key];
	return result;
}

bool32 IsKeyRisingEdge(int32 key)
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

bool32 IsKeyFallingEdge(int32 key)
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

bool32 IsMouseButtonPressed(int32 button)
{
	const bool32 result = state->inputState.mouse.buttons[button];
	return result;
}

bool32 IsMouseButtonRisingEdge(int32 button)
{
	const bool32 last = state->lastInputState.mouse.buttons[button];
	const bool32 curr = state->inputState.mouse.buttons[button];

	const bool32 result = curr && !last;
	return result;
}

bool32 IsMouseButtonFallingEdge(int32 button)
{
	const bool32 last = state->lastInputState.mouse.buttons[button];
	const bool32 curr = state->inputState.mouse.buttons[button];

	const bool32 result = last && !curr;
	return result;
}

bool32 IsGamepadButtonPressed(int32 pad, int32 button)
{
	const bool32 result = state->inputState.gamepads[pad].buttons[button];
	return result;
}

bool32 IsGamepadButtonRisingEdge(int32 pad, int32 button)
{
	const bool32 last = state->lastInputState.gamepads[pad].buttons[button];
	const bool32 curr = state->inputState.gamepads[pad].buttons[button];

	const bool32 result = curr && !last;
	return result;
}

bool32 IsGamepadButtonFallingEdge(int32 pad, int32 button)
{
	const bool32 last = state->lastInputState.gamepads[pad].buttons[button];
	const bool32 curr = state->inputState.gamepads[pad].buttons[button];

	const bool32 result = last && !curr;
	return result;
}

z::vec2 GetCursorPosition()
{
	const z::vec2 result = state->inputState.mouse.cursor;
	return result;
}

z::vec2 GetCursorWorldPosition()
{
	const z::vec2 result = ViewportToWorld(GetCursorPosition());
	return result;
}