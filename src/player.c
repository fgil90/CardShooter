#include "player.h"

Vector2 playerSize = (Vector2){(float)PLAYER_SIZE, (float)PLAYER_SIZE};

PLAYER player = {
    .pos = (Vector2){0.0f, 0.0f},
    .center = (Vector2){(float)(PLAYER_SIZE / 2), (float)(PLAYER_SIZE / 2)},
    .speed = PLAYER_BASE_SPEED,
    .color = BLACK,
    .isMoving = false,
};

void DrawPlayer()
{
    DrawRectangleV(player.pos, playerSize, player.color);
}

void UpdatePlayer()
{
    // Move Player
    if (IsKeyDown(KEY_W))
        player.pos.y -= player.speed;
    if (IsKeyDown(KEY_S))
        player.pos.y += player.speed;
    if (IsKeyDown(KEY_A))
        player.pos.x -= player.speed;
    if (IsKeyDown(KEY_D))
        player.pos.x += player.speed;
}