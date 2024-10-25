#pragma once
#include "raylib.h"
#include "raymath.h"

#define MAX_CARDS 100
#define CARD_SIZE 16
#define CARD_SPEED_MULT 1 / 25.0f

//----------------------------------------------------------------------------------
// Card Definition
//----------------------------------------------------------------------------------

enum cardTypes
{
    NORMAL,
    PIERCING,
};

typedef struct Card
{
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
typedef struct CardHandler
{
    CARD cardArray[MAX_CARDS];
    Vector2 initialMousePos;
    float shotTimeAccumulator;
    int selectedType;
    bool isShooting;
    int currentCardIdx;
} CARD_HANDLER;

// Card Functions
//----------------------------------------------------------------------------------
void HandleCardThrow(CARD_HANDLER *ch, Vector2 origin); 
void SpawnCard(CARD *c, int cardType);                  
void ThrowCard(Vector2 origin, Vector2 speed, CARD_HANDLER *ch); 
void MoveCard(CARD *c);
void RotateCard(CARD *c);
void UpdateCards(CARD_HANDLER *ch);
void DrawCards(CARD_HANDLER *ch);