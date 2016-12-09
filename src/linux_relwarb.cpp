
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <stdio.h>

#include "relwarb_defines.h"
#include "relwarb_opengl.h"
#include "relwarb_utils.h"
#include "relwarb.h"

global_variable bool32 g_running;

// https://www.opengl.org/wiki/Tutorial:_OpenGL_3.0_Context_Creation_(GLX)
// https://www.opengl.org/wiki/Programming_OpenGL_in_Linux:_GLX_and_Xlib
#define GLX_CONTEXT_MAJOR_VERSION_ARB               0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB               0x2092
#define GLX_CONTEXT_FLAGS_ARB                       0x2094
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB            0x00000001
#define GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB   0x00000002
typedef GLXContext (*glxCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

global_variable bool32 g_ctxErrorOccured;
internal int linux_CtxErrorHandler(Display* dpy, XErrorEvent* ev)
{
    g_ctxErrorOccured = true;
    return 0;
}

internal bool32 linux_IsExtensionSupported(const char* extList, const char* ext)
{
    bool32 result = false;
    uint32 extLen = StrLength(ext);
    if (extLen != 0)
    {
        const char* start;
        const char* end;

        for (start = extList;;)
        {
            end = start;

            for (;;)
            {
                if (*end == '\0' || *end == ' ')
                {
                    break;
                }

                ++end;
            }

            if (StrEqual(start, ext, extLen))
            {
                result = true;
                break;
            }

            start = end;
            if (*start == '\0')
            {
                break;
            }

            ++start;
        }
    }

    return result;
}

internal Bool linux_CheckEvent(Display* display, XEvent* event, XPointer userData)
{
    Bool result = event->xany.window == *(Window*)userData;
    return result;
}

// Input processing is greatly inspired by SDL / SFML code
internal bool32 linux_ProcessEvent(Display* display, XEvent* event, GameState* gameState)
{
    // Detect and remove key repeat events
    if (event->xkey.keycode == KeyRelease)
    {
        XEvent nextEvent;
        XPeekEvent(display, &nextEvent);

        if (nextEvent.type == KeyPress)
        {
            // Same keycode and times are close enough
            if (nextEvent.xkey.keycode == event->xkey.keycode &&
                (nextEvent.xkey.time - event->xkey.time < 2))
            {
                // Ignore key repeat
                fprintf(stderr, "quit\n");
                XNextEvent(display, &nextEvent);
                return false;
            }
        }
    }

    switch (event->type)
    {
        // TODO(Charly): Properly handle window destruction (ClientMessage)
        case DestroyNotify:
        {
            g_running = false;
        } break;

        // TODO(Charly): Focus handling ?
        case FocusIn:
        case FocusOut:
        case EnterNotify:
        case LeaveNotify:
        {
        } break;

        // TODO(Charly): ConfigureNotify might trigger resize event
        //               Do we want them ?
        case ConfigureNotify:
        {
        } break;

        case Expose:
        case ResizeRequest:
        {
            XWindowAttributes xwa;
            XGetWindowAttributes(display, event->xkey.window, &xwa);
            gameState->viewportSize = Vec2(xwa.width, xwa.height);
        } break;

        case KeyPress:
        {
            KeySym key = XLookupKeysym(&event->xkey, 0);
            if (key == XK_z)
            {
                gameState->controller[0].moveUp = true;
            }
            else if (key == XK_s)
            {
                gameState->controller[0].moveDown = true;
            }
            else if (key == XK_q)
            {
                gameState->controller[0].moveLeft = true;
            }
            else if (key == XK_d)
            {
                gameState->controller[0].moveRight = true;
            }
        } break;

        case KeyRelease:
        {
            KeySym key = XLookupKeysym(&event->xkey, 0);
            if (key == XK_z)
            {
                gameState->controller[0].moveUp = false;
            }
            else if (key == XK_s)
            {
                gameState->controller[0].moveDown = false;
            }
            else if (key == XK_q)
            {
                gameState->controller[0].moveLeft = false;
            }
            else if (key == XK_d)
            {
                gameState->controller[0].moveRight = false;
            }
            else if (key == XK_Escape)
            {
                g_running = false;
            }
        } break;
    }

    return true;
}

void linux_ProcessInputMessages(Display* display, Window* window, GameState* gameState)
{
    XEvent event;
    while (XCheckIfEvent(display, &event, &linux_CheckEvent, (XPointer)window))
    {
        linux_ProcessEvent(display, &event, gameState);
    }
}

internal void linux_GLDebugOutput(GLenum source,
                                  GLenum type,
                                  GLuint id,
                                  GLenum severity,
                                  GLsizei length,
                                  const char* message,
                                  const void* userParam)
{
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR_ARB:
        {
            fprintf(stderr, "OpenGL error ");
        } break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
        {
            fprintf(stderr, "OpenGL deprecated behaviour ");
        } break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
        {
            fprintf(stderr, "OpenGL undefined behaviour ");
        } break;

        case GL_DEBUG_TYPE_PORTABILITY_ARB:
        {
            fprintf(stderr, "OpenGL portability issue ");
        } break;

        case GL_DEBUG_TYPE_PERFORMANCE_ARB:
        {
            fprintf(stderr, "OpenGL performance issue ");
        } break;

        case GL_DEBUG_TYPE_OTHER_ARB:
        default:
        {
            fprintf(stderr, "OpenGL other issue ");
        }
    }

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH_ARB:
        {
            fprintf(stderr, "(HIGH): ");
        } break;

        case GL_DEBUG_SEVERITY_MEDIUM_ARB:
        {
            fprintf(stderr, "(MEDIUM): ");
        } break;

        case GL_DEBUG_SEVERITY_LOW_ARB:
        {
            fprintf(stderr, "(LOW): ");
        } break;

        default: {}
    }

    fprintf(stderr, "%s", message);
    fprintf(stderr, "\n");
}

int main()
{
    Display* display = XOpenDisplay(nullptr);

    if (display)
    {
        Window rootWindow;

        GLint visualAttribs[] = {
            GLX_X_RENDERABLE, True,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_RENDER_TYPE, GLX_RGBA_BIT,
            GLX_RED_SIZE, 8,
            GLX_GREEN_SIZE, 8,
            GLX_BLUE_SIZE, 8,
            GLX_ALPHA_SIZE, 8,
            GLX_DEPTH_SIZE, 24,
            GLX_STENCIL_SIZE, 8,
            GLX_DOUBLEBUFFER, True,
            GLX_SAMPLE_BUFFERS, 1,
            GLX_SAMPLES, 4,
            None,
        };
        
        int major, minor;
        if (glXQueryVersion(display, &major, &minor) && (major >= 1 && minor >= 3))
        {
            int fbCount;
            GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visualAttribs, &fbCount);

            if (fbc)
            {
                int bestFbcIdx = -1, worstFbcIdx = -1, bestNumSamp = -1, worstNumSamp = 999;
                for (int fb = 0; fb < fbCount; ++fb)
                {
                    XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbc[fb]);
                    if (vi)
                    {
                        int sampBuf, samples;
                        glXGetFBConfigAttrib(display, fbc[fb], GLX_SAMPLE_BUFFERS, &sampBuf);
                        glXGetFBConfigAttrib(display, fbc[fb], GLX_SAMPLES, &samples);

                        if (bestFbcIdx < 0 || (sampBuf && samples > bestNumSamp))
                        {
                            bestFbcIdx = fb;
                            bestNumSamp = samples;
                        }

                        if (worstFbcIdx < 0 || !sampBuf || samples < worstNumSamp)
                        {
                            worstFbcIdx = fb;
                            worstNumSamp = samples;
                        }
                    }

                    XFree(vi);
                }

                Assert(bestFbcIdx >= 0);
                GLXFBConfig bestFbc = fbc[bestFbcIdx];
                XFree(fbc);

                XVisualInfo* vi = glXGetVisualFromFBConfig(display, bestFbc);
                XSetWindowAttributes swa;
                Colormap cmap;
                swa.colormap = cmap = XCreateColormap(display, RootWindow(display, vi->screen), vi->visual, AllocNone);
                swa.background_pixmap = None;
                swa.border_pixel = 0;
                swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | FocusChangeMask;

                // Create the window
                Window window = XCreateWindow(display,
                                              RootWindow(display, vi->screen),
                                              0, 0,
                                              800, 600,
                                              0,
                                              vi->depth,
                                              InputOutput,
                                              vi->visual,
                                              CWColormap | CWEventMask | CWBorderPixel,
                                              &swa);
                                    
                if (window)
                {
                    XFree(vi);

                    XStoreName(display, window, "Relwarb");                   
                    // Show the window
                    XMapWindow(display, window);
            
                    const char* glxExts = glXQueryExtensionsString(display, DefaultScreen(display));

                    glxCreateContextAttribsARBProc glXCreateContextAttribsARB = nullptr;
                    glXCreateContextAttribsARB = (glxCreateContextAttribsARBProc)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
                    Assert(glXCreateContextAttribsARB);

                    GLXContext context = nullptr;
                    
                    g_ctxErrorOccured = false;
                    int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(linux_CtxErrorHandler);

                    if (!linux_IsExtensionSupported(glxExts, "GLX_ARB_create_context"))
                    {
                        fprintf(stderr, "Cannot create GL3.3 context, creating an old style context.\n");
                        context = glXCreateNewContext(display, bestFbc, GLX_RGBA_TYPE, 0, True);
                    }
                    else
                    {
                        GLint contextAttribs[] = {
                            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                            GLX_CONTEXT_MINOR_VERSION_ARB, 3,
                            GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                            None,
                        };

                        context = glXCreateContextAttribsARB(display, bestFbc, 0, True, contextAttribs);

                        // NOTE(Charly): Process all eventual errors
                        XSync(display, False);

                        if (!g_ctxErrorOccured && context)
                        {
                            contextAttribs[1] = 1;
                            contextAttribs[3] = 0;
                            contextAttribs[5] = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;

                            g_ctxErrorOccured = false;
                            context = glXCreateContextAttribsARB(display, bestFbc, 0, True, contextAttribs);
                        }
                    }

                    XSync(display, False);
                    XSetErrorHandler(oldHandler);

                    if (!g_ctxErrorOccured && context)
                    {
                        glDebugMessageCallbackARB = (def_glDebugMessageCallbackARB*)glXGetProcAddress((const GLubyte*)"glDebugMessageCallbackARB");
                        Assert(glDebugMessageCallbackARB);
                        glDebugMessageCallbackARB(linux_GLDebugOutput, nullptr);
                    
                        GameState gameState = {};
                        gameState.viewportSize = Vec2(800, 600);
                        g_running = true;
                    
                        InitGame(&gameState);
                    
                        // Game loop
                        while (g_running)
                        {
                            linux_ProcessInputMessages(display, &window, &gameState);
                            UpdateGame(&gameState);
                            RenderGame(&gameState);
                            glXSwapBuffers(display, window);
                        }
                    
                        glXMakeCurrent(display, None, nullptr);
                        glXDestroyContext(display, context);
                        XDestroyWindow(display, window);
                        XCloseDisplay(display);
                    }
                }
            }
            else
            {
                fprintf(stderr, "Could not find an appropriate visual\n");
            }
        }
        else
        {
            fprintf(stderr, "Could not find an appropriate context\n");
        }
    }   
    else
    {
        // TODO(Charly): Logging system
        fprintf(stderr, "Could not connect to X server\n");
    }
    // XFlush(display);

    return 0;
}
