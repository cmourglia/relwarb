#include "relwarb.h"
#include "relwarb_opengl.h"

void InitGame(GameState* gameState)
{
    gameState->triSize = 0.1f;
}

void UpdateGame(GameState* gameState)
{
    gameState->onEdge = false;
    if (gameState->controller.moveLeft)   gameState->triX -= 0.02f;
    if (gameState->controller.moveRight)  gameState->triX += 0.02f;
    if (gameState->controller.moveDown)   gameState->triY -= 0.02f;
    if (gameState->controller.moveUp)     gameState->triY += 0.02f;

    if (gameState->triX < -1.f)
    {
        gameState->triX = -1.f;
        gameState->onEdge = true;
    }

    if (gameState->triX > 1.f - gameState->triSize)
    {
        gameState->triX = 1.f - gameState->triSize;
        gameState->onEdge = true;
    }
    if (gameState->triY < -1.f)
    {
        gameState->triY = -1.f;
        gameState->onEdge = true;
    }
    if (gameState->triY > 1.f - gameState->triSize)
    {
        gameState->triY = 1.f - gameState->triSize;
        gameState->onEdge = true;
    }
}

void RenderGame(GameState* gameState)
{
    glViewport(0, 0, gameState->renderWidth, gameState->renderHeight);

    glClearColor(0.3f, 0.8f, 0.7f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);

    glColor3f(1, 0, 0); glVertex2f(gameState->triX, gameState->triY);
    glColor3f(0, 1, 0); glVertex2f(gameState->triX + gameState->triSize, gameState->triY);
    glColor3f(0, 0, 1); glVertex2f(gameState->triX + gameState->triSize / 2, gameState->triY + gameState->triSize);

    glEnd();

    glFlush();
}
