#ifndef RELWARB_H
#define RELWARB_H

#include "relwarb_defines.h"

// NOTE(Charly): This is garbage code
struct Controller
{
    bool32 moveUp;
    bool32 moveDown;
    bool32 moveLeft;
    bool32 moveRight;
};

// TODO(Charly): This should go somewhere else
struct Bitmap
{
    uint32 texture = 0;
    uint8* data;
};

// TODO(Charly): More "generic" stuff ? Notion of entity, etc
struct Character
{
    // TODO(Charly): vec2
    // TODO(Charly): Decide where is the (0, 0) of the character
    real32 posX;
    real32 posY;

    real32 sizeX;
    real32 sizeY;

    // TODO(Charly): velocity
    // TODO(Charly): acceleration

    Bitmap bitmap;
};

// NOTE(Charly): Store the current state of the game
struct GameState
{
    Controller controller;
    Character character;

    uint32 renderWidth;
    uint32 renderHeight;

    bool32 onEdge;
};

// NOTE(Charly): Initialize all the game logic related stuff here
// TODO(Charly): This should probably be exposed to the scripting
void InitGame(GameState* gameState);

// NOTE(Charly): Do all game logic and computations here
// TODO(Charly): This should probably be exposed to the scripting
void UpdateGame(GameState* gameState);

// NOTE(Charly): Render the current state of the game
// TODO(Charly): Maybe we need to pass the delta time for some
//               time dependent effects ?
void RenderGame(GameState* gameState);
#endif // RELWARB_H
