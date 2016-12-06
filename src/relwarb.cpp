#include "relwarb.h"
#include "relwarb_opengl.h"
#include "relwarb_math.h"

// TODO(Charly): This should go somewhere else.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void InitGame(GameState* gameState)
{
	int halfWidth = gameState->renderWidth * 0.5;
	int halfHeight = gameState->renderHeight * 0.5;

	gameState->nbEntities = 7;
	gameState->entities[0] = Entity(RectangularShape(- halfWidth - 1, halfHeight, 2, gameState->renderHeight + 2), Bitmap());
	gameState->entities[1] = Entity(RectangularShape(  halfWidth + 1, halfHeight, 2, gameState->renderHeight + 2), Bitmap());
	gameState->entities[2] = Entity(RectangularShape(0, - 1, gameState->renderWidth, 2), Bitmap());
	gameState->entities[3] = Entity(RectangularShape(0, gameState->renderHeight + 1, gameState->renderWidth, 2), Bitmap());

	gameState->entities[4] = Entity(RectangularShape(- halfWidth * 0.5, halfHeight * 0.5, halfWidth * 0.25, 32), Bitmap());
	gameState->entities[5] = Entity(RectangularShape(  halfWidth * 0.5, halfHeight * 0.5, halfWidth * 0.25, 32), Bitmap());
	gameState->entities[6] = Entity(RectangularShape(0, halfHeight, halfWidth * 0.25, 32), Bitmap());

    LoadImage("assets/smiley.png", &gameState->entities[0].bitmap);
}

void UpdateGame(GameState* gameState)
{
    // TODO(Charly): How do we retrieve actual characters ?
    gameState->onEdge = false;
    // if (gameState->controller.moveLeft)   gameState->character.posX -= 0.02f;
    // if (gameState->controller.moveRight)  gameState->character.posX += 0.02f;
    // if (gameState->controller.moveDown)   gameState->character.posY -= 0.02f;
    // if (gameState->controller.moveUp)     gameState->character.posY += 0.02f;
    // if (gameState->entities[0].shape.posX < -1.f)
    // {
    //    gameState->entities[0].shape.posX = -1.f;
    //    gameState->onEdge = true;
    // }
    // if (gameState->entities[0].shape.posX > 1.f - gameState->entities[0].shape.sizeX)
    // {
        // gameState->entities[0].shape.posX = 1.f - gameState->entities[0].shape.sizeX;
        // gameState->onEdge = true;
    // }
    // if (gameState->entities[0].shape.posY < -1.f)
    // {
        // gameState->entities[0].shape.posY = -1.f;
        // gameState->onEdge = true;
    // }
    // if (gameState->entities[0].shape.posY > 1.f - gameState->entities[0].shape.sizeY)
    // {
        // gameState->entities[0].shape.posY = 1.f - gameState->entities[0].shape.sizeY;
        // gameState->onEdge = true;
    // }
}

void RenderGame(GameState* gameState)
{
    glViewport(0, 0, gameState->renderWidth, gameState->renderHeight);

    glClearColor(0.3f, 0.8f, 0.7f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (uint32 elementIdx = 0; elementIdx < 1; ++elementIdx)
    {
        Entity* element = &gameState->entities[elementIdx];
        Bitmap* bitmap = &element->bitmap;
        RenderBitmap(&element->bitmap, 0, 0);
    }
}

void LoadImage(const char* filename, Bitmap* bitmap)
{
    // NOTE(Charly): Get images bottom-up
    stbi_set_flip_vertically_on_load(true);
    int n;
    bitmap->data = stbi_load(filename, &bitmap->width, &bitmap->height, &n, 4);
    Assert(bitmap->data);
}

void ReleaseImage(Bitmap* bitmap)
{
    ReleaseBitmap(bitmap);
    stbi_image_free(bitmap->data);
}
