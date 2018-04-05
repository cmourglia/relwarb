#ifndef RELWARB_INPUT_H
#define RELWARB_INPUT_H

#include "relwarb_defines.h"
#include "relwarb_math.h"

enum Key
{
	Key_Unknown = -1,
	Key_A       = 0,
	Key_B,
	Key_C,
	Key_D,
	Key_E,
	Key_F,
	Key_G,
	Key_H,
	Key_I,
	Key_J,
	Key_K,
	Key_L,
	Key_M,
	Key_N,
	Key_O,
	Key_P,
	Key_Q,
	Key_R,
	Key_S,
	Key_T,
	Key_U,
	Key_V,
	Key_W,
	Key_X,
	Key_Y,
	Key_Z,
	Key_0,
	Key_1,
	Key_2,
	Key_3,
	Key_4,
	Key_5,
	Key_6,
	Key_7,
	Key_8,
	Key_9,
	Key_F1,
	Key_F2,
	Key_F3,
	Key_F4,
	Key_F5,
	Key_F6,
	Key_F7,
	Key_F8,
	Key_F9,
	Key_F10,
	Key_F11,
	Key_F12,
	Key_Esc,
	Key_Space,
	Key_Enter,
	Key_LCtrl,
	Key_RCltr,
	Key_LShift,
	Key_RShift,
	Key_LAlt,
	Key_RAlt,
	Key_Tab,
	Key_Left,
	Key_Right,
	Key_Up,
	Key_Down,
	Key_Count,
};

enum MouseButton
{
	MouseButton_Unknown = -1,
	MouseButton_Left    = 0,
	MouseButton_Right,
	MouseButton_Middle,
	MouseButton_Count,
};

enum GamepadButton
{
	GamepadButton_A = 0,
	GamepadButton_B,
	GamepadButton_X,
	GamepadButton_Y,
	GamepadButton_LeftShoulder,
	GamepadButton_RightShoulder,
	GamepadButton_Back,
	GamepadButton_Start,
	GamepadButton_Menu,
	GamepadButton_LeftThumb,
	GamepadButton_RightThumb,
	GamepadButton_PadUp,
	GamepadButton_PadRight,
	GamepadButton_PadDown,
	GamepadButton_PadLeft,
	GamepadButton_Count,
};

struct KeyboardState
{
	bool32 keys[Key_Count];
};

struct MouseState
{
	z::vec2 cursor;
	bool32  buttons[MouseButton_Count];
};

struct GamepadState
{
	z::vec2 leftThumbStick;
	z::vec2 rightThumbStick;
	real32  leftTrigger;
	real32  rightTrigger;
	bool32  buttons[GamepadButton_Count];
};

static const int MAX_GAMEPADS = 4;

struct InputState
{
	KeyboardState keyboard;
	MouseState    mouse;
	GamepadState  gamepads[MAX_GAMEPADS];
};

struct GameState;
bool32 IsKeyPressed(int32 key);
bool32 IsKeyRisingEdge(int32 key);
bool32 IsKeyFallingEdge(int32 key);
bool32 IsMouseButtonPressed(int32 button);
bool32 IsMouseButtonRisingEdge(int32 button);
bool32 IsMouseButtonFallingEdge(int32 button);
bool32 IsGamepadButtonPressed(int32 pad, int32 button);
bool32 IsGamepadButtonRisingEdge(int32 pad, int32 button);
bool32 IsGamepadButtonFallingEdge(int32 pad, int32 button);

z::vec2 GetCursorPosition();
z::vec2 GetCursorWorldPosition();

#endif // RELWARB_INPUT_H
