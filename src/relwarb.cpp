#include "relwarb.h"
#include "relwarb_opengl.h"

void InitGame(GameState* gameState)
{
    gameState->character.sizeX = 0.1f;
    gameState->character.sizeY = 0.1f;

	uint32 halfWidth = gameState->renderWidth * 0.5;
	uint32 halfHeight = gameState->renderHeight * 0.5;

	gameState->nbElements = 7;
	gameState->elements[0] = WorldElement(RectangularShape(- halfWidth - 1, halfHeight, 2, gameState->renderHeight + 2), Bitmap());
	gameState->elements[1] = WorldElement(RectangularShape(  halfWidth + 1, halfHeight, 2, gameState->renderHeight + 2), Bitmap());
	gameState->elements[2] = WorldElement(RectangularShape(0, - 1, gameState->renderWidth, 2), Bitmap());
	gameState->elements[3] = WorldElement(RectangularShape(0, gameState->renderHeight + 1, gameState->renderWidth, 2), Bitmap());

	gameState->elements[4] = WorldElement(RectangularShape(- halfWidth * 0.5, halfHeight * 0.5, halfWidth * 0.25, 32), Bitmap());
	gameState->elements[5] = WorldElement(RectangularShape(  halfWidth * 0.5, halfHeight * 0.5, halfWidth * 0.25, 32), Bitmap());
	gameState->elements[6] = WorldElement(RectangularShape(0, halfHeight, halfWidth * 0.25, 32), Bitmap());
}

void UpdateGame(GameState* gameState)
{
    gameState->onEdge = false;
    if (gameState->controller.moveLeft)   gameState->character.posX -= 0.02f;
    if (gameState->controller.moveRight)  gameState->character.posX += 0.02f;
    if (gameState->controller.moveDown)   gameState->character.posY -= 0.02f;
    if (gameState->controller.moveUp)     gameState->character.posY += 0.02f;

    if (gameState->character.posX < -1.f)
    {
        gameState->character.posX = -1.f;
        gameState->onEdge = true;
    }

    if (gameState->character.posX > 1.f - gameState->character.sizeX)
    {
        gameState->character.posX = 1.f - gameState->character.sizeX;
        gameState->onEdge = true;
    }
    if (gameState->character.posY < -1.f)
    {
        gameState->character.posY = -1.f;
        gameState->onEdge = true;
    }
    if (gameState->character.posY > 1.f - gameState->character.sizeY)
    {
        gameState->character.posY = 1.f - gameState->character.sizeY;
        gameState->onEdge = true;
    }
}

void RenderGame(GameState* gameState)
{
    glViewport(0, 0, gameState->renderWidth, gameState->renderHeight);

    glClearColor(0.3f, 0.8f, 0.7f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);

    glColor3f(1, 0, 0); glVertex2f(gameState->character.posX, gameState->character.posY);
    glColor3f(0, 1, 0); glVertex2f(gameState->character.posX + gameState->character.sizeX, gameState->character.posY);
    glColor3f(0, 0, 1); glVertex2f(gameState->character.posX + gameState->character.sizeX / 2, gameState->character.posY + gameState->character.sizeX);

    glEnd();

    glFlush();
}
