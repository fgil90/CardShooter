#include "raylib.h"
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define DRAW_FPS 60
#define MAX_CARDS 100
#define CARD_SPEED_MULT 1/25.0f
#define PLAYER_SIZE 64

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------

float logic_fps = 60.0f;

Color bgColor = WHITE;
Vector2 playerSize = {(float)PLAYER_SIZE, (float)PLAYER_SIZE};
Vector2 cardSize = {16.0f, 16.0f};
Color cardColor = RED;

typedef struct Player
{
    Vector2 pos;
    Vector2 center;
    float speed;
    Color color;
    bool isMoving;
    bool isAttacking;
} PLAYER;

typedef struct Card
{
    Vector2 pos;
    Vector2 center;
    Vector2 speed;
    float rotation;
    float rotationSpeed;
    bool isAlive;
} CARD;

CARD cardArray[MAX_CARDS];
int currentCardIdx = 0;

PLAYER player = {
    .pos = (Vector2){0.0f, 0.0f},
    .center = (Vector2){(float)(PLAYER_SIZE/2), (float)(PLAYER_SIZE/2)},
    .speed = 5.0f,
    .color = BLACK,
    .isMoving = false,
    .isAttacking = false
};

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------

void UpdatePlayer(PLAYER *p);
void DrawPlayer(const PLAYER *p);
void CardThrowHandler(Vector2 *initialMousePos, float *shotTimeAccumulator, bool *isShooting);
void ThrowCard(Vector2 origin, Vector2 speed, CARD *cardArray, int *currentCardIdx);
void MoveCard(CARD *c);
void RotateCard(CARD *c);
void UpdateCards(CARD *cardArray, int n_cards);
void DrawCards(const CARD *cardArray, int n_cards);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Zeldinha de Carta");
    bool isShooting = false;
    float shotTimeAccumulator = 0.0f;
    Vector2 initialMousePos = {0.0f, 0.0f};

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
        UpdatePlayer(&player);
        CardThrowHandler(&initialMousePos, &shotTimeAccumulator, &isShooting);
        UpdateCards(cardArray, MAX_CARDS);

        // Draw
        BeginDrawing();

        ClearBackground(bgColor);
        DrawCards(cardArray, MAX_CARDS);
        DrawPlayer(&player);

        EndDrawing();
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

void DrawCards(const CARD *cardArray, int n_cards)
{
    for (int i = 0; i < n_cards; i++){
        if (!cardArray[i].isAlive)
            continue;

        Rectangle rect = {cardArray[i].pos.x,
                          cardArray[i].pos.y,
                          cardSize.x,
                          cardSize.y};
        DrawRectanglePro(rect, cardArray[i].center, cardArray[i].rotation, cardColor);
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

void UpdateCards(CARD *cardArray, int n_cards){
    for (int i = 0; i < n_cards; i++)
    {
        CARD *c = &cardArray[i];
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

void CardThrowHandler(Vector2 *initialMousePos, float *shotTimeAccumulator, bool *isShooting)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        *isShooting = true;
        *initialMousePos = GetMousePosition();
    }

    if (isShooting)
        *shotTimeAccumulator += GetFrameTime();

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        Vector2 cardSpeed = {CARD_SPEED_MULT * (GetMousePosition().x - initialMousePos->x) / *shotTimeAccumulator,
                             CARD_SPEED_MULT * (GetMousePosition().y - initialMousePos->y) / *shotTimeAccumulator};
        Vector2 playerCenter = {player.pos.x + player.center.x, player.pos.y + player.center.y};
        ThrowCard(playerCenter, cardSpeed, cardArray, &currentCardIdx);
        *shotTimeAccumulator = 0.0f;
        *isShooting = false;
    }
}

void ThrowCard(Vector2 origin, Vector2 speed, CARD *cardArray, int *currentCardIdx){
    cardArray[*currentCardIdx] = (CARD){
        .pos = origin,
        .center = (Vector2){cardSize.x / 2, cardSize.y / 2},
        .speed = speed,
        .rotation = (float)GetRandomValue(0, 360),
        .rotationSpeed = (float)GetRandomValue(30, 200),
        .isAlive = true
    };

    *currentCardIdx += 1;
    *currentCardIdx %= MAX_CARDS;
}
