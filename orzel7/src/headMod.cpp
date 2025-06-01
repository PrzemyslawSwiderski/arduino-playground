#include <Arduino.h>
#include "pcaMod.h"
#include "utilsMod.h"
#include "pins.h"

void rotateStop()
{
    moveHead(YAW_CHANNEL, 50);
    moveHead(PITCH_CHANNEL, 50);
}

void rotateLeft(int speed)
{
    moveHead(YAW_CHANNEL, 0);
    delay(50);
    rotateStop();
}

void rotateRight(int speed)
{
    moveHead(YAW_CHANNEL, 100);
    delay(50);
    rotateStop();
}

void rotateUp(int speed)
{
    moveHead(PITCH_CHANNEL, 0);
    delay(50);
    rotateStop();
}

void rotateDown(int speed)
{
    moveHead(PITCH_CHANNEL, 100);
    delay(50);
    rotateStop();
}
