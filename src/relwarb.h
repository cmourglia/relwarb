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

// NOTE(Charly): Store the current state of the game
struct GameState
{
    Controller controller;

    uint32 renderWidth;
    uint32 renderHeight;

    real32 triX;
    real32 triY;
    real32 triSize;

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
