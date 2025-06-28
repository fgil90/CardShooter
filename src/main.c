
//----------------------------------------------------------------------------------
// TODO:
// - [x] Camera
// - [x] Enemies
// - [x] Different Types of cards
// - [ ] Cycle card types with Q and E
// - [ ] Throw curved cards

#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include "card.h"
#include "enemy.h"
#include <stdbool.h>
#include <stdio.h>

#define SCREEN_WIDTH 1280  
#define SCREEN_HEIGHT 720
#define DRAW_FPS 60
#define LOGIC_FPS 60
#define LOGIC_FRAME_TIME 1.0/(float)LOGIC_FPS
#define DEFAULT_BG_COLOR WHITE

#define SCREEN_CENTER {(float)SCREEN_WIDTH/2, (float)SCREEN_HEIGHT/2}

#define MAX_DIFFICULTY 5

#define SQRT2 1.41421356237

#define DMG_UI_DURATION 2

//----------------------------------------------------------------------------------
// Variables Definition
//----------------------------------------------------------------------------------

float logicAcc = 0.0f;

CARD_HANDLER cardHandler = {
    .initialMousePos = (Vector2){0.0f, 0.0f},
    .shotTimeAccumulator = 0.0f,
    .currentCardIdx = 0,
    .isShooting = false};


ENEMY_HANDLER enemyHandler = {
    .currentIndex = 0,
    .spawn_timer = 0.0f,
    .variance = 0.15f,
    .curr_spawn_time = 3.0f,
    .avg_spawn_interval = 3.0f,
    .difficulty = 1
};

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------

void CheckCardEnemyCollisions(CARD_HANDLER *ch, ENEMY_HANDLER *eh);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------

int main(){
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Zeldinha de Carta");
    
    // Camera
    Camera2D camera = {
        .offset = SCREEN_CENTER,
        .target = (Vector2){player.pos.x + PLAYER_SIZE / 2, player.pos.y + PLAYER_SIZE / 2},
        .rotation = 0.0f,
        .zoom = 1.0f
        };
    //--------------------------------------------------------------------------------------

    SetTargetFPS(DRAW_FPS); // Set our game maximum FPS
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Logic
        //--------------------------------------------------------------------------------------
        logicAcc += GetFrameTime();

        if(logicAcc>LOGIC_FRAME_TIME){
            UpdatePlayer();
            UpdateCards(&cardHandler);
            UpdateEnemies(&enemyHandler);
            camera.target = Vector2Add(player.pos, (Vector2){PLAYER_SIZE/2, PLAYER_SIZE/2});
            logicAcc -= LOGIC_FRAME_TIME;

        }
        ManageEnemySpawns(&enemyHandler);
        HandleCardThrow(&cardHandler, Vector2Add(player.pos, player.center));
        CheckCardEnemyCollisions(&cardHandler, &enemyHandler);

        // Draw
        //--------------------------------------------------------------------------------------
        BeginDrawing();
        BeginMode2D(camera);

            ClearBackground(DEFAULT_BG_COLOR);
            DrawCards(&cardHandler);
            DrawPlayer();
            DrawEnemies(&enemyHandler);
            DrawEnemyDamage(&enemyHandler);
        EndMode2D();

        // UI
        //--------------------------------------------------------------------------------------
        DrawFPS(20,20);

        //--------------------------------------------------------------------------------------
        EndDrawing();
    }
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}



void CheckCardEnemyCollisions(CARD_HANDLER *ch, ENEMY_HANDLER *eh)
{
    for (int i = 0; i < MAX_CARDS; i++){
        CARD *c = &ch->cardArray[i];
        if (!c->isMoving || !c->isAlive) continue;

        for (int j = 0; j < MAX_ENEMIES; j++)
        {
            ENEMY *e = &eh->enemyArray[j];
            if (!e->isAlive) continue;

            Rectangle enemy_rect = {e->pos.x, e->pos.y, e->size.x, e->size.y};
            Vector2 cardCenter = Vector2Add(c->center, c->pos);
            bool isColliding = CheckCollisionCircleRec(cardCenter, c->size.x/2*SQRT2, enemy_rect); // TODO: fix wrong radius
            if (!isColliding) continue;

            c->isAlive = false;
                if (e->iFrameTimer>0){
                    e->iFrameTimer -= GetFrameTime();
                    e->dmgUITimer  -= GetFrameTime();
                    continue;
            }

            int damage = (int)(Vector2Length(c->speed));
            e->hp -= damage;
            e->lastDmg = damage;
            e->dmgUITimer = DMG_UI_DURATION;
            return; //TODO: Add piercing / Add Enemy iframes
        }
    }
}

