#include "relwarb_defines.h"
#include "relwarb_opengl.h"
#include "relwarb_utils.h"
#include "relwarb_debug.h"
#include "relwarb_input.h"
#include "relwarb.h"

#include <GLFW/glfw3.h>

#include <chrono>

global_variable uint32 worldWindowWidth = 1440;
global_variable uint32 worldWindowHeight = 720;

void KeyCallback(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods)
{
    GameState* state = (GameState*)glfwGetWindowUserPointer(window);

    if (action == GLFW_REPEAT)
    {
        return;
    }

    bool isDown = (action == GLFW_PRESS);
    switch (key)
    {
        case GLFW_KEY_LEFT:
        {
            state->controllers[0].moveLeft = isDown;
        } break;

        case GLFW_KEY_RIGHT:
        {
            state->controllers[0].moveRight = isDown;
        } break;

        case GLFW_KEY_UP:
        {
            state->controllers[0].jump = isDown;
            state->controllers[0].newJump = isDown;
        } break;

        case GLFW_KEY_SPACE:
        {
            state->controllers[0].dash = isDown;
            state->controllers[0].newDash = isDown;
        } break;

        case GLFW_KEY_LEFT_SHIFT:
        {
            state->controllers[0].mana = isDown;
            state->controllers[0].newMana = isDown;
        }

        default: {}
    }
}

void MouseButtonCallback(GLFWwindow* window, int32 button, int32 action, int32 mods)
{
    GameState* state = (GameState*)glfwGetWindowUserPointer(window);

}

void ProcessJoystick(GameState* state)
{
    // TODO
}

int main() {

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(worldWindowWidth, worldWindowHeight, "Relwarb", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gl3wInit();

    GameState gameState = {};
    gameState.viewportSize = z::Vec2(worldWindowWidth, worldWindowHeight);

    glfwSetWindowUserPointer(window, &gameState);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);

    InitGame(&gameState);

    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Seconds = std::chrono::seconds::period;

    TimePoint t0 = Clock::now();
    TimePoint t1;

    glfwSwapInterval(0);

    while (!glfwWindowShouldClose(window)) {
        t1 = Clock::now();
        real32 dt = std::chrono::duration<float, Seconds>(t1 - t0).count();
        t0 = t1;

        glfwPollEvents();

        UpdateGame(&gameState, dt);
        RenderGame(&gameState, dt);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}