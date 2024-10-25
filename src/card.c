#include "card.h"
#include "player.h"

void HandleCardThrow(CARD_HANDLER *ch, Vector2 origin)
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
        Vector2 cardDirection = Vector2Subtract(GetMousePosition(), ch->initialMousePos);
        Vector2 cardSpeed = Vector2ClampValue(Vector2Scale(cardDirection, 1 / ch->shotTimeAccumulator * CARD_SPEED_MULT), 1.0f, 50.0f);
        ThrowCard(origin, cardSpeed, ch);
        ch->shotTimeAccumulator = 0.0f;
        ch->isShooting = false;
    }
}

void SpawnCard(CARD *c, int cardType)
{
    switch (cardType)
    {
    case NORMAL:
        c->center = (Vector2){CARD_SIZE / 2, CARD_SIZE / 2};
        c->type = cardType;
        c->size = (Vector2){CARD_SIZE, CARD_SIZE};
        c->color = RED;
        break;
        // TODO: Add card types

    default:
        break;
    }
}

void DrawCards(CARD_HANDLER *ch)
{
    for (int i = 0; i < MAX_CARDS; i++)
    {
        CARD *c = &ch->cardArray[i];
        if (!c->isAlive)
            continue;

        Rectangle rect = {c->pos.x,
                          c->pos.y,
                          c->size.x,
                          c->size.y};
        DrawRectanglePro(rect, c->center, c->rotation, c->color);
    }
    return;
}

void UpdateCards(CARD_HANDLER *ch)
{
    for (int i = 0; i < MAX_CARDS; i++)
    {
        CARD *c = &ch->cardArray[i];
        if (!c->isMoving)
            continue;
        if (!c->isAlive)
            continue;

        MoveCard(c);
        RotateCard(c);
    }
}

void MoveCard(CARD *c)
{
    c->pos.x += c->speed.x;
    c->pos.y += c->speed.y;
    c->speed.x *= 0.95f;
    c->speed.y *= 0.95f;
    if (Vector2DistanceSqr(c->speed, Vector2Zero()) < 0.01f)
        c->isMoving = false;
}

void RotateCard(CARD *c)
{
    c->rotation += c->rotationSpeed;
    c->rotationSpeed *= 0.95f;
}

void ThrowCard(Vector2 origin, Vector2 speed, CARD_HANDLER *ch)
{
    CARD *c = &ch->cardArray[ch->currentCardIdx];
    SpawnCard(c, NORMAL);
    c->pos = origin;
    c->speed = speed;
    c->rotation = (float)GetRandomValue(0, 360);
    c->rotationSpeed = (float)GetRandomValue(30, 200);
    c->isAlive = true;
    c->isMoving = true;

    ch->currentCardIdx += 1;
    ch->currentCardIdx %= MAX_CARDS;
}