#include "player.h"

Vector2 playerSize = (Vector2){(float)PLAYER_SIZE, (float)PLAYER_SIZE};

PLAYER player = {
    .pos = (Vector2){0.0f, 0.0f},
    .center = (Vector2){(float)(PLAYER_SIZE / 2), (float)(PLAYER_SIZE / 2)},
    .speed = PLAYER_BASE_SPEED,
    .color = BLACK,
    .isMoving = false,
};

void DrawPlayer(PLAYER *p)
{
    DrawRectangleV(p->pos, playerSize, p->color);
}

void UpdatePlayer(PLAYER *p)
{
    // Move Player
    if (IsKeyDown(KEY_W))
        p->pos.y -= p->speed;
    if (IsKeyDown(KEY_S))
        p->pos.y += p->speed;
    if (IsKeyDown(KEY_A))
        p->pos.x -= p->speed;
    if (IsKeyDown(KEY_D))
        p->pos.x += p->speed;
}