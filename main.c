
//----------------------------------------------------------------------------------
// TODO:
// - [x] Camera
// - [x] Enemies
// - [x] Different Types of cards
// - [ ] Cycle card types with Q and E
// - [ ] Throw curved cards

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define DRAW_FPS 120
#define LOGIC_FPS 60
#define LOGIC_FRAME_TIME 1.0/(float)LOGIC_FPS

#define ZERO_VECTOR {0.0f, 0.0f}
#define SCREEN_CENTER {(float)SCREEN_WIDTH/2, (float)SCREEN_HEIGHT/2}

#define MAX_CARDS 100
#define MAX_ENEMIES 100
#define MAX_DIFFICULTY 5

#define PLAYER_SIZE 64
#define CARD_SIZE 16
#define CARD_SPEED_MULT 1/25.0f
#define DMG_MULT 1
#define SQRT2 1.41421356237

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Variables Definition
//----------------------------------------------------------------------------------

Color bgColor = WHITE;
Vector2 playerSize = {(float)PLAYER_SIZE, (float)PLAYER_SIZE};
float logicAcc = 0.0f;

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

PLAYER player = {
    .pos = ZERO_VECTOR,
    .center = (Vector2){(float)(PLAYER_SIZE/2), (float)(PLAYER_SIZE/2)},
    .speed = 5.0f,
    .color = BLACK,
    .isMoving = false,
};

//----------------------------------------------------------------------------------
// Card Definition
//----------------------------------------------------------------------------------

enum cardTypes{
    NORMAL,
    PIERCING,
};

typedef struct Card{
    Vector2 pos;
    Vector2 center;
    Vector2 speed;
    int type;
    Vector2 size;
    Color color;
    float rotation;
    float rotationSpeed;
    bool isAlive;
    bool isMoving;
    bool isPiercing;
} CARD;

//----------------------------------------------------------------------------------
// CardHandler Definition
//----------------------------------------------------------------------------------
typedef struct CardHandler{
    CARD cardArray[MAX_CARDS];
    Vector2 initialMousePos;
    float shotTimeAccumulator;
    int selectedType;
    bool isShooting;
    int currentCardIdx;
} CARD_HANDLER;

CARD_HANDLER cardHandler = {
    .initialMousePos = ZERO_VECTOR,
    .shotTimeAccumulator = 0.0f,
    .currentCardIdx = 0,
    .isShooting = false
    };

//----------------------------------------------------------------------------------
// Enemy Definition
//----------------------------------------------------------------------------------

typedef struct Enemy{
    Vector2 size;
    Color color;
    Vector2 pos;
    Vector2 center;
    Vector2 curr_speed;
    float speed;
    bool isAlive;
    int hp;
    int damage;
    float t_iframes;
    
} ENEMY;

typedef struct EnemyHandler{
    ENEMY enemyArray[MAX_ENEMIES];
    int currentIndex;
    float spawn_timer;
    float variance;
    float curr_spawn_time;
    float avg_spawn_interval;
    int difficulty;
} ENEMY_HANDLER;

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

// Player Functions
//----------------------------------------------------------------------------------
void UpdatePlayer(PLAYER * p);
void DrawPlayer(const PLAYER *p);

// Card Functions
//----------------------------------------------------------------------------------
void HandleCardThrow(CARD_HANDLER *ch);
void CreateCard(CARD *c, int cardType);
void ThrowCard(Vector2 origin, Vector2 speed, CARD_HANDLER *ch);
void MoveCard(CARD *c);
void RotateCard(CARD *c);
void UpdateCards(CARD_HANDLER *ch);
void DrawCards(CARD_HANDLER *ch);

// Enemy Functions
//----------------------------------------------------------------------------------
void MoveEnemy(ENEMY *e, Vector2 target);
void MoveEnemies(ENEMY_HANDLER *eh);
void DrawEnemies(ENEMY_HANDLER *eh);
void SpawnEnemy(ENEMY_HANDLER *eh);
void SpawnEnemies(ENEMY_HANDLER *eh);

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

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, fps, 1);
#else
    SetTargetFPS(DRAW_FPS); // Set our game maximum FPS
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        
        // Logic
        //--------------------------------------------------------------------------------------
        logicAcc += GetFrameTime();

        if(logicAcc>LOGIC_FRAME_TIME){
            UpdatePlayer(&player);
            UpdateCards(&cardHandler);
            MoveEnemies(&enemyHandler);
            camera.target = (Vector2){player.pos.x + PLAYER_SIZE / 2, player.pos.y + PLAYER_SIZE / 2};
            logicAcc -= LOGIC_FRAME_TIME;

        }
        SpawnEnemies(&enemyHandler);
        HandleCardThrow(&cardHandler);
        CheckCardEnemyCollisions(&cardHandler, &enemyHandler);

        // Draw
        //--------------------------------------------------------------------------------------
        BeginDrawing();
        BeginMode2D(camera);

        ClearBackground(bgColor);
        DrawCards(&cardHandler);
        DrawPlayer(&player);
        DrawEnemies(&enemyHandler);
        EndMode2D();

        // UI
        //--------------------------------------------------------------------------------------
        DrawFPS(20,20);
        

        //--------------------------------------------------------------------------------------
        
        EndDrawing();
    }
#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

void DrawPlayer(const PLAYER *p){
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

    return;
}

void DrawCards(CARD_HANDLER *ch){
    for (int i = 0; i < MAX_CARDS; i++){
        CARD *c = &ch->cardArray[i];
        if (!c->isAlive) continue;

        Rectangle rect = {c->pos.x,
                          c->pos.y,
                          c->size.x,
                          c->size.y};
        DrawRectanglePro(rect, c->center, c->rotation, c->color);
    }
    return;
}

void UpdateCards(CARD_HANDLER *ch){
    for (int i = 0; i < MAX_CARDS; i++)
    {
        CARD *c = &ch->cardArray[i];
        if (!c->isMoving) continue;
        if (!c->isAlive) continue;

        MoveCard(c);
        RotateCard(c);
    }
}

void MoveCard(CARD *c){
    c->pos.x += c->speed.x;
    c->pos.y += c->speed.y;
    c->speed.x *= 0.95f;
    c->speed.y *= 0.95f;
    if (Vector2DistanceSqr(c->speed, (Vector2)ZERO_VECTOR)<0.01f) c->isMoving = false;
}

void RotateCard(CARD *c){
    c->rotation += c->rotationSpeed;
    c->rotationSpeed *= 0.95f;
}

void CreateCard(CARD *c, int cardType){
    switch (cardType)
    {
    case NORMAL:
        c->center = (Vector2){CARD_SIZE / 2, CARD_SIZE / 2};
        c->type = cardType;
        c->size = (Vector2){CARD_SIZE, CARD_SIZE};
        c->color = RED;
        break;
    
    default:
        break;
    }
    
}

void ThrowCard(Vector2 origin, Vector2 speed, CARD_HANDLER *ch){
    CARD *c = &ch->cardArray[ch->currentCardIdx];
    CreateCard(c, NORMAL);
    c->pos = origin;
    c->speed = speed;
    c->rotation = (float)GetRandomValue(0, 360);
    c->rotationSpeed = (float)GetRandomValue(30, 200);
    c->isAlive = true;
    c->isMoving = true;

    ch->currentCardIdx += 1;
    ch->currentCardIdx %= MAX_CARDS;
}

void DrawEnemies(ENEMY_HANDLER *eh){
    for (int i = 0; i < MAX_ENEMIES; i++){
        ENEMY *e = &eh->enemyArray[i];
        if(e->isAlive)
            DrawRectangleV(e->pos, e->size, e->color);
    }
}

void MoveEnemy(ENEMY *e, Vector2 target)
{
    Vector2 player_center = Vector2Add(player.pos, player.center);
    Vector2 enemy_center = Vector2Add(e->pos, e->center);
    Vector2 direction = Vector2Subtract(player_center, enemy_center);
    e->curr_speed = Vector2Scale(Vector2Normalize(direction), e->speed);
    e->pos = Vector2Add(e->pos, e->curr_speed);
}

void MoveEnemies(ENEMY_HANDLER *eh){
    for (int i = 0; i < MAX_ENEMIES; i++){
        ENEMY *enemy = &eh->enemyArray[i];
        if (!enemy->isAlive) continue;
        
        MoveEnemy(enemy, player.pos);
    }
}

void SpawnEnemy(ENEMY_HANDLER *eh)
{
    eh->enemyArray[eh->currentIndex] = (ENEMY){
        .pos = ZERO_VECTOR,
        .size = {30.0f, 30.0f},
        .center = {15.0f, 15.0f},
        .speed = GetRandomValue(1, eh->difficulty * 2),
        .hp = 100,
        .color = DARKPURPLE,
        .isAlive = true};
    eh->currentIndex++;
    eh->currentIndex %= MAX_ENEMIES;
}

void SpawnEnemies(ENEMY_HANDLER *eh){
    eh->spawn_timer += GetFrameTime();

    if (eh->spawn_timer > eh->curr_spawn_time)
    {
        SpawnEnemy(eh);

        eh->spawn_timer    -= eh->curr_spawn_time;
        eh->curr_spawn_time = eh->avg_spawn_interval + (float)GetRandomValue(-100, 100) / 100.0f * eh->variance;
    }
}

void HandleCardThrow(CARD_HANDLER *ch){
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        ch->isShooting = true;
        ch->initialMousePos = GetMousePosition();
    }

    if (ch->isShooting)
        ch->shotTimeAccumulator += GetFrameTime();

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        Vector2 cardDirection = Vector2Subtract(GetMousePosition(), ch->initialMousePos);
        Vector2 cardSpeed = Vector2Scale(cardDirection, 1 / ch->shotTimeAccumulator * CARD_SPEED_MULT);
        Vector2 playerCenter = Vector2Add(player.pos, player.center);
        ThrowCard(playerCenter, cardSpeed, ch);
        ch->shotTimeAccumulator = 0.0f;
        ch->isShooting = false;
    }
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
            bool isColliding = CheckCollisionCircleRec(cardCenter, c->size.x/2*SQRT2, enemy_rect);
            if (isColliding){
                c->isAlive = false;
                int damage = (int)(Vector2Length(c->speed) * DMG_MULT);
                e->hp -= damage;
                return;
            }
        }
    }
}

