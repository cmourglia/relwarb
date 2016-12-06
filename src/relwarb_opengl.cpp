#include "relwarb_opengl.h"

#define FUNC_DEF(name) def_##name* name

FUNC_DEF(glGetStringi);
FUNC_DEF(glUseProgram);
FUNC_DEF(glGetIntegerv);
FUNC_DEF(glViewport);
FUNC_DEF(glClearColor);
FUNC_DEF(glClear);
FUNC_DEF(glFlush);
FUNC_DEF(glDebugMessageCallbackARB);
FUNC_DEF(glBegin);
FUNC_DEF(glColor3f);
FUNC_DEF(glVertex2f);
FUNC_DEF(glEnd);
