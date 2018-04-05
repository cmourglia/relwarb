#include "relwarb_defines.h"
#include "relwarb_opengl.h"
#include "relwarb_utils.h"
#include "relwarb_debug.h"
#include "relwarb_input.h"
#include "relwarb.h"

#include <GLFW/glfw3.h>

#include <assert.h>
#include <chrono>
#include <stdio.h>

global_variable uint32 worldWindowWidth  = 1440;
global_variable uint32 worldWindowHeight = 720;

global_variable const int keys[Key_Count] = {
    GLFW_KEY_A,
    GLFW_KEY_B,
    GLFW_KEY_C,
    GLFW_KEY_D,
    GLFW_KEY_E,
    GLFW_KEY_F,
    GLFW_KEY_G,
    GLFW_KEY_H,
    GLFW_KEY_I,
    GLFW_KEY_J,
    GLFW_KEY_K,
    GLFW_KEY_L,
    GLFW_KEY_M,
    GLFW_KEY_N,
    GLFW_KEY_O,
    GLFW_KEY_P,
    GLFW_KEY_Q,
    GLFW_KEY_R,
    GLFW_KEY_S,
    GLFW_KEY_T,
    GLFW_KEY_U,
    GLFW_KEY_V,
    GLFW_KEY_W,
    GLFW_KEY_X,
    GLFW_KEY_Y,
    GLFW_KEY_Z,
    GLFW_KEY_0,
    GLFW_KEY_1,
    GLFW_KEY_2,
    GLFW_KEY_3,
    GLFW_KEY_4,
    GLFW_KEY_5,
    GLFW_KEY_6,
    GLFW_KEY_7,
    GLFW_KEY_8,
    GLFW_KEY_9,
    GLFW_KEY_F1,
    GLFW_KEY_F2,
    GLFW_KEY_F3,
    GLFW_KEY_F4,
    GLFW_KEY_F5,
    GLFW_KEY_F6,
    GLFW_KEY_F7,
    GLFW_KEY_F8,
    GLFW_KEY_F9,
    GLFW_KEY_F10,
    GLFW_KEY_F11,
    GLFW_KEY_F12,
    GLFW_KEY_ESCAPE,
    GLFW_KEY_SPACE,
    GLFW_KEY_ENTER,
    GLFW_KEY_LEFT_CONTROL,
    GLFW_KEY_RIGHT_CONTROL,
    GLFW_KEY_LEFT_SHIFT,
    GLFW_KEY_RIGHT_SHIFT,
    GLFW_KEY_LEFT_ALT,
    GLFW_KEY_RIGHT_ALT,
    GLFW_KEY_TAB,
    GLFW_KEY_LEFT,
    GLFW_KEY_RIGHT,
    GLFW_KEY_UP,
    GLFW_KEY_DOWN,
};

global_variable const int mouseButtons[MouseButton_Count] = {
    GLFW_MOUSE_BUTTON_1,
    GLFW_MOUSE_BUTTON_2,
    GLFW_MOUSE_BUTTON_3,
};

internal void ProcessKeyboard(GLFWwindow* window, InputState* state)
{
	// TODO(charly): Handle esc for real (transition between screens, etc)
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	KeyboardState* kstate = &state->keyboard;

	for (int k = 0; k < Key_Count; ++k)
	{
		kstate->keys[k] = (glfwGetKey(window, keys[k]) == GLFW_PRESS);
	}
}

internal void ProcessMouse(GLFWwindow* window, InputState* state)
{
	MouseState* mstate = &state->mouse;

	double x, y;
	glfwGetCursorPos(window, &x, &y);

	mstate->cursor.x = x;
	mstate->cursor.y = y;

	for (int b = 0; b < MouseButton_Count; ++b)
	{
		mstate->buttons[b] = (glfwGetMouseButton(window, mouseButtons[b]) == GLFW_PRESS);
	}
}

internal void ProcessJoystick(GLFWwindow* window, InputState* state)
{
	(void)window;
	// X-Box controller:
	//  Axes:
	//      - 0-1:  Left thumbstick (-1 left, 1 right, -1 up, 1 down)
	//      - 2:    LT (1 fully pressed)
	//      - 3-4:  Right thumbstick (-1 left, 1 right, -1 up, 1 down)
	//      - 5:    RT (1 fully pressed)
	//  Buttons:
	//      - 0 : A
	//      - 1 : B
	//      - 2 : X
	//      - 3 : Y
	//      - 4 : LB
	//      - 5 : RB
	//      - 6 : back
	//      - 7 : start
	//      - 8 : menu
	//      - 9 : Left stick click
	//      - 10: Right stick click
	//      - 11: Left joystick up
	//      - 12: Left joystick right
	//      - 13: Left joystick down
	//      - 14: Left joystick left

	int currentJoystick = 0;
	for (int jid = 0; jid < GLFW_JOYSTICK_LAST && currentJoystick < MAX_GAMEPADS; ++jid)
	{
		if (glfwJoystickPresent(jid))
		{
			int                  axisCount, buttonCount;
			const float*         axes    = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);
			const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

			GamepadState* gstate = state->gamepads + currentJoystick;

			// TODO(charly): Maybe we should use constants instead of magic numbers ? ...
			gstate->leftThumbStick  = z::Vec2(axes[0], axes[1]);
			gstate->rightThumbStick = z::Vec2(axes[3], axes[4]);
			gstate->leftTrigger     = axes[2];
			gstate->rightTrigger    = axes[5];

			for (int button = 0; button < GamepadButton_Count; ++button)
			{
				gstate->buttons[button] = (buttons[button] == GLFW_PRESS);
			}

			static const real32 DEAD_ZONE = 0.25;
			// HACK(charly): Set the pad state to the same as the left thumbstick
			if (axes[0] > DEAD_ZONE)
			{
				gstate->buttons[GamepadButton_PadRight] = true;
			}
			else if (axes[0] < -DEAD_ZONE)
			{
				gstate->buttons[GamepadButton_PadLeft] = true;
			}

			if (axes[1] > DEAD_ZONE)
			{
				gstate->buttons[GamepadButton_PadDown] = true;
			}
			else if (axes[1] < -DEAD_ZONE)
			{
				gstate->buttons[GamepadButton_PadUp] = true;
			}

			++currentJoystick;
		}
	}
}

internal void ProcessInputState(GLFWwindow* window, InputState* state)
{
	ProcessKeyboard(window, state);
	ProcessMouse(window, state);
	ProcessJoystick(window, state);
}

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(worldWindowWidth,
	                                      worldWindowHeight,
	                                      "Relwarb",
	                                      nullptr,
	                                      nullptr);
	glfwMakeContextCurrent(window);
	gl3wInit();

	state               = new GameState;
	state->viewportSize = z::Vec2(worldWindowWidth, worldWindowHeight);

	InitGame();

	using Clock     = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using Seconds   = std::chrono::seconds::period;

	TimePoint t0 = Clock::now();
	TimePoint t1;

	glfwSwapInterval(1);

	static const float MAX_FRAME_TIME = 2.0f / 60.0f;

	while (!glfwWindowShouldClose(window))
	{
		t1        = Clock::now();
		real32 dt = std::chrono::duration<float, Seconds>(t1 - t0).count();
		t0        = t1;

		if (dt > MAX_FRAME_TIME)
		{
			continue;
		}

		glfwPollEvents();

		state->lastInputState = state->inputState;
		ProcessInputState(window, &state->inputState);

		UpdateGame(dt);
		RenderGame(dt);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}