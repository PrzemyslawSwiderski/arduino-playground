#include "pins.h"
#include "Arduino.h"
#include "robot.h"

static int speed = 150;
static const int freq = 2000;
static const int lresolution = 8;

void setup_robot() {
  digitalWrite(LED_LIGHT_GPIO_NUM, LOW);

  ledcAttach(LEFT_BACK_MOTOR, freq, lresolution); /* 2000 hz PWM, 8-bit resolution and range from 0 to 255 */
  ledcAttach(LEFT_FRONT_MOTOR, freq, lresolution);
  ledcAttach(RIGHT_BACK_MOTOR, freq, lresolution);
  ledcAttach(RIGHT_FRONT_MOTOR, freq, lresolution);

  robot_stop();
}

void robot_stop() {
  ledcWrite(LEFT_BACK_MOTOR, 0);
  ledcWrite(LEFT_FRONT_MOTOR, 0);
  ledcWrite(RIGHT_BACK_MOTOR, 0);
  ledcWrite(RIGHT_FRONT_MOTOR, 0);
}

void robot_fwd() {
  ledcWrite(LEFT_BACK_MOTOR, 0);
  ledcWrite(LEFT_FRONT_MOTOR, speed);
  ledcWrite(RIGHT_BACK_MOTOR, 0);
  ledcWrite(RIGHT_FRONT_MOTOR, speed);
}

void robot_back() {
  ledcWrite(LEFT_BACK_MOTOR, speed);
  ledcWrite(LEFT_FRONT_MOTOR, 0);
  ledcWrite(RIGHT_BACK_MOTOR, speed);
  ledcWrite(RIGHT_FRONT_MOTOR, 0);
}

void robot_right() {
  ledcWrite(LEFT_BACK_MOTOR, 0);
  ledcWrite(LEFT_FRONT_MOTOR, speed);
  ledcWrite(RIGHT_BACK_MOTOR, speed);
  ledcWrite(RIGHT_FRONT_MOTOR, 0);
}

void robot_left() {
  ledcWrite(LEFT_BACK_MOTOR, speed);
  ledcWrite(LEFT_FRONT_MOTOR, 0);
  ledcWrite(RIGHT_BACK_MOTOR, 0);
  ledcWrite(RIGHT_FRONT_MOTOR, speed);
}