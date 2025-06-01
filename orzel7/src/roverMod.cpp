#include <Arduino.h>
#include "pins.h"
#include "roverMod.h"
#include "utilsMod.h"
#include "pcaMod.h"
#include "esp_log.h"

static const char *TAG = "roverMod";

static unsigned long headLastRotateTime = 0;
static const unsigned long HEAD_AUTO_STOP_DELAY_TIME = 500; // time to auto stop in ms

void roverStop()
{
  stopWheels(LEFT_BACK_MOTOR_CHANNEL);
  stopWheels(LEFT_FRONT_MOTOR_CHANNEL);
  stopWheels(RIGHT_BACK_MOTOR_CHANNEL);
  stopWheels(RIGHT_FRONT_MOTOR_CHANNEL);
}

void roverFwd(int speed)
{
  moveWheels(RIGHT_FRONT_MOTOR_CHANNEL, speed);
  moveWheels(LEFT_FRONT_MOTOR_CHANNEL, speed);
  stopWheels(RIGHT_BACK_MOTOR_CHANNEL);
  stopWheels(LEFT_BACK_MOTOR_CHANNEL);
  headLastRotateTime = millis();
}

void roverBack(int speed)
{
  moveWheels(RIGHT_BACK_MOTOR_CHANNEL, speed);
  moveWheels(LEFT_BACK_MOTOR_CHANNEL, speed);
  stopWheels(RIGHT_FRONT_MOTOR_CHANNEL);
  stopWheels(LEFT_FRONT_MOTOR_CHANNEL);
  headLastRotateTime = millis();
}

void roverRight(int speed)
{
  moveWheels(RIGHT_BACK_MOTOR_CHANNEL, speed);
  moveWheels(LEFT_FRONT_MOTOR_CHANNEL, speed);
  stopWheels(RIGHT_FRONT_MOTOR_CHANNEL);
  stopWheels(LEFT_BACK_MOTOR_CHANNEL);
  headLastRotateTime = millis();
}

void roverLeft(int speed)
{
  moveWheels(RIGHT_FRONT_MOTOR_CHANNEL, speed);
  moveWheels(LEFT_BACK_MOTOR_CHANNEL, speed);
  stopWheels(RIGHT_BACK_MOTOR_CHANNEL);
  stopWheels(LEFT_FRONT_MOTOR_CHANNEL);
  headLastRotateTime = millis();
}

void ledOn()
{
  digitalWrite(LED_LIGHT_GPIO_NUM, HIGH);
}

void ledOff()
{
  digitalWrite(LED_LIGHT_GPIO_NUM, LOW);
}

void roverToSleep()
{
  roverStop();
}

static void autoStopTask(void *pvParameters)
{
  const TickType_t checkInterval = pdMS_TO_TICKS(50);
  while (1)
  {
    unsigned long currentTime = millis();
    if (isTimeAfter(currentTime, headLastRotateTime, HEAD_AUTO_STOP_DELAY_TIME))
    {
      ESP_LOGD(TAG, "Auto stopping the rover");
      roverStop();
    }
    vTaskDelay(checkInterval);
  }
}

void setupRoverMod()
{
  pinMode(LED_LIGHT_GPIO_NUM, OUTPUT);
  digitalWrite(LED_LIGHT_GPIO_NUM, LOW);

  // Create autoStop task
  xTaskCreate(
      autoStopTask,   // Task function
      "AutoStopTask", // Task name
      2048,           // Stack size (bytes)
      NULL,           // Task parameter
      0,              // Priority (same or lower than roverFwd task)
      NULL);          // Task handle
}
