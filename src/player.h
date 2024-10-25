#pragma once
#include "raylib.h"

#define PLAYER_SIZE 64
#define PLAYER_BASE_SPEED 5.0f

//----------------------------------------------------------------------------------
// Player Definition
//----------------------------------------------------------------------------------

typedef struct Player
{
    Vector2 pos;
    Vector2 center;
    float speed;
    Color color;
    bool isMoving; // currently unused
} PLAYER;

extern PLAYER player;

// Player Functions
//----------------------------------------------------------------------------------
void UpdatePlayer(PLAYER *p);
void DrawPlayer(PLAYER *p);