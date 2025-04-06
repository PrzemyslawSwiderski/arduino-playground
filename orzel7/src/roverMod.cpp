#include <Arduino.h>
#include "pins.h"
#include "roverMod.h"
#include "utilsMod.h"

static int speed = 150;
static const int freq = 2000;
static const int lresolution = 8;
static unsigned long lastMovementTime = 0;
static const unsigned long AUTO_STOP_DELAY_TIME = 1000;  // 1 seconds

void setupRoverMod() {
  digitalWrite(LED_LIGHT_GPIO_NUM, LOW);

  ledcAttach(LEFT_BACK_MOTOR, freq, lresolution); /* 2000 hz PWM, 8-bit resolution and range from 0 to 255 */
  ledcAttach(LEFT_FRONT_MOTOR, freq, lresolution);
  ledcAttach(RIGHT_BACK_MOTOR, freq, lresolution);
  ledcAttach(RIGHT_FRONT_MOTOR, freq, lresolution);
}

void roverStop() {
  ledcWrite(LEFT_BACK_MOTOR, 0);
  ledcWrite(LEFT_FRONT_MOTOR, 0);
  ledcWrite(RIGHT_BACK_MOTOR, 0);
  ledcWrite(RIGHT_FRONT_MOTOR, 0);
}

void roverFwd() {
  ledcWrite(LEFT_BACK_MOTOR, 0);
  ledcWrite(LEFT_FRONT_MOTOR, speed);
  ledcWrite(RIGHT_BACK_MOTOR, 0);
  ledcWrite(RIGHT_FRONT_MOTOR, speed);
  lastMovementTime = millis();
}

void roverBack() {
  ledcWrite(LEFT_BACK_MOTOR, speed);
  ledcWrite(LEFT_FRONT_MOTOR, 0);
  ledcWrite(RIGHT_BACK_MOTOR, speed);
  ledcWrite(RIGHT_FRONT_MOTOR, 0);
  lastMovementTime = millis();
}

void roverRight() {
  ledcWrite(LEFT_BACK_MOTOR, 0);
  ledcWrite(LEFT_FRONT_MOTOR, speed);
  ledcWrite(RIGHT_BACK_MOTOR, speed);
  ledcWrite(RIGHT_FRONT_MOTOR, 0);
  lastMovementTime = millis();
}

void roverLeft() {
  ledcWrite(LEFT_BACK_MOTOR, speed);
  ledcWrite(LEFT_FRONT_MOTOR, 0);
  ledcWrite(RIGHT_BACK_MOTOR, 0);
  ledcWrite(RIGHT_FRONT_MOTOR, speed);
  lastMovementTime = millis();
}

void ledOn() {
  digitalWrite(LED_LIGHT_GPIO_NUM, HIGH);
}

void ledOff() {
  digitalWrite(LED_LIGHT_GPIO_NUM, LOW);
}

static void autoStop() {
  unsigned long currentTime = millis();
  if (isTimeAfter(currentTime, lastMovementTime, AUTO_STOP_DELAY_TIME)) {
    roverStop();
  }
}

void loopRoverMod() {
  autoStop();
}
