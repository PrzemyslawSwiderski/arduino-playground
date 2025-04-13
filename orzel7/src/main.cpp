#include <Arduino.h>
#include "esp_camera.h"
#include "pins.h"
#include "cameraMod.h"
#include "roverMod.h"
#include "wifiMod.h"
#include "webSocketMod.h"
#include "esp_log.h"

static const char *TAG = "main";

// Checks if motion was detected, sets LED HIGH and starts a timer
void IRAM_ATTR detectsMovement()
{
  ESP_EARLY_LOGI(TAG, "Motion detected! Taking photo...");
}

void setup()
{
  Serial.begin(115200);

  pinMode(LED_LIGHT_GPIO_NUM, OUTPUT);
  pinMode(RED_LIGHT_GPIO_NUM, OUTPUT);
  setupRoverMod();
  setupCameraMod();

  setupWifiMod();
  setupWebSocketMod();

  digitalWrite(RED_LIGHT_GPIO_NUM, LOW);

  pinMode(PIR_SENSOR_PIN, INPUT_PULLUP);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(PIR_SENSOR_PIN), detectsMovement, RISING);

  ESP_LOGI(TAG, "Waiting for PIR sensor to stabilize (30s)...");
  // delay(30000); // HC-SR501 needs ~30-60s to stabilize after power-on
}

void loop()
{
}
