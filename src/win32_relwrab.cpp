#include <windows.h>
#include <stdint.h>

#include "relwrab_opengl.h"

#define global_variable static
#define internal static

typedef bool bool32;

typedef signed char int8;
typedef int16_t int16;
typedef int32_t int32;

typedef unsigned char uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef float real32;
typedef double real64;

struct InputState
{
    bool32 moveLeft;
    bool32 moveRight;
    bool32 moveUp;
    bool32 moveDown;
};

struct GameState
{
    real32 triX;
    real32 triY;
};

global_variable bool g_running;

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

typedef BOOL WINAPI def_wglGetPixelFormatAttribivARB(HDC hdc,
                                                     int iPixelFormat,
                                                     int iLayerPlane,
                                                     UINT nAttributes,
                                                     const int *piAttributes,
                                                     int *piValues);

typedef BOOL WINAPI def_wglGetPixelFormatAttribfvARB(HDC hdc,
                                                     int iPixelFormat,
                                                     int iLayerPlane,
                                                     UINT nAttributes,
                                                     const int *piAttributes,
                                                     FLOAT *pfValues);

typedef BOOL WINAPI def_wglChoosePixelFormatARB(HDC hdc,
                                                const int *piAttribIList,
                                                const FLOAT *pfAttribFList,
                                                UINT nMaxFormats,
                                                int *piFormats,
                                                UINT *nNumFormats);

typedef HGLRC def_wglCreateContextAttribsARB(HDC hDC,
                                             HGLRC hshareContext,
                                             const int *attribList);

global_variable def_wglGetPixelFormatAttribivARB* wglGetPixelFormatAttribivARB;
global_variable def_wglGetPixelFormatAttribfvARB* wglGetPixelFormatAttribfvARB;
global_variable def_wglChoosePixelFormatARB* wglChoosePixelFormatARB;
global_variable def_wglCreateContextAttribsARB* wglCreateContextAttribsARB;

LRESULT CALLBACK win32_MainWindowCallback(HWND, UINT, WPARAM, LPARAM);

internal void Render(GameState* gameState)
{
    glClearColor(0.3f, 0.8f, 0.7f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);

    const real32 size = 0.1f;
    glColor3f(1, 0, 0); glVertex2f(gameState->triX, gameState->triY);
    glColor3f(0, 1, 0); glVertex2f(gameState->triX + size, gameState->triY);
    glColor3f(0, 0, 1); glVertex2f(gameState->triX + size / 2, gameState->triY + size);

    glEnd();
    glFlush();
}

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
            WGL_SAMPLES_ARB, 8,
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
            wglGetPixelFormatAttribivARB = (def_wglGetPixelFormatAttribivARB*)wglGetProcAddress("wglGetPixelFormatAttribivARB");
            wglGetPixelFormatAttribfvARB = (def_wglGetPixelFormatAttribfvARB*)wglGetProcAddress("wglGetPixelFormatAttribfvARB");
            wglChoosePixelFormatARB = (def_wglChoosePixelFormatARB*)wglGetProcAddress("wglChoosePixelFormatARB");
            wglCreateContextAttribsARB = (def_wglCreateContextAttribsARB*)wglGetProcAddress("wglCreateContextAttribsARB");

            wglMakeCurrent(nullptr, nullptr);
        }

        wglDeleteContext(glrc);
        ReleaseDC(window, hdc);
        DestroyWindow(window);
    }
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
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
            // WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
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
            OutputDebugString("Ok, context is on\n");
        }
    }
    else
    {
        OutputDebugString("wglCreateContextAttribsARB has not been loaded\n");
    }

    return openglRC;
}

internal void win32_ProcessInputMessages(InputState* inputState)
{
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        switch (message.message)
        {
            case WM_QUIT:
            {
                g_running = false;
            } break;

            case WM_KEYUP:
            case WM_KEYDOWN:
            case WM_SYSKEYUP:
            case WM_SYSKEYDOWN:
            {
                uint32 vkCode = (uint32)message.wParam;
                bool32 wasDown = ((message.lParam & (1 << 30)) != 0);
                bool32 isDown = ((message.lParam & (1 << 31)) == 0);

                if (isDown != wasDown)
                {
                    switch (vkCode)
                    {
                        case VK_ESCAPE:
                        {
                            g_running = false;
                        } break;

                        case VK_LEFT:
                        case 'A':
                        {
                            inputState->moveLeft = isDown;
                        } break;

                        case VK_RIGHT:
                        case 'D':
                        {
                            inputState->moveRight = isDown;
                        } break;

                        case VK_UP:
                        case 'W':
                        {
                            inputState->moveUp = isDown;
                        } break;

                        case VK_DOWN:
                        case 'S':
                        {
                            inputState->moveDown = isDown;
                        } break;
                    }
                }
            } break;

            default:
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
    }
}

internal void win32_ProcessXBoxControllers(InputState* inputState)
{

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

    if (RegisterClass(&wc))
    {
        HWND window = CreateWindowExA(0,
                                      wc.lpszClassName,
                                      "Relwrab",
                                      WS_VISIBLE | WS_POPUP,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      1280,
                                      720,
                                      0,
                                      0,
                                      instance,
                                      0);

        HDC openglDC = GetDC(window);
        HGLRC openglRC = win32_InitOpenGL(openglDC);

        wglMakeCurrent(openglDC, openglRC);

        g_running = true;

        GameState gameState = {};
        InputState inputState = {};

        while (g_running)
        {
            win32_ProcessInputMessages(&inputState);
            win32_ProcessXBoxControllers(&inputState);

            if (inputState.moveLeft)   gameState.triX -= 0.01f;
            if (inputState.moveRight)  gameState.triX += 0.01f;
            if (inputState.moveDown)   gameState.triY -= 0.01f;
            if (inputState.moveUp)     gameState.triY += 0.01f;

            if (gameState.triX < -1.f) gameState.triX =  1.f;
            if (gameState.triX >  1.f) gameState.triX = -1.f;
            if (gameState.triY < -1.f) gameState.triY =  1.f;
            if (gameState.triY >  1.f) gameState.triY = -1.f;

            Render(&gameState);
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
        case WM_SIZE:
        {
            glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
            PostMessage(hwnd, WM_PAINT, 0, 0);
        } break;

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
            OutputDebugString("Missed a keyboard input");
            int* ptr = 0;
            *ptr = 42;
        } break;

        default:
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}
