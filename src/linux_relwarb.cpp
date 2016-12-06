
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <stdio.h>

#include "relwarb_defines.h"
#include "relwarb_opengl.h"
#include "relwarb.h"

global_variable bool32 g_running;

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
            gameState->renderWidth = xwa.width;
            gameState->renderHeight = xwa.height;
        } break;

        case KeyPress:
        {
            KeySym key = XLookupKeysym(&event->xkey, 0);
            if (key == XK_z)
            {
                gameState->controller.moveUp = true;
            }
            else if (key == XK_s)
            {
                gameState->controller.moveDown = true;
            }
            else if (key == XK_q)
            {
                gameState->controller.moveLeft = true;
            }
            else if (key == XK_d)
            {
                gameState->controller.moveRight = true;
            }
        } break;

        case KeyRelease:
        {
            KeySym key = XLookupKeysym(&event->xkey, 0);
            if (key == XK_z)
            {
                gameState->controller.moveUp = false;
            }
            else if (key == XK_s)
            {
                gameState->controller.moveDown = false;
            }
            else if (key == XK_q)
            {
                gameState->controller.moveLeft = false;
            }
            else if (key == XK_d)
            {
                gameState->controller.moveRight = false;
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

int main()
{
    // https://www.opengl.org/wiki/Programming_OpenGL_in_Linux:_GLX_and_Xlib
    // TODO(Charly): https://www.opengl.org/wiki/Tutorial:_OpenGL_3.0_Context_Creation_(GLX)
    Display* display;
    Window rootWindow;
    GLint attribs[] = {
        GLX_RGBA,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        GLX_SAMPLE_BUFFERS, 1,
        GLX_SAMPLES, 4,
        None
    };
    XVisualInfo* infos;
    Colormap colormap;
    XSetWindowAttributes setAttribs;
    Window window;
    GLXContext glxContext;
    XEvent event;

    // Create the display
    display = XOpenDisplay(nullptr);

    if (display)
    {
        int major, minor;
        if (glXQueryVersion(display, &major, &minor))
        {
            fprintf(stderr, "Best context found : %d.%d\n", major, minor);
            rootWindow = DefaultRootWindow(display);
        
            // Get context creation informations
            infos = glXChooseVisual(display, 0, attribs);
        
            if (infos)
            {
                printf("Visual %p selected\n", (void*)infos->visualid);
                // Create colormap and set attributes for window creation
                colormap = XCreateColormap(display, rootWindow, infos->visual, AllocNone);
                setAttribs.colormap = colormap;
                setAttribs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | FocusChangeMask;
            
                // Create the window
                window = XCreateWindow(display,
                                        rootWindow,
                                        0, 0,
                                        800, 600,
                                        0,
                                        infos->depth,
                                        InputOutput,
                                        infos->visual,
                                        CWColormap | CWEventMask,
                                        &setAttribs);
                                    
                // Show the window
                XMapWindow(display, window);
            
                // Set the window title
                XStoreName(display, window, "Relwarb");
            
                // Create context and make it current
                glxContext = glXCreateContext(display, infos, nullptr, GL_TRUE);
                glXMakeCurrent(display, window, glxContext);

                fprintf(stderr, "%s\n", glGetString(GL_VERSION));

                glGenerateMipmap = (def_glGenerateMipmap*)glXGetProcAddress((const GLubyte*)"glGenerateMipmap");
                Assert(glGenerateMipmap);

                GameState gameState = {};
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
                glXDestroyContext(display, glxContext);
                XDestroyWindow(display, window);
                XCloseDisplay(display);
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
