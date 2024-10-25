#include "raylib.h"
#include "raymath.h"

#define MAX_ENEMIES 100

typedef struct Enemy
{
    Vector2 size;
    Color color;
    Vector2 pos;
    Vector2 center;
    Vector2 curr_speed;
    float speed;
    bool isAlive;
    int hp;
    int damage;
    float iFrameTimer;
    int lastDmg;
    float dmgUITimer;

} ENEMY;

typedef struct EnemyHandler
{
    ENEMY enemyArray[MAX_ENEMIES];
    int currentIndex;
    float spawn_timer;
    float variance;
    float curr_spawn_time;
    float avg_spawn_interval;
    int difficulty;
} ENEMY_HANDLER;

// Enemy Functions
//----------------------------------------------------------------------------------
void MoveEnemy(ENEMY *e, Vector2 target);
void UpdateEnemies(ENEMY_HANDLER *eh);
void DrawEnemies(ENEMY_HANDLER *eh);
void DrawEnemyDamage(ENEMY_HANDLER *eh);
void SpawnEnemy(ENEMY_HANDLER *eh);
void SpawnEnemies(ENEMY_HANDLER *eh);