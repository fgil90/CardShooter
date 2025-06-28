#include "enemy.h"
#include "player.h"
#include <stdio.h>

void DrawEnemies(ENEMY_HANDLER *eh)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        ENEMY *e = &eh->enemyArray[i];
        if (e->isAlive)
            DrawRectangleV(e->pos, e->size, e->color);
    }
}

void DrawEnemyDamage(ENEMY_HANDLER *eh)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        ENEMY *e = &eh->enemyArray[i];
        if (!e->isAlive)
            continue;
        if (e->dmgUITimer > 0)
        {

            int max_dmg = 999999;
            e->lastDmg = (e->lastDmg > max_dmg) ? max_dmg : e->lastDmg;

            Vector2 offset = {0.0f, -30.0f};
            Vector2 dmg_pos = Vector2Add(e->pos, offset);
            DrawText(TextFormat("%d", e->lastDmg), (int)dmg_pos.x, (int)dmg_pos.y, 20, RED);

            e->dmgUITimer -= GetFrameTime();
        }
    }
}

void MoveEnemy(ENEMY *e, Vector2 target)
{
    Vector2 enemy_center = Vector2Add(e->pos, e->center);
    Vector2 direction = Vector2Subtract(target, enemy_center);
    e->curr_speed = Vector2Scale(Vector2Normalize(direction), e->speed);
    e->pos = Vector2Add(e->pos, e->curr_speed);
}

void UpdateEnemies(ENEMY_HANDLER *eh)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        ENEMY *enemy = &eh->enemyArray[i];
        if (!enemy->isAlive)
            continue;
        if (enemy->hp < 0)
        {
            enemy->isAlive = false;
            continue;
        }
        MoveEnemy(enemy, Vector2Add(player.pos, player.center));
    }
}

void SpawnEnemy(ENEMY_HANDLER *eh)
{
    eh->enemyArray[eh->currentIndex] = (ENEMY){
        .pos = Vector2Zero(),
        .size = {30.0f, 30.0f},
        .center = {15.0f, 15.0f},
        .speed = GetRandomValue(1, eh->difficulty * 2),
        .hp = 100,
        .color = DARKPURPLE,
        .isAlive = true,
        .iFrameTimer = 0,
        .dmgUITimer = 0,
    };
    eh->currentIndex++;
    eh->currentIndex %= MAX_ENEMIES;
}

void ManageEnemySpawns(ENEMY_HANDLER *eh) // BAD NAME
{
    eh->spawn_timer += GetFrameTime();

    if (eh->spawn_timer > eh->curr_spawn_time)
    {
        SpawnEnemy(eh);

        eh->spawn_timer -= eh->curr_spawn_time;
        eh->curr_spawn_time = eh->avg_spawn_interval + (float)GetRandomValue(-100, 100) / 100.0f * eh->variance;
    }
}