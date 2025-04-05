#include "esp_camera.h"
#include "pins.h"
#include "cameraMod.h"
#include "roverMod.h"
#include "wifiMod.h"
#include "webSocketMod.h"

extern void startCameraServer();

void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  Serial.println();

  pinMode(LED_LIGHT_GPIO_NUM, OUTPUT);
  pinMode(RED_LIGHT_GPIO_NUM, OUTPUT);
  setupRoverMod();
  setupCameraMod();

  setupWifiMod();
  // startCameraServer();
  setupWebSocketMod();

  digitalWrite(RED_LIGHT_GPIO_NUM, LOW);
}

void loop() {
  loopWebSocketMod();
  loopRoverMod();
}
