#ifndef RELWARB_INPUT_H
#define RELWARB_INPUT_H

struct InputState
{
	bool32 clicked;
	bool32 stateChange;

	int cursorX, cursorY;
};

enum Key
{
	Key_Unknown = -1,
	Key_A = 0,
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
	Key_Ctrl,
	Key_Shift,
	Key_Alt,
	Key_Tab,
	Key_Left,
	Key_Right,
	Key_Up,
	Key_Down,
	Key_Count,
};

enum Button
{
	Button_Unknown = -1,
	Button_Left = 0,
	Button_Right,
	Button_Middle,
	Button_Count,
};

inline bool32 InputUpFront(InputState* state)
{
	bool32 result = (state->stateChange && state->clicked);
	return result;
}

inline bool32 InputDownFront(InputState* state)
{
	bool32 result = (state->stateChange && !state->clicked);
	return result;
}

#endif // RELWARB_INPUT_H
