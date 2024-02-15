
//----------------------------------------------------------------------------------
// TODO:
// - [x] Camera
// - [ ] Enemies
// - [ ] Throw curved cards
// - [ ] Different Types of cards
// - [ ] Cycle card types with Q and E

#include "raylib.h"
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define DRAW_FPS 60

#define ZERO_VECTOR {0.0f, 0.0f}
#define SCREEN_CENTER {(float)SCREEN_WIDTH/2, (float)SCREEN_HEIGHT/2}

#define MAX_CARDS 100
#define MAX_ENEMIES 100

#define PLAYER_SIZE 64
#define CARD_SIZE 16
#define CARD_SPEED_MULT 1/25.0f

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Variables Definition
//----------------------------------------------------------------------------------

Color bgColor = WHITE;
Vector2 playerSize = {(float)PLAYER_SIZE, (float)PLAYER_SIZE};


// THIS WILL BECOME OBSOLETE AFTER I HAVE DIFFERENT TYPES OF CARDS
Vector2 cardSize = {(float)CARD_SIZE, (float)CARD_SIZE}; 
Color cardColor = RED;

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

typedef struct Card{
    Vector2 pos;
    Vector2 center;
    Vector2 speed;
    float rotation;
    float rotationSpeed;
    bool isAlive;
} CARD;

//----------------------------------------------------------------------------------
// CardHandler Definition
//----------------------------------------------------------------------------------
typedef struct CardHandler{
    CARD cardArray[MAX_CARDS];
    Vector2 initialMousePos;
    float shotTimeAccumulator;
    int currentCardIdx;
    bool isShooting;
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
    Vector2 pos;
    Vector2 speed;
    int hp;
    int damage;
} ENEMY;

typedef struct EnemyHandler{
    ENEMY enemyArray[MAX_ENEMIES];
} ENEMY_HANDLER;

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
void ThrowCard(Vector2 origin, Vector2 speed, CARD_HANDLER *ch);
void MoveCard(CARD *c);
void RotateCard(CARD *c);
void UpdateCards(CARD_HANDLER *ch);
void DrawCards(CARD_HANDLER *ch);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Zeldinha de Carta");
    
    // Camera
    Camera2D camera = {
        .offset = SCREEN_CENTER,
        .target = (Vector2){player.pos.x + PLAYER_SIZE / 2, player.pos.y + PLAYER_SIZE / 2},
        .rotation = 0.0f,
        .zoom = 1.0f};

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

        UpdatePlayer(&player);
        HandleCardThrow(&cardHandler);
        UpdateCards(&cardHandler);
        camera.target = (Vector2){player.pos.x + PLAYER_SIZE / 2, player.pos.y + PLAYER_SIZE / 2};

        // Draw
        //--------------------------------------------------------------------------------------
        BeginDrawing();
        BeginMode2D(camera);

        ClearBackground(bgColor);
        DrawCards(&cardHandler);
        DrawPlayer(&player);

        EndMode2D();
        EndDrawing();
        
        //--------------------------------------------------------------------------------------
    }
#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

void DrawPlayer(const PLAYER *p)
{
    DrawRectangleV(p->pos, playerSize, p->color);
    return;
}

void DrawCards(CARD_HANDLER *ch)
{
    for (int i = 0; i < MAX_CARDS; i++){
        if (!ch->cardArray[i].isAlive)
            continue;

        Rectangle rect = {ch->cardArray[i].pos.x,
                          ch->cardArray[i].pos.y,
                          cardSize.x,
                          cardSize.y};
        DrawRectanglePro(rect, ch->cardArray[i].center, ch->cardArray[i].rotation, cardColor);
    }
    return;
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

void UpdateCards(CARD_HANDLER *ch){
    for (int i = 0; i < MAX_CARDS; i++)
    {
        CARD *c = &ch->cardArray[i];
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
}

void RotateCard(CARD *c){
    c->rotation += c->rotationSpeed;
    c->rotationSpeed *= 0.95f;
}

void HandleCardThrow(CARD_HANDLER *ch)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        ch->isShooting = true;
        ch->initialMousePos = GetMousePosition();
    }

    if (ch->isShooting)
        ch->shotTimeAccumulator += GetFrameTime();

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        Vector2 cardSpeed = {CARD_SPEED_MULT * (GetMousePosition().x - ch->initialMousePos.x) / ch->shotTimeAccumulator,
                             CARD_SPEED_MULT * (GetMousePosition().y - ch->initialMousePos.y) / ch->shotTimeAccumulator};
        Vector2 playerCenter = {player.pos.x + player.center.x, player.pos.y + player.center.y};
        ThrowCard(playerCenter, cardSpeed, ch);
        ch->shotTimeAccumulator = 0.0f;
        ch->isShooting = false;
    }
}

void ThrowCard(Vector2 origin, Vector2 speed, CARD_HANDLER *ch){
    ch->cardArray[ch->currentCardIdx] = (CARD){
        .pos = origin,
        .center = (Vector2){cardSize.x / 2, cardSize.y / 2},
        .speed = speed,
        .rotation = (float)GetRandomValue(0, 360),
        .rotationSpeed = (float)GetRandomValue(30, 200),
        .isAlive = true
    };

    ch->currentCardIdx += 1;
    ch->currentCardIdx %= MAX_CARDS;
}
