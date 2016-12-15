// TODO(Charly): Remove this
#include <string.h>

#include "relwarb_defines.h"
#include "relwarb_utils.h"
#include "relwarb_opengl.h"
#include "relwarb_debug.h"
#include "relwarb_input.h"
#include "relwarb.h"

#include <xinput.h>

global_variable bool32 g_running;
global_variable WORD g_vibrationLevel;

global_variable uint32 localLeft;
global_variable uint32 localRight;
global_variable uint32 localUp;

#define WGL_GET_AND_CHECK(name, type)                                   \
do {                                                                    \
    name = (type)wglGetProcAddress(STRINGIFY(name));                    \
    if (name == 0 ||                                                    \
        (name == (void*)0x1) || (name == (void*)0x2) ||                 \
        (name == (void*)0x3) || (name == (void*)-1)) {                  \
        HMODULE module = LoadLibraryA("opengl32.dll");                  \
        name = (type)GetProcAddress(module, STRINGIFY(name));           \
    }                                                                   \
    Assert(name);                                                       \
} while (false)

#define WGL_DRAW_TO_WINDOW_ARB                      0x2001
#define WGL_ACCELERATION_ARB                        0x2003
#define WGL_FULL_ACCELERATION_ARB                   0x2027
#define WGL_SUPPORT_OPENGL_ARB                      0x2010
#define WGL_DOUBLE_BUFFER_ARB                       0x2011
#define WGL_PIXEL_TYPE_ARB                          0x2013
#define WGL_TYPE_RGBA_ARB                           0x202B

#define WGL_CONTEXT_MAJOR_VERSION_ARB               0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB               0x2092
#define WGL_CONTEXT_DEBUG_BIT_ARB                   0x0001
#define WGL_CONTEXT_PROFILE_MASK_ARB                0x9126
#define WGL_CONTEXT_FLAGS_ARB                       0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB            0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB   0x00000002

#define WGL_SAMPLE_BUFFERS_ARB                      0x2041
#define WGL_SAMPLES_ARB                             0x2042

typedef BOOL (WINAPI *def_wglGetPixelFormatAttribivARB)(HDC, int, int, UINT, const int*, int*);
typedef BOOL (WINAPI *def_wglGetPixelFormatAttribfvARB)(HDC, int, int, UINT, const int*, FLOAT*);
typedef BOOL (WINAPI *def_wglChoosePixelFormatARB)(HDC, const int*, const FLOAT*, UINT, int*, UINT*);
typedef HGLRC (WINAPI *def_wglCreateContextAttribsARB)(HDC, HGLRC, const int*);
typedef const char* (WINAPI *def_wglGetExtensionsStringARB)(HDC);
typedef BOOL (WINAPI *def_wglSwapIntervalEXT)(int);

global_variable def_wglGetPixelFormatAttribivARB wglGetPixelFormatAttribivARB;
global_variable def_wglGetPixelFormatAttribfvARB wglGetPixelFormatAttribfvARB;
global_variable def_wglChoosePixelFormatARB wglChoosePixelFormatARB;
global_variable def_wglCreateContextAttribsARB wglCreateContextAttribsARB;
global_variable def_wglGetExtensionsStringARB wglGetExtensionsStringARB;
global_variable def_wglSwapIntervalEXT wglSwapIntervalEXT;

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD, XINPUT_STATE*)
typedef X_INPUT_GET_STATE(def_XInputGetState);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable def_XInputGetState* XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD, XINPUT_VIBRATION*)
typedef X_INPUT_SET_STATE(def_XInputSetState);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable def_XInputSetState* XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

LRESULT CALLBACK win32_MainWindowCallback(HWND, UINT, WPARAM, LPARAM);

internal void win32_SetPixelFormat(HDC hdc)
{
    int pixelFormat = 0;
    GLuint numFormat = 0;

    if(wglChoosePixelFormatARB)
    {
        int attribList[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_SAMPLE_BUFFERS_ARB, 1,
            WGL_SAMPLES_ARB, 16,
            // WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,
            0,
        };

        wglChoosePixelFormatARB(hdc, attribList, 0, 1, &pixelFormat, &numFormat);
    }

    // wgl not loaded yet, or could not figure out a proper pixel format
    if (!numFormat)
    {
        PIXELFORMATDESCRIPTOR requiredPixelFormat = {};
        requiredPixelFormat.nSize = sizeof(requiredPixelFormat);
        requiredPixelFormat.nVersion = 1;
        requiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
        requiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
        requiredPixelFormat.cColorBits = 32;
        requiredPixelFormat.cAlphaBits = 8;
        requiredPixelFormat.cDepthBits = 24;
        requiredPixelFormat.cStencilBits = 8;
        requiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

        pixelFormat = ChoosePixelFormat(hdc, &requiredPixelFormat);
    }

    PIXELFORMATDESCRIPTOR suggestedPixelFormat;
    DescribePixelFormat(hdc,
                        pixelFormat,
                        sizeof(suggestedPixelFormat),
                        &suggestedPixelFormat);

    SetPixelFormat(hdc, pixelFormat, &suggestedPixelFormat);
}

internal void win32_LoadWGLFunctions()
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "WGLFunctionsLoader";

    if (RegisterClass(&wc))
    {
        HWND window = CreateWindowExA(0,
                                      wc.lpszClassName,
                                      "FunctionsLoader",
                                      0,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      0,
                                      0,
                                      wc.hInstance,
                                      0);

        HDC hdc = GetDC(window);

        win32_SetPixelFormat(hdc);

        HGLRC glrc = wglCreateContext(hdc);

        if (wglMakeCurrent(hdc, glrc))
        {
            // Retrieve wgl function pointers
            WGL_GET_AND_CHECK(wglGetPixelFormatAttribivARB, def_wglGetPixelFormatAttribivARB);
            WGL_GET_AND_CHECK(wglGetPixelFormatAttribfvARB, def_wglGetPixelFormatAttribfvARB);
            WGL_GET_AND_CHECK(wglChoosePixelFormatARB, def_wglChoosePixelFormatARB);
            WGL_GET_AND_CHECK(wglCreateContextAttribsARB, def_wglCreateContextAttribsARB);
            WGL_GET_AND_CHECK(wglGetExtensionsStringARB, def_wglGetExtensionsStringARB);
			WGL_GET_AND_CHECK(wglSwapIntervalEXT, def_wglSwapIntervalEXT);

            wglMakeCurrent(nullptr, nullptr);
        }

        wglDeleteContext(glrc);
        ReleaseDC(window, hdc);
        DestroyWindow(window);
    }
}

internal void win32_GLDebugOutput(GLenum source,
                                  GLenum type,
                                  GLuint id,
                                  GLenum severity,
                                  GLsizei length,
                                  const GLchar* message,
                                  const void* userParam)
{
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR_ARB:
        {
            OutputDebugString("OpenGL error ");
        } break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
        {
            OutputDebugString("OpenGL deprecated behaviour ");
        } break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
        {
            OutputDebugString("OpenGL undefined behaviour ");
        } break;

        case GL_DEBUG_TYPE_PORTABILITY_ARB:
        {
            OutputDebugString("OpenGL portability issue ");
        } break;

        case GL_DEBUG_TYPE_PERFORMANCE_ARB:
        {
            OutputDebugString("OpenGL performance issue ");
        } break;

        case GL_DEBUG_TYPE_OTHER_ARB:
        default:
        {
            OutputDebugString("OpenGL other issue ");
        }
    }

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH_ARB:
        {
            OutputDebugString("(HIGH): ");
        } break;

        case GL_DEBUG_SEVERITY_MEDIUM_ARB:
        {
            OutputDebugString("(MEDIUM): ");
        } break;

        case GL_DEBUG_SEVERITY_LOW_ARB:
        {
            OutputDebugString("(LOW): ");
        } break;

        default: {}
    }

    OutputDebugString(message);
    OutputDebugString("\n");
}

internal HGLRC win32_InitOpenGL(HDC hdc)
{
    win32_LoadWGLFunctions();
    win32_SetPixelFormat(hdc);

    HGLRC openglRC = 0;

    OutputDebugString("Hello there\n");

    if (wglCreateContextAttribsARB)
    {
        int attribList[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            // TODO(Charly): Toggle this flag depending on the build type
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
            // TODO(Charly): Swap to WGL_CONTEXT_CORE_PROFILE_BIT_ARB
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0,
        };

        openglRC = wglCreateContextAttribsARB(hdc, 0, attribList);

        if (!openglRC)
        {
            OutputDebugString("Could not get an OpenGL 3.3 context.\n");
            return 0;
        }

        if (wglMakeCurrent(hdc, openglRC))
        {
            gl3wInit();

            int extensionsCount = 0;
            glGetIntegerv(GL_NUM_EXTENSIONS, &extensionsCount);

            for (int extensionIndex = 0; extensionIndex < extensionsCount; ++extensionIndex)
            {
                const char* extensionName = (const char*)glGetStringi(GL_EXTENSIONS, extensionIndex);
                if (StrEqual(extensionName, "GL_ARB_debug_output", StrLength("GL_ARB_debug_output")))
                {
                    // WGL_GET_AND_CHECK(glDebugMessageCallbackARB, def_glDebugMessageCallbackARB*);
                     glDebugMessageCallbackARB(win32_GLDebugOutput, nullptr);
                }
            }
        }
    }
    else
    {
        OutputDebugString("wglCreateContextAttribsARB has not been loaded\n");
    }

    return openglRC;
}

internal Key win32_ConvertKey(uint32 vkCode)
{
	switch (vkCode)
	{
		case 'A' : return Key_A;
		case 'B' : return Key_B;
		case 'C' : return Key_C;
		case 'D' : return Key_D;
		case 'E' : return Key_E;
		case 'F' : return Key_F;
		case 'G' : return Key_G;
		case 'H' : return Key_H;
		case 'I' : return Key_I;
		case 'J' : return Key_J;
		case 'K' : return Key_K;
		case 'L' : return Key_L;
		case 'M' : return Key_M;
		case 'N' : return Key_N;
		case 'O' : return Key_O;
		case 'P' : return Key_P;
		case 'Q' : return Key_Q;
		case 'R' : return Key_R;
		case 'S' : return Key_S;
		case 'T' : return Key_T;
		case 'U' : return Key_U;
		case 'V' : return Key_V;
		case 'W' : return Key_W;
		case 'X' : return Key_X;
		case 'Y' : return Key_Y;
		case 'Z' : return Key_Z;
		case '0' : return Key_0;
		case '1' : return Key_1;
		case '2' : return Key_2;
		case '3' : return Key_3;
		case '4' : return Key_4;
		case '5' : return Key_5;
		case '6' : return Key_6;
		case '7' : return Key_7;
		case '8' : return Key_8;
		case '9' : return Key_9;
		case VK_F1 : return Key_F1;
		case VK_F2 : return Key_F2;
		case VK_F3 : return Key_F3;
		case VK_F4 : return Key_F4;
		case VK_F5 : return Key_F5;
		case VK_F6 : return Key_F6;
		case VK_F7 : return Key_F7;
		case VK_F8 : return Key_F8;
		case VK_F9 : return Key_F9;
		case VK_F10 : return Key_F10;
		case VK_F11 : return Key_F11;
		case VK_F12 : return Key_F12;
		case VK_ESCAPE : return Key_Esc;
		case VK_SPACE : return Key_Space;
		case VK_RETURN : return Key_Enter;
		case VK_CONTROL : return Key_Ctrl;
		case VK_SHIFT : return Key_Shift;
		case VK_MENU : return Key_Alt;
		case VK_TAB: return Key_Tab;
		case VK_LEFT: return Key_Left;
		case VK_RIGHT: return Key_Right;
		case VK_UP: return Key_Up;
		case VK_DOWN: return Key_Down;
		default: return Key_Unknown;
	}
}

internal Vec2 win32_GetCursorPos()
{
    POINT cursor;
    GetCursorPos(&cursor);
    ScreenToClient(GetActiveWindow(), &cursor);

    Vec2 result = Vec2(cursor.x, cursor.y);
    return result;
}

internal void win32_FillButtonState(GameState* gameState, Button bt, bool32 clicked)
{
	if (bt != Button_Unknown)
	{
		InputState state;
		state.clicked = clicked;
		state.stateChange = true;

        Vec2 cursor = win32_GetCursorPos();
        state.cursorX = cursor.x;
        state.cursorY = cursor.y;

		gameState->buttonStates[bt] = state;
	}
}

internal void win32_ProcessInputMessages(GameState* gameState)
{
    for (int key = 0; key < Key_Count; ++key)
    {
        gameState->keyStates[key].stateChange = false;
    }

    for (int button = 0; button < Button_Count; ++button)
    {
        gameState->buttonStates[button].stateChange = false;
    }

    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        switch (message.message)
        {
            case WM_QUIT:
            {
                g_running = false;
            } break;

			case WM_SIZE:
			{
				gameState->viewportSize = Vec2(LOWORD(message.lParam), HIWORD(message.lParam));
			} break;

            case WM_KEYUP:
            case WM_KEYDOWN:
            case WM_SYSKEYUP:
            case WM_SYSKEYDOWN:
            {
                uint32 vkCode = (uint32)message.wParam;
                // NOTE(Charly): RTFM !
                // https://msdn.microsoft.com/en-us/library/windows/desktop/ms646280(v=vs.85).aspx
                bool32 wasDown = ((message.lParam & (1 << 30)) != 0);
                bool32 isDown = ((message.lParam & (1 << 31)) == 0);

				if (isDown != wasDown)
				{
					Key key = win32_ConvertKey(vkCode);
					if (key != Key_Unknown)
					{
						InputState state;

						state.clicked = isDown;
						state.stateChange = true;
						
                        Vec2 cursor = win32_GetCursorPos();
						state.cursorX = cursor.x;
						state.cursorY = cursor.y;

						gameState->keyStates[key] = state;
					}

                    if (vkCode == VK_ESCAPE)
                    {
                        g_running = false;
                    }
                    else if (vkCode == VK_LEFT || vkCode == localLeft)
                    {
                        gameState->controllers[0].moveLeft = isDown;
                    }
                    else if (vkCode == VK_RIGHT || vkCode == localRight)
                    {
                        gameState->controllers[0].moveRight = isDown;
                    }
                    else if (vkCode == VK_UP || vkCode == VK_SPACE || vkCode == localUp)
                    {
                        gameState->controllers[0].jump = isDown;
                    }
                }
            } break;

			case WM_LBUTTONDOWN:
			{
				win32_FillButtonState(gameState, Button_Left, true);
			} break;

			case WM_RBUTTONDOWN:
			{
				win32_FillButtonState(gameState, Button_Right, true);
			} break;

			case WM_MBUTTONDOWN:
			{
				win32_FillButtonState(gameState, Button_Middle, true);
			} break;

			case WM_LBUTTONUP:
			{
				win32_FillButtonState(gameState, Button_Left, false);
			} break;

			case WM_RBUTTONUP:
			{
				win32_FillButtonState(gameState, Button_Right, false);
			} break;

			case WM_MBUTTONUP:
			{
				win32_FillButtonState(gameState, Button_Middle, false);
			} break;

            default:
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
    }

    gameState->cursor = win32_GetCursorPos();
}

internal void win32_InitXInput()
{
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
    if (!XInputLibrary)
    {
        XInputLibrary = LoadLibraryA("xinput1_3.dll");
    }

    if (XInputLibrary)
    {
        XInputGetState = (def_XInputGetState*)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (def_XInputSetState*)GetProcAddress(XInputLibrary, "XInputSetState");
    }
}

internal void win32_ProcessXBoxControllers(GameState* gameState)
{
    for (DWORD controllerIndex = 0; controllerIndex < XUSER_MAX_COUNT; ++controllerIndex)
    {
        XINPUT_STATE state = {};
        if (XInputGetState(controllerIndex, &state) == ERROR_SUCCESS)
        {
            XINPUT_GAMEPAD* pad = &state.Gamepad;
            bool32 bUp = (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
            bool32 bDown = (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
            bool32 bLeft = (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
            bool32 bRight = (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
            bool32 start = (pad->wButtons & XINPUT_GAMEPAD_START);
            bool32 back = (pad->wButtons & XINPUT_GAMEPAD_BACK);
            bool32 lshoulder = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
            bool32 rshoulder = (pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
            bool32 abutton = (pad->wButtons & XINPUT_GAMEPAD_A);
            bool32 bbutton = (pad->wButtons & XINPUT_GAMEPAD_B);
            bool32 xbutton = (pad->wButtons & XINPUT_GAMEPAD_X);
            bool32 ybutton = (pad->wButtons & XINPUT_GAMEPAD_Y);

            real32 lx = state.Gamepad.sThumbLX;
            real32 ly = state.Gamepad.sThumbLY;

#define INPUT_DEADZONE ((real32)(1 << 13))

            if (lx > INPUT_DEADZONE || bRight)
            {
                gameState->controllers[controllerIndex + 1].moveLeft = false;
                gameState->controllers[controllerIndex + 1].moveRight = true;
            }
            else if (lx < -INPUT_DEADZONE || bLeft)
            {
                gameState->controllers[controllerIndex + 1].moveLeft = true;
                gameState->controllers[controllerIndex + 1].moveRight = false;
            }
            else
            {
                gameState->controllers[controllerIndex + 1].moveLeft = false;
                gameState->controllers[controllerIndex + 1].moveRight = false;
            }

            // Vibrate on edges
            if (gameState->onEdge)
            {
                XINPUT_VIBRATION vibrations;
                // real32 left = 1.f - (gameState->character.posX * 0.5f + 0.5f);
                // real32 right = (gameState->character.posX + gameState->character.sizeX) * 0.5f + 0.5f;

                // vibrations.wLeftMotorSpeed = (WORD)(left * (real32)(1 << 15)) * g_vibrationLevel;
                // vibrations.wRightMotorSpeed = (WORD)(right * (real32)(1 << 15)) * g_vibrationLevel;

                XInputSetState(controllerIndex, &vibrations);
            }
            else
            {
                XINPUT_VIBRATION vibrations = {};
                XInputSetState(controllerIndex, &vibrations);
            }
        }
    }
}

int CALLBACK WinMain(HINSTANCE instance,
                     HINSTANCE prevInstance,
                     LPSTR     cmdLine,
                     int       cmdShow)
{
    WNDCLASS wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = win32_MainWindowCallback;
    wc.hInstance = instance;
    wc.hIcon = nullptr;
    wc.lpszClassName = "Win32RelwrabClassName";

    win32_InitXInput();

    if (RegisterClass(&wc))
    {
        RECT windowSize;
        windowSize.left = 0;
        windowSize.right = 1280;
        windowSize.top = 0;
        windowSize.bottom = 720;

        AdjustWindowRect(&windowSize, WS_OVERLAPPEDWINDOW, false);
        HWND window = CreateWindowExA(0,
                                      wc.lpszClassName,
                                      "Relwarb",
                                      WS_VISIBLE | WS_OVERLAPPED,
                                      0,
                                      0,
                                      windowSize.right - windowSize.left,
                                      windowSize.bottom - windowSize.top,
                                      0,
                                      0,
                                      instance,
                                      0);

        HDC openglDC = GetDC(window);
        HGLRC openglRC = win32_InitOpenGL(openglDC);

        wglMakeCurrent(openglDC, openglRC);
		wglSwapIntervalEXT(1);

        g_running = true;

        HKL layout = GetKeyboardLayout(0);
        // https://msdn.microsoft.com/en-us/library/windows/desktop/dd318693(v=vs.85).aspx
        switch (LOWORD(layout))
        {
            case 0x080c:
            case 0x0c0c:
            case 0x040c:
            case 0x140c:
            case 0x180c:
            case 0x100c:
            {
                localLeft = 'Q';
                localRight = 'D';
                localUp = 'Z';
            } break;

            default:
            {
                localLeft = 'A';
                localRight = 'D';
                localUp = 'W';
            }
        }

        GameState gameState;
		gameState = {0};
		gameState.viewportSize = Vec2(1280, 720);

        InitGame(&gameState);

		LARGE_INTEGER timerFreq;
		QueryPerformanceFrequency(&timerFreq);
		LARGE_INTEGER t0, t1;
		QueryPerformanceCounter(&t0);

        while (g_running)
        {
			QueryPerformanceCounter(&t1);
			real32 dt = (t1.QuadPart - t0.QuadPart) / (real32)timerFreq.QuadPart;
			t0 = t1;

            // TODO(Charly): Handle keyboard and xbox controller separatly
            //               For now xbox controller is commented out to avoid overriding keyboard state.
            win32_ProcessInputMessages(&gameState);
            // win32_ProcessXBoxControllers(&inputState, &gameState);

            UpdateGame(&gameState, dt);
            RenderGame(&gameState, dt);

            SwapBuffers(openglDC);
        }

        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(openglRC);
    }
    else
    {
        OutputDebugString("Could not register class\n");
    }

    return 0;
}


LRESULT CALLBACK win32_MainWindowCallback(HWND hwnd,
                                          UINT message,
                                          WPARAM wParam,
                                          LPARAM lParam)
{
    LRESULT result = 0;

    switch (message)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC hdc = BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
        } break;

        case WM_DESTROY:
        case WM_CLOSE:
        {
            g_running = false;
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            Assert(!"Missed a keyboard input");
        } break;

        default:
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}
